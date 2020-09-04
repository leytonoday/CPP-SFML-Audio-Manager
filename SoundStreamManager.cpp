//********** H1 - INCLUDES **********
#include "AudioData.h"
#include "AudioManager.h"

//********** H1 - AUDIOMANAGER NAMESPACE **********
namespace AudioManager
{
	//***** H2- Loaders and Play Functions *****
	int SoundStreamManager::OpenStream(const std::string& path, bool looping, float initVolume, float pitch)
	{
		if (!IsTypeSupported(path))
			return UNSUPPORTED_FILE;

		if (audioMap.size() >= AUDIO_LIMIT)
			return AUDIO_LIMIT_EXCEEDED;

		SoundStreamData as{looping, initVolume, pitch, path};
		as.ID = GenerateID();
		as.looping = looping;
		audioMap[as.ID] = as;
		return audioMap[as.ID].ID;
	}
	int SoundStreamManager::CloseStream(audioID_t audioID)
	{
		SoundStreamData* ad = ReturnAudioData(audioID);

		if (ad == nullptr)
			return INVALID_AUDIO_ID;

		auto result = audioMap.find(audioID);
		delete result->second.audio;
		audioMap.erase(result);
		return SUCCESS;
	}
	int SoundStreamManager::PlayStream(audioID_t audioID)
	{
		SoundStreamData* as = ReturnAudioData(audioID);

		if (as == nullptr)
			return INVALID_AUDIO_ID;

		if (!as->played)
		{
			as->audio = new sf::Music();
			bool openSuccess = as->audio->openFromFile(as->path);
			as->audio->setVolume(as->volume);
			as->audio->setPitch(as->pitch);
			as->audio->setLoop(as->looping);
			as->played = true;
		}

		as->audio->play();

		return SUCCESS;
	}
	int SoundStreamManager::PlayAll()
	{
		for (auto& [key, audioData] : audioMap)
		{
			if (!audioData.played)
			{
				audioData.audio = new sf::Music();
				bool openSuccess = audioData.audio->openFromFile(audioData.path);
				audioData.audio->setVolume(audioData.volume);
				audioData.audio->setPitch(audioData.pitch);
				audioData.audio->setLoop(audioData.looping);
				audioData.played = true;
			}
			audioData.audio->play();
		}
		return SUCCESS;
	}

	//***** H2- Getter Functions *****
	float SoundStreamManager::GetDuration(audioID_t audioID)
	{
		SoundStreamData* ad = ReturnAudioData(audioID);

		if (ad == nullptr)
			return INVALID_AUDIO_ID;

		return ad->audio->getDuration().asMilliseconds();
	}
}