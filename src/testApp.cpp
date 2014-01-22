#include "testApp.h"


void testApp::onVideoEnd(ofxOMXPlayerListenerEventData& e)
{
	ofLogVerbose(__func__) << " RECEIVED";
	


	if (files.size() ==0) 
	{
		//We have only the one file so it will just loop
		return;
	}
	if(videoCounter+1<files.size())
	{
		videoCounter++;
	}else
	{
		videoCounter = 0;
	}
	omxPlayer.loadMovie(files[videoCounter].path());


}

//--------------------------------------------------------------
void testApp::onVideoLoop(ofxOMXPlayerListenerEventData& e)
{
	ofLogVerbose(__func__) << " RECEIVED";
	omxPlayer.setPaused(true);

}


//--------------------------------------------------------------
void testApp::setup()
{
	numBoards = 1;
	pca = new PCA9685(numBoards);

	consoleListener.setup(this);
	ofHideCursor();
	videoCounter = 0;
	
	string videoPath = ofToDataPath("noise_box_video.mp4", true);
	
	/*
	//this will let us just grab a video without recompiling
	ofDirectory currentVideoDirectory(ofToDataPath("../../../videos/current", true));
	bool doRandomSelect		= true;
	if (currentVideoDirectory.exists()) 
	{
		//option to put multiple videos in folder to test
		currentVideoDirectory.listDir();
		files = currentVideoDirectory.getFiles();
		if (files.size()>0) 
		{
			if (doRandomSelect && files.size()>1) {
				videoPath = files[ofRandom(files.size())].path();
			}else 
			{
				videoPath = files[0].path();
			}
		}		
	}
	*/
	
	ofLogVerbose() << "using videoPath : " << videoPath;
	settings.videoPath = videoPath;
	settings.useHDMIForAudio = false;		//default true
	
	//settings.enableTexture = false;		//default true, uncomment for direct-to-screen mode
	settings.enableLooping = true;
	if (files.size()>0)
	{
		//we don't want to loop if we are switching files
		settings.enableLooping = false;		//default true
	}
	settings.enableAudio = true;
	settings.listener = this; //this app extends ofxOMXPlayerListener so it will receive events ;
	omxPlayer.setup(settings);
	
	
	doDrawInfo = true;

	totalFrames = omxPlayer.getTotalNumFrames();
	
}

//--------------------------------------------------------------
void testApp::update(){
	currentFrame = omxPlayer.getCurrentFrame()%totalFrames;
	
}

//--------------------------------------------------------------
void testApp::draw()
{
	
	

	
	if (!settings.enableTexture) return; //direct to screen - nothing else draws so returning
	omxPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());

	
//---------------------------draw debugging info-----------------------------------
	if (doDrawInfo) 
	{
		stringstream info;
		info <<"\n" <<  "APP FPS: "+ ofToString(ofGetFrameRate());
		
		
		info <<"\n" <<	"MEDIA TIME: "			<< omxPlayer.getMediaTime();
		info <<"\n" <<	"OF DIMENSIONS: "		<< ofGetWidth()<<"x"<<ofGetHeight();
		info <<"\n" <<	"DIMENSIONS: "			<< omxPlayer.getWidth()<<"x"<<omxPlayer.getHeight();
		info <<"\n" <<	"DURATION: "			<< omxPlayer.getDuration();
		info <<"\n" <<	"TOTAL FRAMES: "		<< totalFrames;
		info <<"\n" <<	"CURRENT FRAME: "		<< currentFrame;
		info <<"\n" <<	"REMAINING FRAMES: "	<< totalFrames - currentFrame;
		
		info <<"\n" <<	"CURRENT VOLUME: "		<< omxPlayer.getVolume();
		
		info <<"\n" <<	"KEYS:";
		info <<"\n" <<	"t to Toggle Info Display";
		info <<"\n" <<	"p to Toggle Pause";
		info <<"\n" <<	"b to Step frame forward";
		info <<"\n" <<	"1 to Decrease Volume";
		info <<"\n" <<	"2 to Increase Volume";
		ofDrawBitmapStringHighlight(info.str(), 60, 60, ofColor(0, 0, 0, 90), ofColor::yellow);
	}

	

//------------------LED 0---------------------------------------
	if(currentFrame > 100 && currentFrame < 400)
	{
		pca->setLED(0, (currentFrame-100) * 4);
	}

	if(currentFrame > (totalFrames - 100) && currentFrame < totalFrames)
	{
		pca->setLED(0, 0);
	}

//------------------LED 1---------------------------------------	
	if(currentFrame > 550 && currentFrame < 950)
	{
		pca->setLED(1, (currentFrame-550) * 4);
	}

	if(currentFrame > (totalFrames - 100) && currentFrame < totalFrames)
	{
		pca->setLED(1, 0);
	}
//------------------LED 1---------------------------------------	
	{
	if(currentFrame > 600 && currentFrame < 1000)
	{
		pca->setLED(2, (currentFrame-600) * 4);
	}

	if(currentFrame > (totalFrames - 100) && currentFrame < totalFrames)
	{
		pca->setLED(2, 0);
	}}
}



//--------------------------------------------------------------
void testApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	//ConsoleListener* thread = (ConsoleListener*) e.listener;
	keyPressed((int)e.character);
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofLogVerbose() << "key received!";
	switch (key) 
	{
		case 'p':
		{
			ofLogVerbose() << "pause: " << !omxPlayer.isPaused();
			omxPlayer.setPaused(!omxPlayer.isPaused());
			break;
			
		}
		
		case '1':
		{
			
			ofLogVerbose() << "decreaseVolume";
			omxPlayer.decreaseVolume();
			break;
		}
		case '2':
		{
			ofLogVerbose() << "increaseVolume";
			omxPlayer.increaseVolume();
			break;
		}
			
		case 'b':
		{
			ofLogVerbose() << "stepFrameForward";
			omxPlayer.stepFrameForward();
			break;
		}
		

		case 't':
		{
			doDrawInfo = !doDrawInfo;
			break;
		}
		
		default:
		{
			break;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

