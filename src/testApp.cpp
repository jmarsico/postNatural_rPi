#include "testApp.h"

void testApp::onEndOfStream(ofEventArgs& args)
{
	ofLogVerbose() << "onEOS";
	doProcessPixels = !doProcessPixels;
}

//--------------------------------------------------------------
void testApp::setup()
{
	

	//construct new PCA9685 object with the number of boards you're using
	numBoards = 1;
	pca = new PCA9685(numBoards);

	
    //We want to receive an event when gstreamer reaches the end of stream
	//so we need to get access to some internals to attach as a listener
	
	//Create a pointer to a new ofGstVideoPlayer
	gstVideoPlayer = new ofGstVideoPlayer();
	
	//Create a smart pointer that ofVideoPlayer requires 
	ofPtr<ofBaseVideoPlayer> baseVideoPlayer = ofPtr<ofBaseVideoPlayer>(gstVideoPlayer);
	
	//ofGstVideoUtils is what dispatches the events - get a reference
	gstVideoUtils = gstVideoPlayer->getGstVideoUtils();
	
	//We add our listener
	ofAddListener(gstVideoUtils->eosEvent, this, &testApp::onEndOfStream);
	
	//If you were creating multiple players you would want to do this at cleanup
	//We are only using one player so we are okay
	//ofRemoveListener(gstVideoUtils->eosEvent, &ofApp::onEndOfStream);
	
	videoPlayer.setPlayer(baseVideoPlayer);
	
	
	// Uncomment this to show movies with alpha channels
	// videoPlayer.setPixelFormat(OF_PIXELS_RGBA);
	
	
	videoPlayer.loadMovie("Harold.mov");
	videoPlayer.setLoopState(OF_LOOP_NORMAL); //must set this after loadMovie on the RPI
	videoWidth = videoPlayer.getWidth();
	videoHeight = videoPlayer.getHeight();
	videoPlayer.play();

	//on start-up, run through light test
	testLights();

}


//--------------------------------------------------------------
void testApp::update()
{
	
//////////////////Prod Section //////////////////
	videoPlayer.update();

	
	
}


///////////////////////// NOISE ///////////////////////////////////
void testApp::makeNoise(void)
{
    for(int i = 0; i < 30; i++)
    {
        noiseVal[i] = abs(noiseAmp * ofNoise(time * noiseSpeedX*(i+10), time*noiseSpeedY*(10-i)));
       // ofLog() << "index: " << i << " || value: " << noiseVal[i];
    }
    time += timeInc;
}



////////////////////// RUN LIGHTS //////////////////////////////////
void testApp::runLights(float br[])
{
	int lightBright[16*numBoards];
	for(int i = 0; i <30; i++)
    {
    	lightBright[i] = br[i] + noiseVal[i];
    	
    	//pca->setLED(i, br[i]);				//video only
    	//pca->setLED(i, noiseVal[i]);			//noise only
    	pca->setLED(i, lightBright[i]);			//video and noise
    	

    	//ofLog() << "channel: " << i << " value: " << noiseVal[i];
    }
}





//--------------------------------------------------------------
void testApp::draw(){
	videoPlayer.draw(0, 0);

	ofPushStyle();
		ofSetColor(ofColor::white);
		stringstream info;
		info << "App FPS: "		<< ofGetFrameRate()															<< "\n";
		info << "Frame: "		<< videoPlayer.getCurrentFrame() << "/" << videoPlayer.getTotalNumFrames()	<< "\n";
		info << "Duration: "	<< videoPlayer.getDuration()												<< "\n";
		info << "Speed: "		<< videoPlayer.getSpeed()													<< "\n";
		info << "Width: "		<< videoWidth << " / Height: " 	<< videoHeight << "\n";
		ofDrawBitmapString(info.str(), 100, 100);
	ofPopStyle();
	
}


void testApp::testLights(void)
{
	for(int val = 1; val < 4096; val+=200)
	{
	    for(int i = 0; i < 16*numBoards; i++)
	    {
		    pca->setLED(i, val); 
	    }
	}
}


