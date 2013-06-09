#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	led = NULL;
	ofxXmlSettings xml;
	int port = 2838;
	if(xml.loadFile("config.xml"))
	{
		xml.pushTag("DATA");
		port = xml.getValue("PORT",2838);
		xml.popTag();
	}
	receiver.setup(port);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofBackground(0);
    ofSetFrameRate(60);
	ofEnableAlphaBlending();
	
	if(	spi.connect())
	{
		ofLogNotice()<<"connected to SPI";
	}
    
	lastFrameTime = 0;
	numLED = 240;
    
	led = new ofxLEDsLPD8806(numLED);
    
    for(int i = 0; i < numLED; i++) {
        ledData.push_back(ofColor(255));
    }
    
    
	ofLogNotice("OSC") << " Set LED length as " << numLED;
}

void testApp::exit()
{
	if(led!=NULL)
	{
		led->clear(ofColor(255));
		spi.send(led->txBuffer);
	}
}

//--------------------------------------------------------------
void testApp::update(){
	float  dt = 1.0f / ofGetFrameRate();
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;

		receiver.getNextMessage(&m);
		
        if ( m.getAddress() == "/l" ){ // Set a single led
            ledData[m.getArgAsInt32(0)].set(m.getArgAsFloat(1)*255, m.getArgAsFloat(2)*255, m.getArgAsFloat(3)*255);
        
        } else if(m.getAddress() == "/lc"){ // Set a complete frame
            
            for(int i=0; i < ledData.size(); i++) {
                ledData[i].set(m.getArgAsFloat(i)*255, m.getArgAsFloat(i+1)*255, m.getArgAsFloat(i+2)*255);
            }
        
        } else if(m.getAddress() == "/led/all"){ // set all at once
			led->clear(ofColor(m.getArgAsInt32(0),m.getArgAsInt32(1),m.getArgAsInt32(2)));
            
		} else {
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			ofLogWarning("OSC") << " Unknown osc message " << msg_string;
		}
	}
	
	if(led!=NULL)
	{
        // sending stuff
        led->setPixels(ledData);
		led->encode();
		spi.send(led->txBuffer);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if(ofGetLogLevel()==OF_LOG_VERBOSE)
	{
		if(led!=NULL)
		{
			ofPushMatrix();
			led->renderBuffer.draw(0,0,ofGetWidth(),5);
			led->encodedBuffer.draw(0,5,ofGetWidth(),5);
			ofPopMatrix();
		}
	}
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