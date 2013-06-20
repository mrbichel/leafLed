#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxAnimatableFloat.h"
#include "ofxSyphon.h"
#include "ofxUI.h"

struct Client {
    string hostname = "127.0.0.1";
    int port = 2838;
    
    string label = "leaf.local"; // For remembering who is who
    ofPoint inputPos; // For the mapping
        
    ofTexture texture;
    
    ofPixels pixels;
    vector<ofColor> colors;
    
    ofFbo fboOut;
    
    int width = 1;
    int height = 121;
    
    ofxOscSender * osc;
    
    bool connected = false;
    bool enabled = true;
    
    void setup();
    void update(string method);
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
    
    
    int numClients = 5;
    
    int inputWidth = 202;
    int inputHeight = 260;
    
    string updateMethod = "packed";
    
    
    void addClient();
    
    ofxUIScrollableCanvas *gui;
	void guiEvent(ofxUIEventArgs &e);
    
	ofxOscSender oscBroadcast;
    ofxOscReceiver oscReceiver;
	
	ofxXmlSettings xml;
	int numDevice;
	void  initClient();
	bool bDebug;
    
    vector<Client> clients;
    
	ofTexture controlTexture;
	ofxSyphonClient syphonIn;
    ofFbo fboIn;
	
};
