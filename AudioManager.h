#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

//********** INCLUDES **********
#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <type_traits>
#include <time.h>
#include <unordered_map>
#include <vector>

#include "AudioData.h"

#include <SFML/Audio.hpp>

//********** AUDIOMANAGER NAMESPACE **********
namespace AudioManager
{
	//***** ErrorCodes Enum *****
	static enum ErrorCodes
	{
		SUCCESS				= 0,		//This is returned when the function succeeds
		UNSUPPORTED_FILE		= -1,		//This is returned when the user attempts to load an unsupported file type
		AUDIO_LIMIT_EXCEEDED		= -2,		//This is returned when the user tries to load more than 250 files
		INVALID_AUDIO_ID		= -3,		//This is returned when a given audioID doesn't correspond to a sound, and so the target sound doesn't exist
		AUDIO_NOT_PLAYED		= -4,		//This is returned when IsPlaying, IsPaused, Resume, Pause, Stop is called, and the target sound is yet to be played. 
		AUDIO_NOT_PAUSED		= -5,		//This is used for the resume function. If the user tries to resume audio that is already playing and not paused, this is returned
		FILE_NOT_FOUND			= -6,		//This is returned when the user attemps to load a file that cannot be found
		AUDIO_STOPPED			= -7,		//This is returned when the user attempts to resume audio when it has been stopped
		AUDIO_ALREADY_PLAYING		= -8		//This is returned when the user attempts the play a sound, when it is already playing
	};

	//***** AudioStates Enum *****
	static enum AudioStates
	{
		STOPPED		= 0,		//This indicates that a sound/stream is stopped
		PAUSED		= 1,		//This indicates that a sound/stream is paused
		PLAYING		= 2		//This indicates that a sound/stream is playing
	};

	//***** AudioManager Template Class*****
	template <class T>
	class AudioManagerBase
	{
	public:
		template <typename T>
		static std::string FormatErrorCode(T errorCode)
		{
			switch (errorCode)
			{
			case SUCCESS:
				return "Error Code 0 (SUCCESS): Success";
			case UNSUPPORTED_FILE:
				return "Error Code -1 (UNSUPPORTED_FILE): Unsupported file type, supported types are: .ogg,, .wav, .flac";
			case AUDIO_LIMIT_EXCEEDED:
				return "Error Code -2 (AUDIO_LIMIT_EXCEEDED): Audio limit of 256 cannot be exceeded";
			case INVALID_AUDIO_ID:
				return "Error Code -3 (INVALID_AUDIO_ID): Invalid audioID passed";
			case AUDIO_NOT_PLAYED:
				return "Error Code -4 (AUDIO_NOT_PLAYED): Audio has not been played yet";
			case AUDIO_NOT_PAUSED:
				return "Error Code -5 (AUDIO_NOT_PAUSED): Audio is not currently paused";
			case FILE_NOT_FOUND:
				return "Error Code -6 (FILE_NOT_FOUND): File could not be found";
			case AUDIO_STOPPED:
				return "Error Code -7 (AUDIO_STOPPED): Audio has been stopped";
			case AUDIO_ALREADY_PLAYING:
				return "Error Code -8 (AUDIO_ALREADY_PLAYING): Sound/Stream is already playing";
			default:
				return "Invalid Error Code";
			}
		}		
		audioID_t GenerateID()
		{
			IDCounter++;
			return IDCounter;
		}
		bool IsTypeSupported(const std::string& path)
		{
			const int dotIndex = path.find_last_of('.');
			bool contains = std::find(supportedFileExtensions.begin(), supportedFileExtensions.end(), path.substr(dotIndex)) != supportedFileExtensions.end();
			return contains;
		}
		T* ReturnAudioData(int audioID)
		{
			auto result = audioMap.find(audioID);
			if (result == audioMap.end())
				return nullptr;
			else
				return &(result->second);
		}

		//** sound control functions **
		int Pause(audioID_t audioID)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			if (!ad->played)
				return AUDIO_NOT_PLAYED;

			ad->audio->pause();
			return SUCCESS;
		}
		int PauseAll()
		{
			for (const auto & [key, audioData] : audioMap)
			{
				if (!audioData.played)
					return AUDIO_NOT_PLAYED;
				audioData.audio->pause();
			}
			return SUCCESS;
		}
		int Resume(audioID_t audioID)
		{
			T* ad = ReturnAudioData(audioID);
			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			if (ad->played && ad->audio->getStatus() == PAUSED)
			{
				ad->audio->play();
				return SUCCESS;
			}
			else if (ad->played && ad->audio->getstatus() == PLAYING)
				return AUDIO_NOT_PAUSED;
			else if (ad->played == 0)
				return AUDIO_NOT_PLAYED;
			else if (ad->played && ad->audio->getstatus() == STOPPED)
				return AUDIO_STOPPED;
		}
		int ResumeAll()
		{
			for (const auto& [key, audioData] : audioMap)
			{
				if (audioData.played && audioData.audio->getStatus() == PAUSED)
					audioData.audio->play();
				else if (audioData.played && audioData.audio->getStatus() == PLAYING)
					return AUDIO_NOT_PAUSED;
				else if (!audioData.played)
					return AUDIO_NOT_PAUSED;
				else if (audioData.played && audioData.audio->getStatus() == STOPPED)
					return AUDIO_STOPPED;
			}
			return SUCCESS;
		}
		int Stop(audioID_t audioID)
		{
			T* ad = ReturnAudioData(audioID);
			if (ad == nullptr)
				return INVALID_AUDIO_ID;
			if (!ad->played)
				return AUDIO_NOT_PLAYED;

			ad->audio->stop();
			return SUCCESS;
		}
		int StopAll()
		{
			for (const auto& [key, audioData] : audioMap)
			{
				if (audioData.played)
					audioData.audio->stop();
				else
					return AUDIO_NOT_PLAYED;
			}
			return SUCCESS;
		}

		//** setter functions **
		int SetPitchAll(float pitch)
		{
			for (const auto& [key, audioData] : audioMap)
				audioData.audio->setPitch(std::clamp(pitch, 0, 15));
			return SUCCESS;
		}
		int SetVolumeAll(float volume)
		{
			for (const auto& [key, audioData] : audioMap)
				audioData.audio->setVolume(std::clamp(volume, 0, 100));
			return SUCCESS;
		}
		int SetPlayingPositionAll(int position)
		{
			for (const auto& [key, audioData] : audioMap)
			{
				sf::Time offset = sf::milliseconds(position);
				audioData.audio->setPlayingOffset(std::clamp(position, 0, GetDuration(key)));
			}
			return SUCCESS;
		}
		int SetPitch(audioID_t audioID, float pitch)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			ad->audio->setPitch(std::clamp(pitch, 0, 15));
			return SUCCESS;
		}
		int SetVolume(audioID_t audioID, float volume)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			ad->audio->setVolume(std::clamp(volume, 0.0f, 100.0f));
			return SUCCESS;
		}
		int SetPlayingPosition(audioID_t audioID, int position)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			sf::Time offset = sf::milliseconds(position);
			ad->audio->setPlayingOffset(std::clamp(position, 0, GetDuration(audioID)));
			return SUCCESS;
		}
		

		//** getter functions **
		float GetPitch(audioID_t audioID)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			if (!IsPlaying(audioID))
				return AUDIO_NOT_PLAYED;

			return ad->audio->getPitch();
		}
		float GetVolume(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			if (!IsPlaying(audioID))
				return AUDIO_NOT_PLAYED;

			return ad->audio->getVolume();
		}
		int IsPaused(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			if (!ad->played)
				return AUDIO_NOT_PLAYED;

			return ad->audio->getStatus() == PAUSED;
		}
		int IsPlaying(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);
			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			return ad->audio->getStatus() == PLAYING;
		}
		float GetDuration(audioID_t audioID);
		float GetPlayingPosition(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			return ad->audio->getPlayingOffset().asMilliseconds();
		}
		std::vector<int> GetAllIDs()
		{
			std::vector<int> IDs;
			for (const auto & [key, audioData]: audioMap)
				IDs.push_back(key);
			return IDs;
		}
		bool IsManagerActive()
		{
			bool active = false;
			for (const auto & [key, audioData]: audioMap)
				active = IsPlaying(key);
			return active;
		}
		int GetAudioCount()
		{
			return audioMap.size();
		}
		std::string GetPath(audioID_t audioID)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return std::to_string(INVALID_AUDIO_ID);

			return ad->path;
		}

		//** misc. functions **
		float DBToVolume(float dB)
		{
			return powf(10.0f, 0.05f * dB);
		}
		float VolumeTodB(float volume)
		{
			return 20.0f * log10f(volume);
		}

		//*** containers ***
		std::unordered_map<audioID_t, T> audioMap;
		std::vector<audioID_t> IDVec;
		std::vector<std::string> supportedFileExtensions{ ".ogg", ".wav", ".flac", ".aiff", ".au", ".raw", ".paf", ".svx", ".nist",
														".voc", ".ircam", ".w64", ".mat4", ".mat5", ".pvf", ".htk", ".sds", ".avr",
														".sd2", ".caf", ".wve", ".mpc2k", ".rf64" };

		unsigned int IDCounter = 0; //This will be incremented every GenerateID() call, and it's value will be used as a unique soundID
		const unsigned int AUDIO_LIMIT = 255;
	};

	//***** SoundManager Class *****
	class SoundManager : public AudioManagerBase<SoundData>
	{
	public:
		int LoadSound(const std::string& path, bool looping = false, float initVolume = 100, float pitch = 1);
		int UnloadSound(audioID_t audioID);
		int MixSounds(bool looping, float initVolume, float pitch, int sampleRate, int numArgs, ...);
		int PlaySound(audioID_t audioID);
		int PlayAll();
		float GetDuration(audioID_t audioID);
	};

	//***** SoundStreamManager Class *****
	class SoundStreamManager : public AudioManagerBase<SoundStreamData>
	{
	public:
		int OpenStream(const std::string& path, bool looping = false, float initVolume = 100, float pitch = 1);
		int CloseStream(audioID_t audioID);
		int PlayStream(audioID_t audioID);
		int PlayAll();
		float GetDuration(audioID_t audioID);
	};
}

#endif
