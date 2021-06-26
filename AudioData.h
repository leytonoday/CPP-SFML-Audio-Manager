#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H

//********** INCLUDES **********
#include <string>
#include <SFML/Audio.hpp>

//********** AUDIOMANAGER NAMESPACE **********
namespace AudioManager
{
	using audioID_t = std::uint32_t; //This will be used to represent all audioIDs

	//***** AudioDataBase Structure *****
	struct AudioDataBase
	{
		bool			looping;
		float			volume;
		float			pitch;
		std::string		path;
		bool			played;
		audioID_t		ID;
	};
	
	//***** SoundData Structure *****
	struct SoundData : AudioDataBase
	{
		sf::SoundBuffer 	soundBuffer;
		sf::Sound*		audio;
	};

	struct SoundStreamData : public AudioDataBase
	{
		sf::Music* audio;
	};
}

#endif
