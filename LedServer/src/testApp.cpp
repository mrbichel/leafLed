#include "testApp.h"

void testApp::setup(){
    
    ofSetFrameRate(60);
    
    // TODO add gui for configuring nodes
	ofSetLogLevel(OF_LOG_WARNING);
    oscReceiver.setup(7020);
    
    // syphon input
    syphonIn.setup();
    syphonIn.setApplicationName("MadMapper");
    //syphonIn.setApplicationName("Modul8");
    syphonIn.setServerName("");
    
    directory.setup();
    ofAddListener(directory.events.directoryUpdated,this,&testApp::directoryUpdated);
    dirIdx = -1;
        
    fboIn.allocate(inputWidth, inputHeight);
    controlTexture.allocate(inputWidth, inputHeight, GL_RGB);
    
    //fboPixelTransfer.allocate(1, 120);
    // Setup all clients
    
    xml.loadFile("config.xml");
    
    numClients = xml.getNumTags("DEVICE");
    
    if(numClients > 0){
        for(int i = 0; i < numClients; i++){
            string hostname = xml.getValue("DEVICE:HOST", "localhost", i);
            addClient(hostname);
        }
    }
    
    cout<<numClients<<endl;
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    gui = new ofxUIScrollableCanvas(0,0,length+xInit*2.0,ofGetHeight());
    
    gui->setScrollAreaToScreen();
    gui->setScrollableDirections(false, true);
    
	gui->addWidgetDown(new ofxUILabel("LeafLED", OFX_UI_FONT_LARGE));
    gui->addSpacer(length, 2);
    
    gui->addWidgetDown(new ofxUIFPSSlider("fps", length, 10));
    
    gui->addSpacer(length, 2);
    
    gui->addWidgetDown(new ofxUIButton("Enable all", true, 10, 10));
    gui->addWidgetDown(new ofxUIToggle("Monitor input",  &monitorInput, 10, 10));
    gui->addWidgetDown(new ofxUIToggle("Monitor output", &monitorOutput, 10, 10));
    gui->addWidgetDown(new ofxUIToggle("View info", &viewInfo, 10, 10));

    gui->addSpacer(length, 4);
    
    gui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    for(int i=0; i<numClients; i++) {
        gui->addWidgetDown(new ofxUILabel("Unit - " + ofToString(i+1) + ". Hostname: " + clients[i].hostname, OFX_UI_FONT_SMALL));
        
        //gui->addTextInput("Hostname", clients[i].hostname)->setID(i);
        
        gui->addWidgetDown(new ofxUIToggle("Enable", &clients[i].enabled, 10, 10))->setID(i);
        gui->addWidgetRight(new ofxUIToggle("Connected", &clients[i].connected, 10, 10))->setID(i);
        gui->addWidgetRight(new ofxUIToggle("Test", &clients[i].testBlink, 10, 10))->setID(i);
        gui->addSpacer(length, 3);          
    }
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    //gui->loadSettings("GUI/guiSettings.xml");
}

void testApp::guiEvent(ofxUIEventArgs &e) {
    
	int kind = e.widget->getKind();
    
    if(e.widget->getName() == "Enable all") {
        
        for(int i=0; i<numClients; i++) {
            clients[i].osc->setup(clients[i].hostname, clients[i].port);
            clients[i].enabled = true;
        }
        
    }
    
    int id = e.widget->getID();
    
    if(e.widget->getName() == "Enable") {
        
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        
        clients[id].enabled = toggle->getValue();
        if(clients[id].enabled) {
            clients[id].osc->setup(clients[id].hostname, clients[id].port);
        } else {
            clients[id].connected = false;
        }
    }
    
    /*if(name.find("hostname" > 0))
    {
        for(int i=0; i<clients.size();i++) {
            
            if(clients[i].label + "_hostname" == name) {
        
                ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
                if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER) {
                    cout << "ON ENTER: ";
                    //            ofUnregisterKeyEvents((testApp*)this);
                } else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS) {
                    cout << "ON FOCUS: ";
                } else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS) {
                    cout << "ON BLUR: ";
                    //            ofRegisterKeyEvents(this);
                }
                
                string output = textinput->getTextString();
                clients[i].hostname == textinput->getTextString();
                textinput->setTextString(clients[i].hostname);
                
                cout << output << endl;
                
            }
        }
    }*/
}

void testApp::addClient(string hostname) {
    
    int index = clients.size();
    
    Client c;
    
    c.colors.assign(c.height, ofColor(255));
    c.osc = new ofxOscSender();
    
    c.label = "leaf" + ofToString(index+1);
    c.hostname = hostname;
    
    /*if(index == 1) {
        c.hostname = "127.0.0.1";
    }*/
    
    //c.osc->setup(c.hostname, c.port);
    clients.push_back(c);
    clients[index].inputPos.set(1+ index*5, 20);
    
}

void Client::update(string method) {
    
    
    // TODO output data from framebuffer texture
    
    //m.setRemoteEndpoint(hostname, port);    
    
    if(connected && enabled) {
        
        
        // Performance notes
        // raw pack and send: 16 FPS
        // Only pack: 36 FPS
        // Only update pixelsArray: 70 - 100 FPS
        // Conclusion: send fewer OSC packages
        
        // Packed
        // 50 - 80 FPS
        
        if(method == "raw") {
            // Send each pixel as a OSC message
            
            for(int i=0; i < height; i++) {
                
                ofxOscMessage m;
                m.setAddress("/l");
                 
                m.addIntArg(i);
                
                m.addIntArg(colors[i].r);
                m.addIntArg(colors[i].b); // yes this is reversed
                m.addIntArg(colors[i].g);
                
                //cout<<"C color r: "<<ofToString(c.r)<<endl;
                
                osc->sendMessage(m);
                
            }
        } else if(method == "packed") { // does this break at size limit?
            // Send enitre frame as one OSC message
            
            ofxOscMessage m;
            m.setAddress("/p");
            
            for(int i = 0; i < colors.size(); i++) {
                m.addIntArg(colors[i].r);
                m.addIntArg(colors[i].b); // yes this is reversed
                m.addIntArg(colors[i].g);
            }
                        
            osc->sendMessage(m);
            
        } else if(method == "compressed") {
            // TODO: send a compressed frame over osc and decompress it on clients
        }
    }
    
    if(!connected && enabled) {
        if(ofGetElapsedTimeMillis() % 100 == 1) {
            ofxOscMessage m;
            m.setAddress("/status");
            m.addStringArg(label);
            osc->sendMessage(m);
            
            ofLogWarning(ofToString(hostname) + ": Not connected!");
        }
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    while(oscReceiver.hasWaitingMessages()){
        
		// get the next message
		ofxOscMessage m;
        
		oscReceiver.getNextMessage(&m);        
        if ( m.getAddress() == "/status" ){
            
            string label = m.getArgAsString(0);
            int status = m.getArgAsInt32(1);
            string message = m.getArgAsString(2);
            
            
            ofLogNotice("Got Status callback from: " + label + " status: " + ofToString(status) + "   " + message );
            
            for(int i=0; i<clients.size(); i++) {
                if(clients[i].label == label) {
                    if(status > 0) {
                        clients[i].connected = true;
                    }
                }
            }
        
            
        }
    }
    
    
	for(int i=0; i<clients.size(); i++) {
        //if(ofGetFrameNum() % 4 == 1)
        clients[i].update(updateMethod);
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
        
    ofEnableAlphaBlending();
    ofSetColor(255);
    ofBackground(30);
    
    // Get input pixels from syphon through an fbo
    fboIn.begin();
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        ofBackground(0);
    
            syphonIn.draw(0, 0, fboIn.getWidth(), fboIn.getHeight());
    
        glDisable(GL_BLEND);
        ofClearAlpha();
    fboIn.end();
    controlTexture = fboIn.getTextureReference();
    ofDisableAlphaBlending();
    
    
    // Draw the input and copy to output color data
    ofPushMatrix();
    ofTranslate(400, 20);
    ofSetColor(255);
    ofNoFill();
    ofRect(-1, -1, inputWidth+2, inputHeight+2);
    controlTexture.draw(0,0, inputWidth, inputHeight);
    
    for(int i=0; i<clients.size(); i++) {
        
        ofNoFill();
        if(viewInfo) {
            ofRect(clients[i].inputPos.x, clients[i].inputPos.y, clients[i].width+1, clients[i].height);
        }
        ofFill();
        
        /*fboPixelTransfer.begin();
        ofBackground(0);
        controlTexture.drawSubsection(0, 0, clients[i].width, clients[i].height,
                                      clients[i].inputPos.x, clients[i].inputPos.y);
        fboPixelTransfer.end();*/
        
        controlTexture.readToPixels(controlPixels);
        for(int p = 0; p < clients[i].height; p++) {
            clients[i].colors[p] = controlPixels.getColor(clients[i].inputPos.x, clients[i].inputPos.y+p);
        }
        
        if(clients[i].testBlink) {
            for(int p = 0; p < clients[i].height; p++) {
                clients[i].colors[p].set(255,0,0);
            }
        }
        
    }
    ofPopMatrix();

    ofPushMatrix();
    ofTranslate(620, 20);
    ofFill();
    
    if(monitorOutput) {    
        for(int i=0; i<clients.size(); i++) {
        
            // clients debug draw  - draw from input texture instead.
            for(int c=0; c<clients[i].colors.size(); c++) {
                ofSetColor(clients[i].colors[c]);
                ofRect(i*10+4, 10+c, 6, 1);
            }
            
        }
    }
    
    ofSetColor(255, 255, 255);
    
    ofPopMatrix();
    
}

void testApp::exit(){
        
    //gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    
}


void testApp::directoryUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& server : arg.directory->getServerList() ){ //new c++ auto keyword
        ofLogNotice("ofxSyphonServerDirectory Updated:: ")<<" Server Name: "<<server.serverName <<" | App Name: "<<server.appName;
    }
    dirIdx = 0;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    dirIdx++;
    if(dirIdx > directory.size() - 1)
        dirIdx = 0;
    
    if(directory.isValidIndex(dirIdx)){
        syphonIn.setServerName(directory.getServerList()[dirIdx].serverName);
        syphonIn.setApplicationName(directory.getServerList()[dirIdx].appName);
    }
    
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


