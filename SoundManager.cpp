//********** H1 - INCLUDES **********
#include "AudioData.h"
#include "AudioManager.h"

//********** H1 - AUDIOMANAGER NAMESPACE **********
namespace AudioManager
{
	//********** H2 - Loaders and Play Functions **********
	int SoundManager::LoadSound(const std::string& path, bool looping, float initVolume, float pitch)
	{
		if (!IsTypeSupported(path))
			return UNSUPPORTED_FILE;

		if (audioMap.size() < AUDIO_LIMIT)
		{
			SoundData ad{looping, initVolume, pitch, path};
			bool loadSuccess = ad.soundBuffer.loadFromFile(path);
			if (!loadSuccess)
				return FILE_NOT_FOUND;
			ad.ID = GenerateID();
			ad.looping = looping;
			audioMap[ad.ID] = ad;
			return audioMap[ad.ID].ID;
		}
		else
			return AUDIO_LIMIT_EXCEEDED;

		return SUCCESS;
	}
	int SoundManager::UnloadSound(audioID_t audioID)
	{
		for (auto iter = audioMap.begin(); iter != audioMap.end(); iter++)
		{
			delete iter->second.audio;
			audioMap.erase(iter);
			return SUCCESS;
		}
		return INVALID_AUDIO_ID;
	}
	int SoundManager::MixSounds(bool looping, float initVolume, float pitch, int sampleRate, int numArgs, ...)
	{
		//pass sampleRate as -1 to return sound with averageSampleRate
		sf::Uint64 mixedSamplesCount = 0;
		std::vector<int> vaArgIDVec;
		va_list argList;
		int averageSampleRate = 0; // Only used is matchSampleRate is true

		//***** H2 - Find mixedSampleCount & Get averageSampleRate *****
		va_start(argList, numArgs);
		for (int i = 0; i < numArgs; i++)
		{
			int soundID = va_arg(argList, int);
			SoundData* ad = ReturnAudioData(soundID);
			if (ad != nullptr)
			{
				vaArgIDVec.push_back(soundID);
				if (ad->soundBuffer.getSampleCount() > mixedSamplesCount)
					mixedSamplesCount = ad->soundBuffer.getSampleCount();
				if (sampleRate == -1)
					averageSampleRate += ad->soundBuffer.getSampleRate();
			}
			else
				return INVALID_AUDIO_ID;
		}
		if (sampleRate == -1)
			averageSampleRate /= numArgs;

		//***** H2 - Create The sampleMatrix *****
		const std::int16_t** sampleMatrix = new const std::int16_t * [mixedSamplesCount];
		for (int i = 0; i < vaArgIDVec.size(); i++)
		{
			int soundID = vaArgIDVec[i];
			SoundData* ad = ReturnAudioData(soundID);
			memcpy(&sampleMatrix[i], ad->soundBuffer.getSamples(), sizeof(std::int16_t));
		}

		//***** H2 - Create The sampleMatrix *****
		std::int16_t* mixedSamples = new std::int16_t[mixedSamplesCount];
		for (int i = 0; i < mixedSamplesCount; i++)
		{
			std::int16_t totalSample = 0;
			for (int j = 0; j < vaArgIDVec.size(); j++)
			{
				SoundData* ad = ReturnAudioData(vaArgIDVec[j]);
				if (i < ad->soundBuffer.getSampleCount())
				{
					std::int16_t num = ad->soundBuffer.getSamples()[i];
					totalSample += num;
				}
			}
			mixedSamples[i] = totalSample;
		}

		//***** H2 - Generate Sound *****
		int returnID;
		if (audioMap.size() < AUDIO_LIMIT)
		{
			SoundData ad{ looping, initVolume, pitch };
			if (sampleRate == -1)
				ad.soundBuffer.loadFromSamples(mixedSamples, mixedSamplesCount, 2, averageSampleRate / 2);
			else
				ad.soundBuffer.loadFromSamples(mixedSamples, mixedSamplesCount, 2, sampleRate / 2);
			ad.ID = GenerateID();
			returnID = ad.ID;
			ad.looping = looping;

			audioMap[ad.ID] = ad;
			return audioMap[ad.ID].ID;
		}
		else
			return AUDIO_LIMIT_EXCEEDED;

		return returnID;

	}
	int SoundManager::PlaySound(audioID_t audioID)
	{
		SoundData* ad = ReturnAudioData(audioID);

		if (ad != nullptr)
		{
			sf::Sound* sound = new sf::Sound();
			sound->setBuffer(ad->soundBuffer);
			sound->setVolume(ad->volume);
			sound->setPitch(ad->pitch);
			sound->setLoop(ad->looping);
			ad->audio = sound;
			sound->play();
			ad->played = true;
		}
		else
			return INVALID_AUDIO_ID;

		return SUCCESS;
	}
	int SoundManager::PlayAll()
	{
		for (auto& i : audioMap)
		{
			sf::Sound* sound = new sf::Sound();
			sound->setBuffer(i.second.soundBuffer);
			sound->setVolume(i.second.volume);
			sound->setPitch(i.second.pitch);
			sound->setLoop(i.second.looping);
			i.second.audio = sound;
			sound->play();
			i.second.played = true;
		}
		return SUCCESS;
	}
}