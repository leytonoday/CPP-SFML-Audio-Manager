#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

//********** H1 - INCLUDES **********
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
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

//********** H1 - AUDIOMANAGER NAMESPACE **********
namespace AudioManager
{
	//***** H2 - ErrorCodes Enum *****
	static enum ErrorCodes
	{
		SUCCESS					= 0,		//This is returned when the function succeeds
		UNSUPPORTED_FILE		= -1,		//This is returned when the user attempts to return an unsupported file type
		AUDIO_LIMIT_EXCEEDED	= -2,		//This is returned when the user tries to load more than 256
		INVALID_AUDIO_ID		= -3,		//This is returned when a given soundID doesn't correspond to a sound, and so the target sound doesn't exist
		AUDIO_NOT_PLAYED		= -4,		//This is returned when IsPlaying, IsPaused, Resume, Pause, Stop is called, and the target sound is yet to be played. 
		AUDIO_NOT_PAUSED		= -5,		//This is used for the resume function. If the user tries to resume audio that is already playing and not paused, this is returned
		FILE_NOT_FOUND			= -6,		//This is returned when the user attemps to load a file that cannot be found
		VOLUME_OUT_OF_RANGE		= -7,		//This is returned when the user attempts to change the volume to an invalid value
		PITCH_OUT_OF_RANGE		= -8,		//This is returned when the user attempts to change th pitch to an invalid value
		AUDIO_STOPPED			= -9,		//This is returned when the user attempts to resume audio when it has been stopped
		POSITION_OUT_OF_RANGE	= -10,		//This is returned when the user attempts to set the position of audio to an invalid position
	};

	//***** H2 - AudioStates Enum *****
	static enum AudioStates
	{
		STOPPED		= 0,		//This indicates that a sound/stream is stopped
		PAUSED		= 1,		//This indicates that a sound/stream is paused
		PLAYING		= 2			//This indicates that a sound/stream is playing
	};

	//***** H2 - AudioManager Template Class*****
	template <class T>
	class AudioManagerBase
	{
	public:
		//*** H3 - member functions ***
		//** H4 - utility functions **
		std::string FormatErrorCode(int errorCode)
		{
			switch (errorCode)
			{
			case SUCCESS:
				return "Error Code 0 (SUCCESS): Success";
				break;
			case UNSUPPORTED_FILE:
				return "Error Code -1 (UNSUPPORTED_FILE): Unsupported file type, supported types are: .ogg,, .wav, .flac";
				break;
			case AUDIO_LIMIT_EXCEEDED:
				return "Error Code -2 (AUDIO_LIMIT_EXCEEDED): Audio limit of 256 cannot be exceeded";
				break;
			case INVALID_AUDIO_ID:
				return "Error Code -3 (INVALID_AUDIO_ID): Invalid audioID passed";
				break;
			case AUDIO_NOT_PLAYED:
				return "Error Code -4 (AUDIO_NOT_PLAYED): Audio has not been played yet";
				break;
			case AUDIO_NOT_PAUSED:
				return "Error Code -5 (AUDIO_NOT_PAUSED): Audio is not currently paused";
				break;
			case FILE_NOT_FOUND:
				return "Error Code -6 (FILE_NOT_FOUND): File could not be found";
				break;
			case VOLUME_OUT_OF_RANGE:
				return "Error Code -7 (VOLUME_OUT_OF_RANGE): Volume is out of range (keep between 0-100)";
				break;
			case PITCH_OUT_OF_RANGE:
				return "Error Code -8 (PITCH_OUT_OF_RANGE): Pitch is out of range (cannot be below 0)";
				break;
			case AUDIO_STOPPED:
				return "Error Code -9 (AUDIO_STOPPED): Audio has been stopped";
				break;
			case POSITION_OUT_OF_RANGE:
				return "Error Code -10 (POSITION_OUT_OF_RANGE): Position is out of range (cannot exceed the audio duration)";
				break;
			default:
				return "Invalid Error Code";
				break;
			}
		}		
		audioID_t GenerateID()
		{
			IDCounter++;
			return IDCounter;
		}
		bool IsTypeSupported(std::string path)
		{
			const int dotIndex = path.find_last_of('.');
			const std::string fileExtension = path.substr(dotIndex);
			bool contains = std::find(supportedFileExtensions.begin(), supportedFileExtensions.end(), fileExtension) != supportedFileExtensions.end();
			return contains;
		}
		T* ReturnAudioData(int audioID)
		{
			for (auto& i : audioMap)
			{
				if (audioID == i.first)
					return &i.second;
			}
			return nullptr;
		}

		//** H4 - sound control functions **
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
			for (auto& i : audioMap)
			{
				T& audioData = i.second;
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
			for (auto& i : audioMap)
			{
				T& audioData = i.second;
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
			for (auto& i : audioMap)
			{
				T& audioData = i.second;
				if (audioData.played)
					audioData.audio->stop();
				else
					return AUDIO_NOT_PLAYED;
			}
			return SUCCESS;
		}

		//** H4 - setter functions **
		int SetPitchAll(float pitch)
		{
			if (pitch < 0)
				return PITCH_OUT_OF_RANGE;
			for (auto& i : audioMap)
				i.second.audio->setPitch(pitch);
			return SUCCESS;
		}
		int SetVolumeAll(float volume)
		{
			if (!(volume >= 0 && volume <= 00))
				return VOLUME_OUT_OF_RANGE;
			for (auto& i : audioMap)
				i.second.audio->setVolume(volume);
			return SUCCESS;
		}
		int SetPlayingPositionAll(int position)
		{
			for (auto& i : audioMap)
			{
				if (position < 0 || position > GetDuration(i.first))
					return POSITION_OUT_OF_RANGE;
				sf::Time offset = sf::milliseconds(position);
				i.second.audio->setPlayingOffset(offset);
			}
			return SUCCESS;
		}
		int SetPitch(audioID_t audioID, float pitch)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad != nullptr && !(pitch < 0))
			{
				ad->audio->setPitch(pitch);
				return SUCCESS;
			}
			else if (ad == nullptr)
				return INVALID_AUDIO_ID;
			else if (pitch < 0)
				return PITCH_OUT_OF_RANGE;
		}
		int SetVolume(audioID_t audioID, float volume)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad != nullptr && volume >= 0 && volume <= 100)
			{
				ad->audio->setVolume(volume);
				return SUCCESS;
			}
			else if (ad == nullptr)
				return INVALID_AUDIO_ID;
			else if (!(volume > 0 && volume < 100))
				return VOLUME_OUT_OF_RANGE;
		}
		int SetPlayingPosition(audioID_t audioID, int position)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad != nullptr)
			{
				if (position < 0 || position > GetDuration(audioID))
					return POSITION_OUT_OF_RANGE;
				else
				{
					sf::Time offset = sf::milliseconds(position);
					ad->audio->setPlayingOffset(offset);
					return SUCCESS;
				}
			}
			else
				return INVALID_AUDIO_ID;
		}
		

		//** H4 - getter functions **
		float GetPitch(audioID_t audioID)
		{
			T* ad = ReturnAudioData(audioID);

			if (ad != nullptr && IsPlaying(audioID))
				return ad->audio->getPitch();
			else if (ad == nullptr)
				return INVALID_AUDIO_ID;
			else if (!IsPlaying(audioID))
				return AUDIO_NOT_PLAYED;
		}
		float GetVolume(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);

			if (ad != nullptr && IsPlaying(audioID))
				return ad->audio->getVolume();
			else if (ad == nullptr)
				return INVALID_AUDIO_ID;
			else if (!IsPlaying(audioID))
				return AUDIO_NOT_PLAYED;
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

			if (!ad->played)
				return AUDIO_NOT_PLAYED;

			return ad->audio->getStatus() == PLAYING;
		}
		float GetDuration(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);

			if (ad == nullptr)
				return INVALID_AUDIO_ID;

			return ad->audio->getDuration().asMilliseconds();
		}
		float GetPlayingPosition(audioID_t audioID) 
		{
			T* ad = ReturnAudioData(audioID);

			if (ad != nullptr)
			{
				return ad->audio->getPlayingOffset().asMilliseconds();
			}
			else
				return INVALID_AUDIO_ID;
		}
		std::vector<int> GetAllIDs()
		{
			std::vector<int> IDs;
			for (auto& i : audioMap)
				IDs.push_back(i.second.ID);
			return IDs;
		}
		bool IsManagerActive()
		{
			bool active = false;
			for (auto& i : audioMap)
				active = IsPlaying(i.first);
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

		//** H4 - misc. functions **
		float DBToVolume(float dB)
		{
			return powf(10.0f, 0.05f * dB);
		}
		float VolumeTodB(float volume)
		{
			return 20.0f * log10f(volume);
		}

		//*** H3 - containers ***
		std::unordered_map<audioID_t, T> audioMap;
		std::vector<audioID_t> IDVec;
		std::vector<std::string> supportedFileExtensions{ ".ogg", ".wav", ".flac", ".aiff", ".au", ".raw", ".paf", ".svx", ".nist",
														".voc", ".ircam", ".w64", ".mat4", ".mat5", ".pvf", ".htk", ".sds", ".avr",
														".sd2", ".caf", ".wve", ".mpc2k", ".rf64" };

		//*** H3 - data members ***
		//** H4 - variables **
		unsigned int IDCounter = 0; //This will be incremented every GenerateID() call, and it's value will be used as a unique soundID

		//** H4 - constants **
		const unsigned int AUDIO_LIMIT = 255;
	};

	//***** H2 - SoundManager Class *****
	class SoundManager : public AudioManagerBase<SoundData>
	{
	public:
		//*** H3 - loaders and play functions ***
		int LoadSound(const std::string& path, bool looping, float initVolume = 100, float pitch = 1);
		int UnloadSound(audioID_t audioID);
		int MixSounds(bool looping, float initVolume, float pitch, int sampleRate, int numArgs, ...);
		int PlaySound(audioID_t audioID);
		int PlayAll();
	};

	//***** H2 - SoundStreamManager Class *****
	class SoundStreamManager : public AudioManagerBase<SoundStreamData>
	{
	public:
		//*** H3 - loaders and play functions ***
		int OpenStream(const std::string& path, bool looping, float initVolume = 100, float pitch = 1);
		int CloseStream(audioID_t audioID);
		int PlayStream(audioID_t audioID);
		int PlayAll();
	};
}

#endif