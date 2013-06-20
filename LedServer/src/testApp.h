#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxAnimatableFloat.h"
#include "ofxSyphon.h"
#include "ofxUI.h"

struct Client {
    string hostname;// = "127.0.0.1";
    int port = 7010;
    
    string label;// = "leaf.local";
        
    vector<ofColor> colors;
    
    ofPoint inputPos;
    int width = 1;
    int height = 120;
    
    ofxOscSender * osc;
    
    bool connected = false;
    bool enabled = false;
    
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
    int inputHeight = 160;
    
    string updateMethod = "packed";
    
    
    void addClient();
    
    ofxUIScrollableCanvas *gui;
	void guiEvent(ofxUIEventArgs &e);
    
	ofxOscSender oscBroadcast;
    ofxOscReceiver oscReceiver;
	
	ofxXmlSettings xml;
    
    vector<Client> clients;
    
    ofxSyphonClient syphonIn;
    ofFbo fboIn;
	ofTexture controlTexture;
    ofPixels controlPixels;
    
    bool monitorInput;
    bool monitorOutput;
    
	
};
