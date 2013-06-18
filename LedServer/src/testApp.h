#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxAnimatableFloat.h"
#include "ofxSyphon.h"
#include "ofxUI.h"

struct Client {
    string hostname = "leaf.local";
    int port = 2838;
    
    int label; // For remembering who is who
    ofPoint inputPos; // For the mapping
        
    ofTexture texture;
    
    ofPixels pixels;
    vector<ofColor> colors;
    
    ofFbo fboOut;
    
    int width = 1;
    int height = 120;
    
    ofxOscSender * osc;
    
    bool connected = false;
    bool enabled = true;
    
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
    
    
    void addClient();
    
    ofxUIScrollableCanvas *gui;
	void guiEvent(ofxUIEventArgs &e);
    
	ofxOscSender oscBroadcast;
	
	ofxXmlSettings xml;
	int numDevice;
	void  initClient();
	bool bDebug;
    
    vector<Client> clients;
    
	ofTexture controlTexture;
	ofxSyphonClient syphonIn;
    ofFbo fboIn;
	
};
