#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup()
{
	

	//construct new PCA9685 object with the number of boards you're using
	numBoards = 1;
	pca = new PCA9685(numBoards);
	//on start-up, run through light test
	counter = 0;
	show = true;


	string videoPath = ofToDataPath("noise_box_video.mp4", true);

	//this will let us just grab a video without recompiling
	/*
	ofDirectory currentVideoDirectory("/home/pi/videos/current");
	if (currentVideoDirectory.exists()) 
	{
		currentVideoDirectory.listDir();
		vector<ofFile> files = currentVideoDirectory.getFiles();
		if (files.size()>0) 
		{
			videoPath = files[0].path();
		}		
	}
	*/
	
	//Somewhat like ofFboSettings we may have a lot of options so this is the current model
	/*
	ofxOMXPlayerSettings settings;
	settings.videoPath = videoPath;
	settings.useHDMIForAudio = true;	//default true
	settings.enableTexture = false;		//default true
	settings.enableLooping = true;		//default true
	settings.enableAudio = true;		//default true, save resources by disabling
	
	
	if (settings.enableTexture)
	{
		doShader = false;
		if (doShader) 
		{
			ofEnableAlphaBlending();
			
			shader.load("PostProcessing.vert", "PostProcessing.frag", "");
			
			fbo.allocate(ofGetWidth(), ofGetHeight());
			fbo.begin();
			ofClear(0, 0, 0, 0);
			fbo.end();
			
		}
	}
	*/
	
	//so either pass in the settings
	//omxPlayer.setup(settings);
	
	//or live with the defaults
	omxPlayer.loadMovie(videoPath);

}

void testApp::exit()
{
	omxPlayer.close();
}

//--------------------------------------------------------------
void testApp::update()
{
	counter ++;
	ofLog() << "counter: " << counter ;
    if(counter == 1000)
    {
      show = !show;
      counter = 0;
    }

    if(show)
    {
    	pca->setLED(0, 4095);
    }
    else
    {
    	pca->setLED(0,4095);
    }
		

/*	
if(omxPlayer.isPlaying() || omxPlayer.isTextureEnabled)
	{
		if (doShader) 
		{
			updateFbo();
		}
	}
	*/
}


//--------------------------------------------------------------
void testApp::draw(){
	omxPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());

	/*
	for(int i = 0; i<16; i++)
	{
		if(show)
		{
			pca->setLED(i, 4095);
		}
		else
		{
			pca->setLED(i, 0);
		}
	}
	*/
		
		//draw a smaller version in the lower right
		//int scaledHeight = omxPlayer.getHeight()/4;
		//int scaledWidth = omxPlayer.getWidth()/4;
		//omxPlayer.draw(ofGetWidth()-scaledWidth, ofGetHeight()-scaledHeight, scaledWidth, scaledHeight);
	
}





