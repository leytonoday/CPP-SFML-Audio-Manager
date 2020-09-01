/*
**************************************************
* Developer - Leyton O'Day
* Finish Date - 28/08/20
*
* HEADING SYSTEM
* H1 - Heading one, ten asterisk, all capitals			(e.g. ********** H1 - TITLE **********).
* H2 - Heading two, five asterisk, capitalised words	(e.g. ***** H2 - Title *****).
* H3 - Heading three, three asterisk, all lowercase		(e.g. *** H3 - title ***).
* H4 - Heading four, two asterisk, all lowercase		(e.g. ** H4 - title **).
* H5 - Heading five, one asterisk, all lowercase		(e.g. * H5 *).
*
* Note: Exception to the lowercase rule can be made
* when referencing a class or struct data member.
*
**************************************************
*/

//********** H1 - INCLUDES & USING STATEMENTS **********
#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include "AudioManager.h"

using typename AudioManager::audioID_t;

//********** H1 - FUNCTIONS **********
void sleep(int m)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(m));
}


//********** H1 - MAIN **********
int main()
{
	//***** H2 - SoundManager Example *****
	AudioManager::SoundManager sm;

	audioID_t audioID = sm.LoadSound("wavSound1.wav", false);
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

	std::cout << sm.GetPath(22);

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