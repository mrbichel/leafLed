#include "testApp.h"

const int numClients = 1;

const int inputWidth = 202;
const int inputHeight = 260;

const string updateMethod = "raw";

void testApp::setup(){
    
    ofEnableAlphaBlending();
    ofSetFrameRate(30);
    
    // TODO add gui for configuring nodes
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	//ofSetFrameRate(110);
    
    // syphon input
    
    syphonIn.setup();
    //syphonIn.setApplicationName("MadMapper");
    syphonIn.setApplicationName("Modul8");
    syphonIn.setServerName("");
        
    fboIn.allocate(inputWidth, inputHeight);
    controlTexture.allocate(inputWidth, inputHeight, GL_RGB);
    // Setup all clients
    
    for(int i=0; i<numClients; i++) {
        addClient();
    }
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    gui = new ofxUIScrollableCanvas(0,0,length+xInit*2.0,ofGetHeight());
    
    gui->setScrollAreaToScreen();
    gui->setScrollableDirections(false, true);
    
	gui->addWidgetDown(new ofxUILabel("LeafLED", OFX_UI_FONT_LARGE));
    
    gui->addSpacer(length, 2);
    //gui->addWidgetDown(new ofxUILabel("Settings", OFX_UI_FONT_MEDIUM));
    //gui->addWidgetDown(new ofxUILabel("Update method", OFX_UI_FONT_MEDIUM));
    //gui->addWidgetDown(new ofxUILabel("Performance", OFX_UI_FONT_MEDIUM));
    
    
    gui->addWidgetDown(new ofxUILabel("Leafs - Client config", OFX_UI_FONT_MEDIUM));
    gui->addSpacer(length, 2);    
    
    gui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    for(int i=0; i<numClients; i++) {
        
        gui->addWidgetDown(new ofxUILabel("Leaf: " + ofToString(i), OFX_UI_FONT_SMALL));
        gui->addTextInput("hostname" + ofToString(i), clients[i].hostname);
        
        //gui->add2DPad("Input position", ofVec3f(0, inputWidth), ofVec3f(0, inputHeight), clients[i].inputPos);
    }
    
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    //gui->loadSettings("GUI/guiSettings.xml");
    
    
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	
    cout << "event from " << name << endl;
    
	/*if(name == "RED")
     {
     ofxUISlider *slider = (ofxUISlider *) e.widget;
     red = slider->getScaledValue();
     }
     else if(name == "GREEN")
     {
     ofxUISlider *slider = (ofxUISlider *) e.widget;
     green = slider->getScaledValue();
     }*/
    
    if(name.find("hostname" > 0))
    {
        //  name.
        
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: ";
            //            ofUnregisterKeyEvents((testApp*)this);
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
            //            ofRegisterKeyEvents(this);
        }
        string output = textinput->getTextString();
        cout << output << endl;
    }
    
}


void testApp::addClient() {
    
    int index = clients.size();
    
    Client c;
    
    c.fboOut.allocate(c.width, c.height);
    c.texture.allocate(c.width, c.height, GL_RGB);
    c.colors.assign(c.height, ofColor(255));
    
    //c.hostname = "leaf"+ofToString(i)+".local";
    
    c.osc = new ofxOscSender();
    c.osc->setup(c.hostname, c.port);
    clients.push_back(c);
        
    clients[index].inputPos.set(1+ index*5, 20);
    
}

void Client::update() {
    
    
    // TODO output data from framebuffer texture
    
    //m.setRemoteEndpoint(hostname, port);    
    
    texture.readToPixels(pixels);
    
    // Performance notes
    // raw pack and send: 16 FPS
    // Only pack: 36 FPS
    // Only update pixelsArray: 70 - 100 FPS
    // Conclusion: send fewer OSC packages
    
    // Packed
    // 50 - 80 FPS
    
    if(updateMethod == "raw") {
        // Send each pixel as a OSC message
        
        
        
        for(int i = 0; i < height; i++) {
            
            ofxOscMessage m;
            m.setAddress("/l");
            
            
            
            colors[i] = pixels.getColor(1, i);
            
            m.addIntArg(i);
            
            m.addIntArg(colors[i].r);
            m.addIntArg(colors[i].b); // yes this is reversed
            m.addIntArg(colors[i].g);
            
            //cout<<"C color r: "<<ofToString(c.r)<<endl;
            
            osc->sendMessage(m);
            
        }
    } else if(updateMethod == "packed") { // does this break a size limit
        // Send enitre frame as one OSC message
        
        ofxOscMessage m;
        m.setAddress("/p");
        
        for(int i = 0; i < pixels.size(); i++) {
            m.addFloatArg(pixels[i]);
        }
        
        osc->sendMessage(m);
        
    } else if(updateMethod == "compressed") {
        // TODO: send a compressed frame over osc and decompress it on clients
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
	for(int i=0; i<clients.size(); i++) {

       if(ofGetFrameNum() % 2 == 1) clients[i].update();
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255);
    ofBackground(30);
    
    fboIn.begin();
    
    ofBackground(0);
    
    syphonIn.draw(0, 0, fboIn.getWidth(), fboIn.getHeight());
    
    fboIn.end();
    
    controlTexture = fboIn.getTextureReference();
    
    ofPushMatrix();
    ofTranslate(400, 20);
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
        controlTexture.drawSubsection(0, 0, clients[i].width, clients[i].height,
                                      clients[i].inputPos.x, clients[i].inputPos.y);
        
        clients[i].fboOut.end();
        clients[i].texture = clients[i].fboOut.getTextureReference();
    }
    
    ofPopMatrix();
    
    
    ofPushMatrix();
    ofTranslate(620, 20);
    ofNoFill();
    ofRect(0,0,10*clients.size()+4, 404);
    ofFill();
    
    for(int i=0; i<clients.size(); i++) {
        clients[i].texture.draw(i*10+4,2,6,400);
    }
    
    
    for(int i=0; i<clients.size(); i++) {
        
        
        for(int c=0; c<clients[i].colors.size(); c++) {
            
            ofSetColor(clients[i].colors[c]);
            
            ofRect(i*10+4, 10+400+c, 6, 1);
        }
        
    }
    
    
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(400, 500);
    ofSetColor(255);
    ofDrawBitmapString("LED Server", 10, 20);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 10, 60);
    
    ofPopMatrix();
    
}

void testApp::exit(){
    
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    
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


