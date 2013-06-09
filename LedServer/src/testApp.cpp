#include "testApp.h"

const int numClients = 1;


void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofSetFrameRate(60);
    
    // syphon input
    
    syphonIn.setup();
    syphonIn.setApplicationName("LED Output");
    syphonIn.setServerName("");
    
    
    // Setup all clients
    
    for(int i=0; i<numClients; i++) {
        Client c;
        c.width = 240;
        c.height = 1;
        
        c.data.allocate(c.width, c.height, 3);
        c.data.clear();
        
        //c.dataTexture.allocate(c.width, c.height, GL_RGB);
        //c.dataTexture.clear();
        
        c.hostname = "localhost";
        c.port = 2838;
        
        c.osc.setup(c.hostname, c.port);
        
        clients.push_back(c);
    }
    
    
    
    
}

void Client::update() {
    
    ofxOscMessage m;
    m.setRemoteEndpoint(hostname, port);
    m.setAddress("/lc");
    for(int i = 0; i < data.size(); i++) {
        m.addFloatArg(data[i]);
    }
    osc.sendMessage(m);
    
}

//--------------------------------------------------------------
void testApp::update(){
	for(int i=0; i<numClients; i++) {
        
        // Send new frame
        clients[i].update();
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	
    
    syphonIn.draw(0, 0);
    
    ofDrawBitmapString("LED Server", 10, 10);
    
    
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