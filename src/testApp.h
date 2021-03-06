#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ConsoleListener.h"
#include "PCA9685.h"
#include "ofxWiringPi.h"



class testApp : public ofBaseApp, public SSHKeyListener, public ofxOMXPlayerListener{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void reset();
	
	ofxOMXPlayerSettings settings;
	ofxOMXPlayer omxPlayer;
	
	//allows key commands via Shell
	void onCharacterReceived(SSHKeyListenerEventData& e);
	ConsoleListener consoleListener;
	
	//ofxOMXPlayerListener inheritance
	void onVideoEnd(ofxOMXPlayerListenerEventData& e);
	void onVideoLoop(ofxOMXPlayerListenerEventData& e);
	
	vector<ofFile> files;
	int videoCounter;
	
	bool doDrawInfo;

	int numBoards;
	PCA9685* pca;


	int currentFrame;
	int totalFrames;
	int mainCounter;

	
	bool isOffHook;
	bool prevIsOffHook;
	int phone[3];
	int phoneState[3];
	string videoPath;

	struct Ramp{
		int channel;
		int upFrame;
		int downFrame;
	};

	//CHANGE THESE VALUES FOR YOUR INSTALL
	static const int numRamps = 5;
	static const int fadeInc = 40;
	Ramp ramps[numRamps];



	
};
