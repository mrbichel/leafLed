#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxAnimatableFloat.h"
#include "ofxSyphon.h"
#include "ofxUI.h"

struct Client {
    
    Client(){
        osc = new ofxOscSender;
        colors.assign(height, ofColor(255));
    };
    
    ~Client() {
        delete gui;
    }

    void updateHeight(int _height);
    
    void setup();
    void setGui();
    void update(string method);
    
    void newId();
    void setId();
    
    void updateGui();
    
    void guiEvent(ofxUIEventArgs &e);
    
    ofxUICanvas * gui;
    
    string hostname;// = "127.0.0.1";
    int port = 7010;
    
    int clientId;
    
    string label;// = "leaf.local";
        
    vector<ofColor> colors;
    
    ofPoint inputPos;
    int width = 1;
    int height = 400;
    
    ofxOscSender * osc;
    
    bool connected = false;
    bool enabled = false;
    
    bool testBlink = false;
    
    int long lastCallback = 0;
    
    bool setRemove = false;
    
    int long lastCmdTime = 0;

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
    
    void createClientGui();
    
    void saveSettings();
    void setGui();
    void setGuiTabBar();
    void updateGui();
    
    int numClients;
    float inputWidth  = 480;
    float inputHeight = 480;
    
    bool autoEnable = true;
    float inputscale;
    
    int lastId;
    
    string updateMethod = "packed";
    Client* handshakeClient(string hostname, int clientId);
    
    
    int selectedClientIndex;
    Client * selectedClient;
    
    ofxUICanvas *gui;
    
    void resetGuiTabBar();
    
    ofxUITabBar *guiTabBar;
    
    //ofxUIScrollableCanvas *clientsGui;
    
	void guiEvent(ofxUIEventArgs &e);
    
	ofxOscSender oscBroadcast;
    ofxOscReceiver oscReceiver;
	
	ofxXmlSettings xml;
    
    vector<Client *> clients;
    ofxSyphonClient syphonIn;
    
    ofxSyphonServerDirectory directory;
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverUpdated(ofxSyphonServerDirectoryEventArgs &args);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    int dirIdx;
    
    ofxUIDropDownList * syphonDropdown;
    
    ofFbo fboIn;
	ofTexture controlTexture;
    ofPixels controlPixels;
    
    bool monitorInput;
    bool monitorOutput;
    bool connect;
    bool viewInfo;
    
    bool testBlink;
    
    bool listenForNewClients = true;
    
    ofxXmlSettings settings;
    
    vector<string> syphonInputs;
    
    ofTrueTypeFont font;
    
	
};
