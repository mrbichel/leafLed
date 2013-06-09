#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxAnimatableFloat.h"
#include "ofxSyphon.h"


struct Client {
    string hostname;
    int port;
    
    int label; // For remembering who is who
    int position; // For the mapping
        
    ofTexture dataTexture;
    
    ofFloatPixels data;
    
    int width;
    int height;
    
    ofxOscSender osc;
    
    void setup();
    void update();
    void exit();
};


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
	ofxOscSender *oscBroadcast;
	
	ofxXmlSettings xml;
	int numDevice;
	void  initClient();
	bool bDebug;
    
    vector<Client> clients;
    
	ofTexture controlTexture;
	ofxSyphonClient syphonIn;
	
};
