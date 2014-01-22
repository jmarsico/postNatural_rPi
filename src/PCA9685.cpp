#include "PCA9685.h"

PCA9685::PCA9685(int _numBoards)
{
	numBoards = _numBoards;					//number of PCA9685 boards to be used
	for(int i = 0; i < numBoards; i++)		//initialize an array of new board objects
	{
		pwm[i] = new PWM(64+i, true);
		pwm[i] ->setPWMFreq(6000);
	}
	ofLog() << "PCA Initialized with" << numBoards << "boards";
}

void PCA9685::setLED(int channel, int val)
{
	
	
	if(channel <=numBoards*16-1)		//check to make sure channel exists
	{
		int unit = 0;					//variable for index in pwm[] array
		int unitChan = channel;			//channel relative to it's board (0-16)
		
		unit = floor(channel/16);			//unit is channel/16, rounded down
		unitChan = channel - unit*16;		//re-interpret channel for this pwm[i]
		
		pwm[unit]->setPWM(unitChan, 0, val);	//write to the unit/channel
	}
}



	