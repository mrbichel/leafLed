#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	led = NULL;
	ofxXmlSettings xml;
	int port = 2838;

    ofSetFrameRate(60);
    
	receiver.setup(port);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(0);
    
	if(	spi.connect())
	{
		ofLogNotice()<<"connected to SPI";
	}
    
    refreshRate = 60; // How many times persecond to update all LEDS in strip    
    autoModeDelay = 10000; // Go in auto mode after 16 seconds
    autoMode = true;
    
    
	numLED = 120;
	led = new ofxLEDsLPD8806(numLED);
    ledData.assign(numLED,ofColor(255, 255, 255));
    
	startThread(false,false);
    
	ofLogNotice("OSC") << " Set LED length as " << numLED;
    
    sender.setup("0.0.0.0", 7777);
    ofxOscMessage m;
    m.setAddress("/status");
    m.addIntArg(2);
    m.addStringArg("Ready to receive data.");
    sender.sendMessage(m);
        
}

void testApp::exit()
{
	if(led!=NULL)
	{
        stopThread();
		led->clear(0);
		spi.send(led->txBuffer);
	}
}

//--------------------------------------------------------------
void testApp::threadedFunction()
{
	while( isThreadRunning() != 0 ){
		if( lock() ){
			led->setPixels(ledData);
			spi.send(led->txBuffer);
			unlock();
			usleep(10000);
		}
	}
}

//--------------------------------------------------------------
void testApp::update(){
	// check for waiting messages
	while(receiver.hasWaitingMessages()){
        
        lastCmdTime = ofGetElapsedTimeMillis();
        
		// get the next message
		ofxOscMessage m;
                
		receiver.getNextMessage(&m);
        
        if ( m.getAddress() == "/l" ){ // Set a single led
            
            ledData[m.getArgAsInt32(0)].set(m.getArgAsInt32(1), m.getArgAsInt32(2), m.getArgAsInt32(3), 255);
            
            ofLogNotice("OSC") << "l: " << ofToString(m.getArgAsInt32(0)) << " l - r:" << ofToString(m.getArgAsInt32(1)) << "g:" <<ofToString(m.getArgAsInt32(2)) << "b:" << ofToString(m.getArgAsInt32(3))<<endl;
            
            lastLedCmdTime = ofGetElapsedTimeMillis();
            autoMode = false;
            
        } else if ( m.getAddress() == "/p" ) { // Set all
            
            ofLogNotice("got msg");
            if( ofGetElapsedTimeMillis() - lastLedCmdTime > (refreshRate/60 * 1000) ) {
                
                ofLogNotice("OSC") << "Got packed. First color: " << ofToString(m.getArgAsInt32(0)) << " r:" << ofToString(m.getArgAsInt32(1)) << "g:" << ofToString(m.getArgAsInt32(2))<<endl;
                
                int led = 0;
                for(int i=0; i<numLED; i++) {
                    ledData[i].set(m.getArgAsInt32(led),m.getArgAsInt32(led+1),m.getArgAsInt32(led+2));
                    led +=3;
                }
                lastLedCmdTime = ofGetElapsedTimeMillis();
                
            }
            autoMode = false;
            
        } else if ( m.getAddress() == "/c" ) {
            // Compressed data not yet implemented
       
        } else if ( m.getAddress() == "/status" ) {
            
            sender.setup(m.getRemoteIp(), m.getRemotePort());
            ofxOscMessage r;
            r.setAddress("/status");
            r.addStringArg(m.getArgAsString(0));
            if(autoMode) {
                r.addIntArg(2);
                r.addStringArg("Not receiving data. Running auto mode.");
            } else {
                r.addIntArg(1);
                r.addStringArg("All good, alive and listening.");
            }
            sender.sendMessage(r);
            
            
        } else if ( m.getAddress() == "/debug" ) {
            
            if(m.getArgAsInt32(0) == 1) {
                ofSetLogLevel(OF_LOG_VERBOSE);
            } else {
                ofSetLogLevel(OF_LOG_SILENT);
            }
            
        
        } else if ( m.getAddress() == "/setLength" ) {
            
            numLED = m.getArgAsInt32(0);
            
            led = new ofxLEDsLPD8806(numLED);
            ledData.assign(numLED,ofColor());
            
            
		} else {
            
			// unrecognized message: display on the bottom of the screen
            
            if(ofGetLogLevel() == OF_LOG_VERBOSE) {
                
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
	}
    
    if( ofGetElapsedTimeMillis() - lastLedCmdTime > autoModeDelay ) {
        autoMode = true;
    }
    
    if(autoMode) {
        
        
        // sine function
        // Amplitude * sin( frequency(oscillations pr second) * time in seconds + phase (shift in seconds))
        
        // 255 * sin(4+ofGetElapsedTimef())
        
        float t =ofGetElapsedTimeMillis()/1000.0;
        
        float fade = ofMap(sin(2*t), -1, 1, 0.5, 1);
        
        float position = ofMap(sin((ofNoise(t)-1)*t), -1, 1, 0, numLED);
        
        
        float blink = ofMap(sin(40*t), -1, 1, 0, 1);
        
        
        float blinkPhase = ofMap(sin(1.2*t), -1, 1, 0, 1);
        
        
        //if(blinkPhase > 0.8) {
        //    fade = blink;
        //}
        
        
        for(int i=0; i<numLED; i++) {
            
            if(blinkPhase > 0.9) {
                
                if(blink > 0.9) {
                    ledData[i].set(140, 140, 140);
                } else {
                    ledData[i].set(ledData[i].r * 0.6, ledData[i].g * 0.6, ledData[i].b * 0.6);
                }
                
            } else {
            
                if(i == floor(position)) {
                    ledData[i].set(200, 90, 90);
                } else {
                    ledData[i].set(ledData[i].r * 0.99, ledData[i].g * 0.96, ledData[i].b * 0.96);
                }
            
            }
        }
        
        
        
    }
    

}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    //if(ofGetLogLevel() == OF_LOG_VERBOSE) {
        
        ofBackground(20);
        int size = ofGetWidth()/numLED;
        
        for(int i =0 ; i < numLED; i++)
        {
            ofSetColor(ledData[i]);
            ofFill();
            ofRect(i * size, 0, size, ofGetHeight()/2);
        }
    
    
    ofSetColor(255);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 20, ofGetHeight()-20);
    
    
    ofDrawBitmapString("autoMode: " + ofToString(autoMode), 20, ofGetHeight()-40);
    
    
    ofDrawBitmapString("lastLedCmdTime: " + ofToString(lastLedCmdTime), 20, ofGetHeight()-60);
        
    //}
    
    
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