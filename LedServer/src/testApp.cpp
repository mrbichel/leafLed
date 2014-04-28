#include "testApp.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <ofURLFileLoader.h>

void testApp::setup(){
    
    selectedClient = NULL;
    
    ofSetFrameRate(30);
    
    // TODO add gui for configuring nodes
	ofSetLogLevel(OF_LOG_NOTICE);
    oscReceiver.setup(7020);

    // syphon input
    // TODO add select Syphon input
    syphonIn.setup();
    
    directory.setup();
    //ofAddListener(directory.events.serverUpdated,this,&testApp::directoryUpdated);
    
    //register for our directory's callbacks
    ofAddListener(directory.events.serverAnnounced, this, &testApp::serverAnnounced);
    ofAddListener(directory.events.serverUpdated, this, &testApp::serverUpdated);
    ofAddListener(directory.events.serverRetired, this, &testApp::serverRetired);
    dirIdx = -1;
        
    fboIn.allocate(inputWidth, inputHeight);
    controlTexture.allocate(inputWidth, inputHeight, GL_RGB);
    
    
    // Load from xml
    settings.loadFile("settings.xml");
    lastId = settings.getValue("settings:lastId", 0.0);
    
    if(settings.tagExists("clients")) {
        settings.pushTag("clients");
        for(int i = 0; i<settings.getNumTags("client"); i++) {
            settings.pushTag("client", i);
            
            Client * c = new Client;
            c->hostname =   settings.getValue("hostname",  "127.0.0.1");
            c->clientId =   settings.getValue("clientId",  0);
            c->label =      settings.getValue("label",     "");
            c->inputPos.x = settings.getValue("inputPosX", 0);
            c->inputPos.y = settings.getValue("inputPosY", 0);
            
            if(c->clientId != 0) clients.push_back(c);
            
            settings.popTag();
        }
        settings.popTag();
    }
    
    
    monitorInput = true;
    monitorOutput = true;
    font.loadFont("Gui/DINNextLTPro-Regular.ttf", 12);

    setGui();
    
}

void testApp::setGui() {
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    /* Adding GUI */
    gui = new ofxUICanvas();
    // Changing default GUI font to DinPRO Regular
    
    gui->setFont("Gui/DINNextLTPro-Regular.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 24);
    gui->setColorFill(ofColor(255, 255, 255));
    
	gui->addWidgetDown(new ofxUILabel("LeafLED", OFX_UI_FONT_LARGE));
    gui->addSpacer(length, 4);
    
    gui->addWidgetDown(new ofxUIFPSSlider("fps", length, 10));
    //gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUIToggle("View input overlay",      &viewInfo,      10, 10));
    
    //gui->addLabelButton("Reset", false);
    //gui->addSlider("Input scale", 0.01, 2, &inputScale);
    //gui->addSlider("Input width",  0, 2000, &inputWidth);
    //gui->addSlider("Input height", 0, 2000, &inputHeight);
    
    //gui->addSpacer(length, 2);
    
    // Greying LeafList a little
    gui->setColorFill(200);
    
    gui->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
    
    //setGuiTabBar();
    
    for(int i=0; i<clients.size(); i++) {
        clients[i]->setup();
    }
    
}

void testApp::setGuiTabBar() {
    
    /*guiTabBar = new ofxUITabBar();
    guiTabBar->setPosition(10, gui->getRect()->height+20);
    guiTabBar->setFont("Gui/DINNextLTPro-Regular.ttf");
    guiTabBar->setFontSize(OFX_UI_FONT_LARGE, 24);
    
    for(int i=0; i<clients.size(); i++) {
        clients[i]->setup();
    }
    */
}


void testApp::guiEvent(ofxUIEventArgs &e) {
	int kind = e.widget->getKind();
    
        for(int i=0; i<clients.size(); i++) {
            if(clients[i]->clientId == e.widget->getID()) {
                
                if(e.widget->getName() == "Length") {
                    ofxUIIntSlider *n = (ofxUIIntSlider *) e.widget;
                    clients[i]->updateHeight(n->getValue());
                }
                
            }
        }

}

void Client::setId() {
    
    if(connected) {
        ofxOscMessage setId;
        setId.setAddress("/setId");
        setId.addIntArg(clientId); // set to a uuid
        osc->sendMessage(setId);
    }
    
    delete gui;
    setGui();
    
}


void Client::newId() {
    
    clientId = ofGetUnixTime();
    
    if(connected) {
        ofxOscMessage setId;
        setId.setAddress("/setId");
        setId.addIntArg(clientId); // set to a uuid
        osc->sendMessage(setId);
    }
    
}

Client*  testApp::handshakeClient(string hostname, int _clientId) {
    bool exists = false;
    bool change = false;
    Client * c;
    
    if(_clientId != 0) {
    
        for(int i = 0; i < clients.size(); i++) {
            if(_clientId == clients[i]->clientId) {
                exists = true;
                c = clients[i];
                ofLogNotice()<<"Client with id already exists. Will update hostname.";
                break;
            }
        }
    }
    
    if(!exists) {
        c = new Client();
        c->inputPos.set(1+ clients.size()*5, 20);
        
        clients.push_back(c);
    }
    
    if(_clientId == 0) {
        void newId();
    }
    
    c->osc->setup(c->hostname, c->port);
    c->clientId = _clientId;
    c->connected = true;
    
    if(c->hostname != hostname) {
        c->hostname = hostname;
        
        change = true;
        if(exists) { delete c->gui;
            c->setGui();
        }
    }
    
    
    if (!exists) c->setGui();
    
    c->setId();
    
    if(autoEnable) c->enabled = true;

    
    //guiTabBar->exit();
    //delete guiTabBar;
    //setGuiTabBar();
    
    
    
    return c;
}

void Client::updateHeight(int _height) {
        height = _height;
        colors.assign(height, ofColor(255));
        
        if(connected) {
            ofLogNotice()<<"Sending set length";
            ofxOscMessage m;
            m.setAddress("/setLength");
            m.addIntArg(height);
            osc->sendMessage(m);
        }
}

void Client::setup() {
    setGui();
}


void Client::setGui() {
    
    testApp* app = ((testApp*)ofGetAppPtr());
    
    gui = new ofxUICanvas;
    
    gui->setName("[" + ofToString(clientId) + "]  " + hostname);
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    gui->setFont("Gui/DINNextLTPro-Regular.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 24);
    gui->setColorFill(ofColor(255, 255, 255));
    
        gui->addWidgetDown(new ofxUILabel("Selected Client", OFX_UI_FONT_MEDIUM));
    
        gui->addWidgetDown(new ofxUILabel("[" + ofToString(clientId) + "]  " + hostname, OFX_UI_FONT_MEDIUM));
    
        gui->addSlider("Position x", 0, app->inputWidth, &inputPos.x);
        gui->addSlider("Position y", 0, app->inputHeight, &inputPos.y);
        gui->addIntSlider("Length", 1, 600, height);
        gui->addToggle("Enabled", &enabled);
        
        gui->addToggle("Connected", &connected);
    
        gui->addLabelButton("Remove", false);
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&Client::guiEvent);
    
    gui->setPosition(10, app->gui->getRect()->height+200);
    gui->setVisible(false);
    
    // add to the tab bar menu
    //app->guiTabBar->update();
    //app->guiTabBar->addCanvas(gui);
    
}


void Client::guiEvent(ofxUIEventArgs &e) {
	int kind = e.widget->getKind();

    if(e.widget->getName() == "Length") {
        ofxUIIntSlider *n = (ofxUIIntSlider *) e.widget;
        updateHeight(n->getValue());
        
    }
    
    if(e.widget->getName() == "Remove") {
        ofxUILabelButton *n = (ofxUILabelButton *) e.widget;
        setRemove = true;
    }
    
    
    if(e.widget->getName() == "Connected") {
        ofxUIToggle *n = (ofxUIToggle *) e.widget;
        connected = false;
    }
    
}


void Client::update(string method) {
    
    if(!connected) enabled = false;
    
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
        } else if(method == "packed") { //TODO: break this up so we don't hit the size limit
            // Send enitre frame as one OSC message
            
            if( ofGetElapsedTimeMillis() - lastCmdTime > 1 ) {
                
            int oscpackets = ceil(colors.size() / 120);
            
            for(int p=0; p<colors.size(); p+=120) {
                
                ofxOscMessage m;
                m.setAddress("/p");
                m.addIntArg(p);
                
                int start   = p;
                int end     = start+120;
                
                for(int i = start; i < colors.size() && i<end; i++) {
                    m.addIntArg(colors[i].r);
                    m.addIntArg(colors[i].b); // yes this is reversed
                    m.addIntArg(colors[i].g);
                }
                
                osc->sendMessage(m);
                
            }
                lastCmdTime = ofGetElapsedTimeMillis();
            }

        } else if(method == "compressed") {
            // TODO: send a compressed frame over osc and decompress it on clients
            
            ofxOscMessage m;
            m.setAddress("/p");
            
            string cdata = "";
            
            for(int i = 0; i < colors.size(); i++) {
                cdata += colors[i].r;
                cdata += colors[i].b;
                cdata += colors[i].g;
            }
            
            m.addStringArg(cdata);
            
            osc->sendMessage(m);
            
            
        }
    }
     
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    if(selectedClientIndex == clients.size()) {
        selectedClientIndex = 0;
    }
    
    if (selectedClientIndex == -1) {
        selectedClientIndex = clients.size()-1;
    }
    selectedClient = clients[selectedClientIndex];

    vector<Client *>::iterator it;
    for(it = clients.begin() ; it != clients.end();)
    {
        if((*it)->setRemove) {
            //selectedClientIndex = 0;
            delete (*it);
            clients.erase(it);
        } else {
            it++;
        }
        
    }
    
    for(it = clients.begin() ; it != clients.end() ; ++it)
    {
        
        if((*it) == selectedClient) {
            (*it)->gui->setVisible(true);
        } else {
            (*it)->gui->setVisible(false);
        }
    }
    

    
    
    while(oscReceiver.hasWaitingMessages()){
        
		// get the next message
		ofxOscMessage m;
        
		oscReceiver.getNextMessage(&m);        
        if ( m.getAddress() == "/status" ){
            
            string label = m.getArgAsString(0);
            int status = m.getArgAsInt32(1);
            string message = m.getArgAsString(2);
            
            ofLogNotice("Got Status callback from: " + label + " status: " + ofToString(status) + "   " + message );
            
        } else if(m.getAddress() == "/hello") {
            
            int _cid = m.getArgAsInt32(0);
            
            ofLogNotice()<<"Client with id "<<_cid<<" and ip "<<m.getRemoteIp()<<" says hello.";
            
            handshakeClient(m.getRemoteIp(), _cid);
            createClientGui();
            
        }
    }
    
    
	for(int i=0; i<clients.size(); i++) {
        clients[i]->update(updateMethod);
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    inputHeight = round(inputHeight);
    inputWidth = round(inputWidth);
    
    int scale = 1;
    
    gui->setPosition(10, 10);
    
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
    ofTranslate(gui->getRect()->width + 40, 80);
    
    ofSetColor(255);
    ofNoFill();
    ofRect(0, 0, inputWidth*scale+2, inputHeight*scale+2);
    controlTexture.draw(1,1, inputWidth*scale, inputHeight*scale);
    

    font.drawString("Input " + syphonIn.getServerName() + "-" + syphonIn.getApplicationName()
                    + " [" + ofToString(syphonIn.getWidth()) + "x" + ofToString(syphonIn.getHeight())+"] "
                    + "Scaled to [" + ofToString(inputWidth) + "x" + ofToString(inputHeight) + "]", 0, -16);
    
    for(int i=0; i<clients.size(); i++) {
        
        ofNoFill();
        if(viewInfo) {
            ofRect(clients[i]->inputPos.x*scale, clients[i]->inputPos.y*scale, clients[i]->width*scale+1, clients[i]->height*scale);
        }
        ofFill();
        
        controlTexture.readToPixels(controlPixels);
        for(int p = 0; p < clients[i]->colors.size(); p++) {
            
            ofColor(0,0,0);
            
            if(clients[i]->inputPos.y+p > controlPixels.getHeight()) {
                clients[i]->colors[p] = ofColor(0,0,0);
            } else {
                clients[i]->colors[p] = controlPixels.getColor(clients[i]->inputPos.x, clients[i]->inputPos.y+p);
            }
        }
        
        if(clients[i]->testBlink) {
            for(int p = 0; p < clients[i]->height; p++) {
                clients[i]->colors[p].set(255,0,0);
            }
        }
    }
    
    ofPushMatrix();
    
    ofTranslate(inputWidth+ 40, 0);
    
    font.drawString("Output ", 0, -16);
    
    ofFill();
    if(monitorOutput) {    
        for(int i=0; i<clients.size(); i++) {
            
            // clients debug draw  - draw from input texture instead.
            for(int c=0; c<clients[i]->colors.size(); c++) {
                ofSetColor(clients[i]->colors[c]);
                ofRect(i*4+4, 4+c, 2, 1);
            }
        }
    }
    
    ofSetColor(255, 255, 255);
    
    ofPopMatrix();
    
    ofPopMatrix();
    
    ofPushMatrix();
    
    ofTranslate(10, gui->getRect()->height + 40);
    
    int connectedClients = 0;
    for(int i = 0; i<clients.size(); i++) {
        if (clients[i]->connected) connectedClients++;
    }

    int line = 0;
    
    font.drawString("Use arrow keys to select client. \nPress i to change syphon input.", 0, line);
    line+=font.getLineHeight()*3;
    font.drawString("Total clients: " + ofToString(clients.size()), 0, line);
    line+=font.getLineHeight()*1.5;
    

    font.drawString("Connected clients:  " + ofToString(connectedClients), 0, line);
    
    ofPopMatrix();
    /*for(int i=0; i<clients.size(); i++) {
        
        ofPushMatrix();
        ofTranslate(ofGetWidth()-200, 20+ i*90);
        
        ofDrawBitmapString(clients[i]->hostname, 0,0);
        ofDrawBitmapString("ID: " + ofToString(clients[i]->clientId), 0, 20);
        ofDrawBitmapString("Connected: " + ofToString(clients[i]->connected), 0, 40);
        ofDrawBitmapString("Enabled: " + ofToString(clients[i]->enabled), 0, 60);
        
        ofPopMatrix();
    }*/
}


void testApp::createClientGui() {
    
    //delete clientsGui;
    
    /*float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    clientsGui = new ofxUIScrollableCanvas(10,gui->getRect()->height+20,length+xInit*2.0,ofGetHeight()-(gui->getRect()->height+20));
    
    clientsGui->setFont("Gui/DINNextLTPro-Regular.ttf");
    clientsGui->setFontSize(OFX_UI_FONT_LARGE, 24);
    clientsGui->setColorFill(ofColor(255, 255, 255));
    
    clientsGui->setScrollAreaHeight(ofGetHeight()-(gui->getRect()->height+20));
    clientsGui->setScrollableDirections(false, true);
    
	//clientsGui->addWidgetDown(new ofxUILabel("Clients", OFX_UI_FONT_MEDIUM));
    
    
    for(int i=0; i<clients.size(); i++) {
        
        //clientsGui->addWidgetDown(new ofxUICanvas)
        clientsGui->addWidgetDown(new ofxUILabel("[" + ofToString(clients[i]->clientId) + "]  " + clients[i]->hostname, OFX_UI_FONT_SMALL));
        
        clientsGui->addSlider("Position x", 0, inputWidth, &clients[i]->inputPos.x);
        clientsGui->addSlider("Position y", 0, inputWidth, &clients[i]->inputPos.y);
        clientsGui->addIntSlider("Length", 1, 600, &clients[i]->height)->setID(clients[i]->clientId);
        clientsGui->addToggle("Enabled", &clients[i]->enabled);
        
        clientsGui->addToggle("Connected", clients[i]->connected);
        clientsGui->addSpacer(length, 1);
    }
    
    clientsGui->autoSizeToFitWidgets();
    ofAddListener(clientsGui->newGUIEvent,this,&testApp::guiEvent);*/
    
}



void testApp::saveSettings() {
    settings.clear();
    
    settings.setValue("lastId", lastId);
    settings.addTag("clients");
    settings.pushTag("clients");
    
    for(int i=0; i<clients.size(); i++) {
        
        settings.addTag("client");
        settings.pushTag("client", i);
        
        settings.setValue("clientId", clients[i]->clientId);
        settings.setValue("hostname", clients[i]->hostname);
        settings.setValue("label", clients[i]->label);
        
        settings.setValue("inputPosX", clients[i]->inputPos.x);
        settings.setValue("inputPosY", clients[i]->inputPos.y);
        
        settings.popTag();
        
    }
    
    settings.popTag();
    settings.save("settings.xml");
    
}


void testApp::exit(){
    
    saveSettings();
    gui->saveSettings("GUI/guiSettings.xml");
    
    delete gui;
    
}

//these are our directory's callbacks
void testApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    
    syphonInputs.clear();
    for(int i =0; i<directory.getServerList().size(); i++) {
        syphonInputs.push_back( directory.getServerList()[i].serverName + directory.getServerList()[i].appName);
    }
    
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void testApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void testApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'i') {
        dirIdx++;
        if(dirIdx > directory.size() - 1)
            dirIdx = 0;
        
        if(directory.isValidIndex(dirIdx)){
            syphonIn.setServerName(directory.getServerList()[dirIdx].serverName);
            syphonIn.setApplicationName(directory.getServerList()[dirIdx].appName);
        }
    }
    
    if(key == OF_KEY_LEFT) {
        selectedClientIndex--;
    } else if(key == OF_KEY_RIGHT) {
        selectedClientIndex++;
        
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


