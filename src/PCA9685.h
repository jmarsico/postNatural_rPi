#ifndef PCA9685H
#define PCA9685H

#pragma once
#include "ofMain.h"
#include "pwmlib.h"

class PCA9685
{
public:
	int numBoards;			//number of PCA9685 boards

	
	PCA9685(int _numBoards);			//constructor
	void setLED(int channel, int _val);  	//set each LED by it's channel (irregardless of board number)

	PWM* pwm[50];			//hold memory for up to 50 pca9685 units


	int val;


};

#endif