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

		if (audioMap.size() < AUDIO_LIMIT)
		{
			SoundStreamData as{looping, initVolume, pitch, path};
			as.ID = GenerateID();
			as.looping = looping;
			audioMap[as.ID] = as;
			return audioMap[as.ID].ID;
		}
		else
			return AUDIO_LIMIT_EXCEEDED;

		return SUCCESS;
	}
	int SoundStreamManager::CloseStream(audioID_t audioID)
	{
		for (auto iter = audioMap.begin(); iter != audioMap.end(); iter++)
		{
			delete iter->second.audio;
			audioMap.erase(iter);
			return SUCCESS;
		}
		return INVALID_AUDIO_ID;
	}
	int SoundStreamManager::PlayStream(audioID_t audioID)
	{
		SoundStreamData* as = ReturnAudioData(audioID);

		if (as != nullptr)
		{
			as->audio = new sf::Music();
			bool openSuccess = as->audio->openFromFile(as->path);
			as->audio->setVolume(as->volume);
			as->audio->setPitch(as->pitch);
			as->audio->setLoop(as->looping);
			as->audio->play();
			as->played = true;
		}
		else
			return INVALID_AUDIO_ID;

		return SUCCESS;
	}
	int SoundStreamManager::PlayAll()
	{
		for (auto& i : audioMap)
		{
			i.second.audio = new sf::Music();
			bool openSuccess = i.second.audio->openFromFile(i.second.path);
			i.second.audio->setVolume(i.second.volume);
			i.second.audio->setPitch(i.second.pitch);
			i.second.audio->setLoop(i.second.looping);
			i.second.audio->play();
			i.second.played = true;
		}
		return SUCCESS;
	}
}