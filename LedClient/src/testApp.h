#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxLEDsLPD8806.h"
#include "ofxLPD8806.h"
#include "ofxXmlSettings.h"

enum {STATE_CONNECTED, STATE_WAITING};

class testApp : public ofBaseApp, public ofThread{

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
    
    
    void saveSettings();
    void threadedFunction();
	
	ofxOscReceiver receiver;
    ofxOscSender sender;
    
	ofxLEDsLPD8806 *led;
	ofxLPD8806 spi;
	
	int numLED;
	int refreshRate;
    
	long lastLedCmdTime;
    long lastCmdTime;
    
    int autoModeDelay;
    bool autoMode;
    
    int state;
    
    string hostname;
    int number;
    
    vector<ofColor> ledData; //ofPixels ledData;
    
    ofTexture outTexture;
    
    float position;
    
    int clientId;
    
    string masterHostname;
    int masterPort;
    
    ofxXmlSettings settings;
    
    string label;
    
    // todo:
    int height;
    int width;
    
    // handshake
    int long helloTime;
    int long helloResponseWait;
    
    
};
