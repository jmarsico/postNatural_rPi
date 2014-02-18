/*
video is made possible through Jason Van Cleave's ofxOMXPlayer https://github.com/jvcleave/ofxOMXPlayer
to enable audio from the rPi 3.5mm audio jack try this from the command line:
$sudo modprobe snd_bcm2835
$sudo amixer cset numid=3 1

To use HDMI Audio you may need to add the below line to /boot/config.txt and reboot. You'll also need
to change settings.useHDMIForAudio to true. 
See http://elinux.org/RPiconfig for more details
	
hdmi_drive=2


PINS: 
phone 0: pin 2
phone 1: pin 3
phone 2: pin 0

pins are based on the WiringPi pin layout on this page: 
https://projects.drogon.net/raspberry-pi/wiringpi/pins/
*/


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
void testApp::reset(void)
{
	//re-load movie, bounce the pause to get to the first or second frame
	ofLogVerbose() << "re-load and pause";
	omxPlayer.loadMovie(ofToDataPath(videoPath, true));
	omxPlayer.setPaused(true);
	omxPlayer.setPaused(false);
	//make sure all LED channels are set to zero
	for(int i = 0; i < 16 * numBoards; i ++)
	{
		pca->setLED(i, 0);
	}
	sleep(1);
	omxPlayer.setPaused(true);
	mainCounter = 0;

	

}
//--------------------------------------------------------------
void testApp::setup()
{
	numBoards = 1;
	pca = new PCA9685(numBoards);


	/////////////// SET UP RAMPS! /////////////////////
	ramps[0].channel = 0;
	ramps[0].upFrame = 100;
	ramps[0].downFrame = 400;

	ramps[1].channel = 1;
	ramps[1].upFrame = 200;
	ramps[1].downFrame = 500;

	ramps[2].channel = 2;
	ramps[2].upFrame = 500;
	ramps[2].downFrame = 1000;

	ramps[3].channel = 0;
	ramps[3].upFrame = 600;
	ramps[3].downFrame = 900;

	ramps[4].channel = 1;
	ramps[4].upFrame = 700;
	ramps[4].downFrame = 900;



	//make sure all LED channels are set to zero
	for(int i = 0; i < 16 * numBoards; i ++)
	{
		pca->setLED(i, 0);
	}

	
	//set up phone pins
	phone[0] = 2;
	phone[1] = 3;
	phone[2] = 0;
	wiringPiSetup();
	for(int i = 0; i < 3; i++){
		pinMode(phone[i], INPUT);
		pullUpDnControl (phone[i], PUD_UP);
	}

	consoleListener.setup(this);
	ofHideCursor();
	videoCounter = 0;
	
	//we can hardcode a different location for the video
	videoPath = ofToDataPath("Intro_HD720.mp4", true);

	
	//OR
/*
	//if there is a file in /bin/data, it will choose the first file it sees
	//this will let us just grab a video without recompiling
	ofDirectory currentVideoDirectory(ofToDataPath("/bin/data", true));
	if (currentVideoDirectory.exists()) 
	{
		//option to put multiple videos in folder to test
		currentVideoDirectory.listDir();
		files = currentVideoDirectory.getFiles();
		if (files.size()>0) 
		{
			videoPath = files[0].path();
		}		
	}
	*/

	ofLogVerbose() << "using videoPath : " << videoPath;
	settings.videoPath = videoPath;
	settings.useHDMIForAudio = false;		//default true
	
	settings.enableTexture = true;		//default true, uncomment for direct-to-screen mode
	settings.enableLooping = true;
	if (files.size()>0)
	{
		//we don't want to loop if we are switching files
		settings.enableLooping = false;		//default true
	}
	settings.enableAudio = true;
	settings.listener = this; 				//this app extends ofxOMXPlayerListener so it will receive events
	omxPlayer.setup(settings);

	
	//omxPlayer.setPaused(true);w
	doDrawInfo = false;

	totalFrames = omxPlayer.getTotalNumFrames();

	mainCounter = 0;
	
}

//--------------------------------------------------------------
void testApp::update(){
	currentFrame = omxPlayer.getCurrentFrame()%totalFrames;
	//pullUpDnControl (0, PUD_UP);
	for(int i = 0; i < 3; i++)
	{
		phoneState[i] = digitalRead(phone[i]);
	
	}
	
	

	//----------------state machine for phones / pausing----------------
	
	
	//phoneState 1 = off hook, 0 = on hook
	
	//if any of the phones are off the hook, set isOffHook to true
	if(phoneState[0]==0 || phoneState[1]==0 || phoneState[2]==0 )
	{
		isOffHook = true;
	}

	//if all phones are on the hook, isOffHook is false
	if(phoneState[0] == 1 && phoneState[1] == 1 && phoneState[2] == 1)	
	{
		isOffHook = false;
		//make sure all LED channels are set to zero
		for(int i = 0; i < 16 * numBoards; i ++)
		{
			pca->setLED(i, 0);
		}
	}

	//if the video is paused, phone is on hook and then removed, play video
	if(omxPlayer.isPaused() && !prevIsOffHook && isOffHook)
	{
		omxPlayer.setPaused(false);
	}

	//if video is playing and phones were off hook but are hung up, reset video
	if(!omxPlayer.isPaused() && prevIsOffHook && !isOffHook)
	{
		reset();
		mainCounter = 0;
	}

	//set previous hook state to current hook state
	prevIsOffHook = isOffHook;

	//if the player is not paused, increment the counter
	if(omxPlayer.isPaused() == false)
	{
		mainCounter++;
	}

	//--------------------------------------------------------------
	//----------------RAMPING CUES----------------------------------
	//--------------------------------------------------------------

	for(int i = 0; i < numRamps; i++)
	{
		//if the currentFrame is within 5 frames of the upFrame
		if(mainCounter >=ramps[i].upFrame && mainCounter <= ramps[i].upFrame + (int)4095/fadeInc)
		{
			
			int rampingVal =mainCounter - ramps[i].upFrame;
			int val = rampingVal*fadeInc;

			if(val > 4095) val = 4095;
			pca->setLED(ramps[i].channel, val);
			ofLog() << "channel: " << ramps[i].channel << " value: " << val;
		}


		if(mainCounter >=ramps[i].downFrame && mainCounter <= ramps[i].downFrame + (int)4095/fadeInc)
		{
			int rampingVal = mainCounter - ramps[i].downFrame;
			int val = 4000 - rampingVal*fadeInc;	
			if(val <= 0) val = 0;
			pca->setLED(ramps[i].channel, val);
			ofLog() << "channel: " << ramps[i].channel << " value: " << val;
		}
	}
	ofLog() << "global Counter: " << mainCounter;
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
		info <<"\n" <<  "PIN2 VALUE: "			<< phoneState[0];
		info <<"\n" <<  "PIN1 VALUE: "			<< phoneState[0];
		info <<"\n" <<  "PIN1 VALUE: "			<< phoneState[0];
		info <<"\n" <<	"counter: "				<< mainCounter;
		if(isOffHook)
		{
			info <<"\n" <<  "PHONE OFF HOOK"			;
		}
		if(!isOffHook)
		{
			info <<"\n" <<  "ALL PHONES ON HOOK"		;

		}

		info <<"\n" <<	"KEYS:";
		info <<"\n" <<	"t to Toggle Info Display";
		info <<"\n" <<	"p to Toggle Pause";
		ofDrawBitmapStringHighlight(info.str(), 60, 60, ofColor(0, 0, 0, 90), ofColor::yellow);
	}

	

	
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

		case 'r':
		{
			reset();
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

