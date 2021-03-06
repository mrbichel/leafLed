#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    // todo if id in xml set id
    state = STATE_WAITING;
    
    settings.loadFile("settings.xml");
    clientId = settings.getValue("id", 0);
    label = settings.getValue("label", "unnamed leafLed");
    height = 300; //settings.getValue("height", 300);;
    
	led = NULL;
	ofxXmlSettings xml;
    
    helloResponseWait = 1000;
	int port = 7010;
    
    masterHostname = settings.getValue("master", "leafMaster.local");
    masterPort = 7020;
    
    sender.setup(masterHostname, masterPort);
    
    char hostnamestr[40];
    hostname = gethostname(hostnamestr, 40);
    
    ofHideCursor();
    ofSetFrameRate(30);
    
	receiver.setup(port);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(0);
    
	if(	spi.connect())
	{
		ofLogNotice()<<"connected to SPI";
	}
    
    //refreshRate = 60; // How many times persecond to update all LEDS in strip
    autoModeDelay = 10000; // Go in auto mode after 16 seconds
    autoMode = true;
    
    width = 1; // todo add support for pixel grids
	led = new ofxLEDsLPD8806(height);
    ledData.assign(height,ofColor(255, 255, 255));
    
	startThread(false,false);
	ofLogNotice() << " Set LED length as " << height;
    
}

void testApp::exit()
{
    saveSettings();
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
                        
            lastLedCmdTime = ofGetElapsedTimeMillis();
            autoMode = false;
            
        } else if ( m.getAddress() == "/p" ) { // Set all
            
            //if( ofGetElapsedTimeMillis() - lastLedCmdTime > 2 ) {
                                
                int led = 1;
                int section = m.getArgAsInt32(0);
                int start   = section;
                int end     = start+120;
                
                for(int i=start; i<end && i<height; i++) {
                    
                    if(led+2 < m.getNumArgs()) {
                        ledData[i].set(m.getArgAsInt32(led),m.getArgAsInt32(led+1),m.getArgAsInt32(led+2));
                    }
                    
                    led +=3;
                }
                lastLedCmdTime = ofGetElapsedTimeMillis();
                
            //}
            autoMode = false;
            
        } else if ( m.getAddress() == "/c" ) {
            // Compressed data not yet implemented
            if( ofGetElapsedTimeMillis() - lastLedCmdTime > 2 ) {
                
                lastLedCmdTime = ofGetElapsedTimeMillis();
            
            }
            
       
        } else if ( m.getAddress() == "/status" ) {
            
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
            
            r.addStringArg(hostname);
            sender.sendMessage(r);
            
        } else if ( m.getAddress() == "/debug" ) {
            
            if(m.getArgAsInt32(0) == 1) {
                ofSetLogLevel(OF_LOG_VERBOSE);
            } else {
                ofSetLogLevel(OF_LOG_SILENT);
            }
            
        
        } else if ( m.getAddress() == "/setLength" ) {
            height  = m.getArgAsInt32(0);
            led     = new ofxLEDsLPD8806(height);
            ledData.assign(height,ofColor());
            ofLogNotice() << " Set LED length to " << height;

		} else if ( m.getAddress() == "/setId" ) {
            
            clientId = m.getArgAsInt32(0);
            saveSettings();
            state = STATE_CONNECTED;
            
            ofLogNotice() << " Set ID to " << clientId;

        } else if ( m.getAddress() == "/setMaster" ) {
            
            sender.setup(masterHostname, masterPort);
            masterHostname = m.getArgAsString(0);
            saveSettings();
            
            ofLogNotice() << " Set Master hostname to " << masterHostname;
            
        }
        
	}
    
    if( ofGetElapsedTimeMillis() - lastLedCmdTime > autoModeDelay ) {
        autoMode = true;
        state = STATE_WAITING;
    }
    
    
    if(state == STATE_WAITING) {
        
        if(ofGetElapsedTimeMillis() - helloTime > helloResponseWait) {
            helloTime = ofGetElapsedTimeMillis();
            ofxOscMessage m;
            
            m.setAddress("/hello");
            m.addIntArg(clientId);
            sender.sendMessage(m);
        }
    }

}

void testApp::saveSettings() {
    
    settings.setValue("id", clientId);
    settings.setValue("label", label);
    settings.setValue("height", height);
    settings.setValue("width", width);
    settings.setValue("master", masterHostname);
    settings.saveFile("settings.xml");
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    if( ofGetElapsedTimeMillis() - lastLedCmdTime > autoModeDelay/2 && !autoMode ) {
        
        // fade out when not receiving a signal for over half autoModeDelay duration
        for(int i=0; i<height; i++) {
            ledData[i].set(ledData[i].r * 0.97, ledData[i].g * 0.97, ledData[i].b * 0.97);
        }
        
    }
    
    if(autoMode) {
        
        // sine function
        // Amplitude * sin( frequency(oscillations pr second) * time in seconds + phase (shift in seconds))
        
        // 255 * sin(4+ofGetElapsedTimef())
        
        float t =ofGetElapsedTimeMillis()/1000.0;
        
        float fade = ofMap(sin(2*t), -1, 1, 0.5, 1);
        
        if(ofGetFrameNum() % 30 == 1) {colorTestBlink = !colorTestBlink; }
        
        position += 1;
        
        if(position > height-1) {
            position = 0; 
        }
        
        
        for(int i=0; i<height; i++) {
            if(colorTestBlink) {
                ledData[i].set(10, 255, 10);
            } else {
                ledData[i].set(255, 10, 10);
            }
        }
        
        ledData[position].set(0, 0, 0);
        ledData[position+1].set(0, 0, 0);
    }
    
    
    ofBackground(20);
    float size = ofGetWidth()*1.0 / height;
        
    ofFill();
    for(int i =0 ; i < height; i++)
    {
        ofSetColor(ledData[i].r, ledData[i].b, ledData[i].g);
        ofRect(i * size, 0, size, ofGetHeight()/2);
    }
    
    
    ofSetColor(255);
    
    ofPushMatrix();
    
    ofTranslate(60, 0);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 00, ofGetHeight()-20);
    
    string autoString = "off";
    if(autoMode) autoString = "on";
    
    ofDrawBitmapString("Automatic mode: " + autoString, 0, ofGetHeight()-40);
    
    ofDrawBitmapString("Millis since last command: " + ofToString((ofGetElapsedTimeMillis() - lastLedCmdTime) ), 0, ofGetHeight()-60);
    
    
    string strstate;
    if(state == STATE_CONNECTED) {
        strstate = "Connected";
    } else if(STATE_WAITING) {
        strstate = "Waiting for signal";
    }
    
    ofDrawBitmapString("State: " + strstate, 0, ofGetHeight()-80);
    ofDrawBitmapString("id: " + ofToString(clientId), 0, ofGetHeight()-100);
    ofDrawBitmapString("Master: " + masterHostname + " port: " + ofToString(masterPort), 0, ofGetHeight()-120);
    ofDrawBitmapString("Pixels: " + ofToString(height), 0, ofGetHeight()-140);
    ofPopMatrix();
    
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