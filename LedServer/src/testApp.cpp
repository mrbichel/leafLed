#include "testApp.h"

const int numClients = 20;

const int inputWidth = 202;
const int inputHeight = 260;

void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofSetFrameRate(60);
    
    // syphon input
    
    syphonIn.setup();
    //syphonIn.setApplicationName("MadMapper");
     syphonIn.setApplicationName("Modul8");
    syphonIn.setServerName("");
        
    fboIn.allocate(inputWidth, inputHeight);
    // Setup all clients
    
    for(int i=0; i<numClients; i++) {
        addClient();
        
    }
    
}


void testApp::addClient() {
    
    int index = clients.size();
    
    Client c;
    c.width = 1;
    c.height = 240;
    
    
    c.fboOut.allocate(c.width, c.height);
    //c.fboOut.clear();
    
    //c.dataTexture.allocate(c.width, c.height, GL_RGB);
    //c.dataTexture.clear();
    
    //c.hostname = "leaf"+ofToString(i)+".local";
    c.hostname = "127.0.0.1";
    c.port = 2838;
    
    c.osc = new ofxOscSender();
    c.osc->setup(c.hostname, c.port);
    clients.push_back(c);
        
    clients[index].inputPos.set(1+ index*5, 20);
    
}

void Client::update() {
    
    
    //m.setRemoteEndpoint(hostname, port);
    
    /*for(int i = 0; i < data.size(); i++) {
        
        
        ofxOscMessage m;
        m.setAddress("/l");
        m.addFloatArg(data[i]);
        
        
        osc->sendMessage(m);
        
    }*/
    
    
}

//--------------------------------------------------------------
void testApp::update(){
	for(int i=0; i<clients.size(); i++) {
        
        // Send new frame
        clients[i].update();
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0);
    
    fboIn.begin();
    
    ofBackground(0);
    syphonIn.draw(0, 0, fboIn.getWidth(), fboIn.getHeight());
    
    fboIn.end();
    
    controlTexture = fboIn.getTextureReference();
    
    ofPushMatrix();
    ofTranslate(200, 20);
    ofScale(1, 1);
    
    ofSetColor(255);
    ofRect(-1, -1, inputWidth+2, inputHeight+2);
    controlTexture.draw(0,0, inputWidth, inputHeight);
    
    for(int i=0; i<clients.size(); i++) {
        
        ofNoFill();
        ofRect(clients[i].inputPos.x, clients[i].inputPos.y, clients[i].width, clients[i].height);
        
        ofFill();
        
        clients[i].fboOut.begin();
        ofBackground(0);
        
        
        controlTexture.drawSubsection(0, 0, clients[i].width, clients[i].height, clients[i].inputPos.x, clients[i].inputPos.y);
        
        //controlTexture.drawSubsection(10, 10, 1, 240, 10, 10, 1, 240);
        
        //controlTexture.drawSubsection(clients[i].inputPos.x, clients[i].inputPos.y,
        //                              clients[i].width, clients[i].height,
        //                              clients[i].width, clients[i].height);
                                    
        clients[i].fboOut.end();
        
        
    }
    
    ofPopMatrix();
    
    
    ofPushMatrix();
    ofTranslate(420, 20);
    
    ofNoFill();
    ofRect(0,0,10*clients.size()+4, 404);
    
    ofFill();
    
    for(int i=0; i<clients.size(); i++) {
        
        clients[i].fboOut.getTextureReference().draw(i*10+4,2,6,400);
        //clients[i].fboOut.draw(0, 0);
    }
    
    ofPopMatrix();
    
    //controlTexture.readToPixels()
    
    ofDrawBitmapString("LED Server", 10, 20);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 10, 60);
    
    
}

void testApp::exit(){
    
    
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
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