#pragma once

#include "ofMain.h"

#include "ofxImageSequence.h"
#include "ofxAnimatableFloat.h"
#include "ofxOsc.h"
#include "ofxLEDsLPD8806.h"
#include "ofxLPD8806.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp{

	public:
		void setup();
	void exit();
		void update();
		void draw();
		
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
    
	
	ofxOscReceiver receiver;
	ofxLEDsLPD8806 *led;
	ofxLPD8806 spi;

	bool isLoop;
	
	int numLED;
	int framerate;
	int lastFrameTime;
    
    ofPixels ledData;
    
    ofTexture outTexture;
    
};
