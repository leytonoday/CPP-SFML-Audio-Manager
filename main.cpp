//********** INCLUDES & USING STATEMENTS **********
#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include "AudioManager.h"

using typename AudioManager::audioID_t;

//********** FUNCTIONS **********
void sleep(int m)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(m));
}


//********** MAIN **********
int main()
{
	//***** SoundManager Example *****
	AudioManager::SoundManager sm;

	audioID_t audioID = sm.LoadSound("wavSound1.wav");
	if (audioID < 0)
	{
		std::cout << sm.FormatErrorCode(audioID) << std::endl;
		return -1;
	}

	int playSuccess = sm.PlaySound(audioID);

	if (playSuccess < 0)
	{
		std::cout << sm.FormatErrorCode(playSuccess) << std::endl;
		return -1;
	}
	
	while (sm.IsPlaying(audioID))
	{
		//The sound is playing here
	}

	int unloadSuccess = sm.UnloadSound(audioID);
	if (unloadSuccess < 0)
	{
		std::cout << sm.FormatErrorCode(playSuccess) << std::endl;
		return -1;
	}

	return 0;
}