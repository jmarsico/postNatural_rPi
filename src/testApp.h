#pragma once

#include "ofMain.h"
#include "PCA9685.h"
#include "ofxOMXPlayer.h"

 // set to -1 to not use the enable pin (its optional)

class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
    void exit();


	//void runLights(float br[]);

    ofxOMXPlayer omxPlayer;

    int numBoards;
    int counter;
    bool show;


	


	PCA9685* pca; //initialize the class with number of boards
	
};
