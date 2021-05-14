# C++ Audio Manager
A small libarary to enable developers to integrate audio into their C++ applications with ease. 

Implementing audio into an application in C++ can be daunting, with libraries like OpenAL, DirectSound and XAudio2. This library avoids the confusion of all that by presenting 
the user with two simple classes: SoundManager (for short audio) and SoundStreamManager (for long audio).

This can be used in graphical applications or even command line applications! There are 3 wav files you can use to test out the functionality of the library if you are interested. 

# Installation 
The Audio Manager used SFML as a backend. It's simply a convenient wrapper and helper for the SFML audio library.
First, you must install SFML in your project. Go here for more details on that: https://www.sfml-dev.org/tutorials/2.5/start-vc.php 

Once SFML is installed and linked correctly in your C++ application, simply copy the ```.cpp``` and ```.h``` files into your C++ project. Then you'll have access to the 
aforementioned classes, that reside in the ```AudioManager``` namespace.

# Usage
In the example below, I use a lot of error handling. However, this isn't entirely neccessary, I just do this to display how it would be done. If you wanted, you could just call these functions without the error handling, e.g. ```c++ int playSuccess = sm.PlaySound(audioID);```

```c++
//********** INCLUDES & USING STATEMENTS **********
#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include "AudioManager.h"

using typename AudioManager::audioID_t;

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
```

# Credits
My code and the design of the Audio Manager was greatly improved and optimized with the help of lapinozz, https://github.com/lapinozz 

SFML was used as a basis for the code: https://www.sfml-dev.org/ 
