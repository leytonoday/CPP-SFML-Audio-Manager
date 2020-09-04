#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H

//********** H1 - INCLUDES **********
#include <string>
#include <SFML/Audio.hpp>

//********** H1 - AUDIOMANAGER NAMESPACE **********
namespace AudioManager
{
	using audioID_t = std::uint32_t; //This will be used to represent all audioIDs

	//***** H2 - AudioDataBase Structure *****
	struct AudioDataBase
	{
		//*** H3 - data members ***
		//** H4 - variables **
		bool			looping;
		float			volume;
		float			pitch;
		std::string		path;
		bool			played;
		audioID_t		ID;
	};
	
	//***** H2 - SoundData Structure *****
	struct SoundData : AudioDataBase
	{
		//*** H3 - data members ***
		//** H4 - variables **
		sf::SoundBuffer soundBuffer;
		sf::Sound*		audio;
	};

	//***** H2 - SoundStreamData *****
	struct SoundStreamData : public AudioDataBase
	{
		//*** H3 - data members ***
		//** H4 - variables **
		sf::Music* audio;
	};
}

#endif