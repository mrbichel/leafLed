#include "testApp.h"

const int numClients = 20;

const int inputWidth = 202;
const int inputHeight = 260;

const string updateMethod = "packed";

void testApp::setup(){
    
    // TODO add gui for configuring nodes
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	//ofSetFrameRate(110);
    
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
    
    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
	
    gui = new ofxUICanvas(20,20,length+xInit*2.0,ofGetHeight());
	gui->addWidgetDown(new ofxUILabel("LeafLED", OFX_UI_FONT_LARGE));
    
    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUILabel("Leafs", OFX_UI_FONT_MEDIUM));
    /*gui->addSlider("BGR", 0, 255, backgroundColor.r, 95, dim);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addSlider("BGG", 0, 255, backgroundColor.g, 95, dim);
    gui->addSlider("BGB", 0, 255, backgroundColor.b, 95, dim);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addSpacer(length, 2);
    
    gui->addWidgetDown(new ofxUILabel("CIRCLE CONTROL", OFX_UI_FONT_MEDIUM));
    gui->addSlider("RED", 0.0, 255.0, red, length,dim);
	gui->addSlider("GREEN", 0.0, 255.0, green, length,dim);
    gui->addSlider("BLUE", 0.0, 255.0, blue, length,dim);
    gui->addSlider("ALPHA", 0.0, 255.0, alpha, length,dim);
    gui->addSlider("RADIUS", 0.0, 600.0, radius, length,dim);
	gui->addSlider("RESOLUTION", 3, 60, resolution, length,dim);
    
    gui->addWidgetDown(new ofxUILabelToggle(drawFill, "DRAW FILL", OFX_UI_FONT_MEDIUM));
    
    float padWidth = length;
    float padHeight = length*((float)ofGetHeight()/(float)ofGetWidth());
    
    gui->addWidgetDown(new ofxUI2DPad(padWidth, padHeight, ofPoint(padWidth*.5, padHeight*.5), "POSITION"));
    
    gui->addSpacer(length, 2);
    gui->addWidgetDown(new ofxUILabel("HIDE & SHOW GUI BY PRESSING 'g'", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabel("MOUSE OVER A SLIDER AND", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUILabel("PRESS UP, DOWN, LEFT, RIGHT", OFX_UI_FONT_MEDIUM));
    */
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
    
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
    
    
    // TODO output data from framebuffer texture
    
    //m.setRemoteEndpoint(hostname, port);    
    
    texture.readToPixels(data);
    
    // Performance notes
    // raw pack and send: 16 FPS
    // Only pack: 36 FPS
    // Only update pixelsArray: 70 - 100 FPS
    // Conclusion: send fewer OSC packages
    
    // Packed
    // 50 - 80 FPS
    
    if(updateMethod == "raw") {
        // Send each pixel as a OSC message
        for(int i = 0; i < data.size()/3; i++) {
            
            ofxOscMessage m;
            m.setAddress("/l");
            
            m.addIntArg(i);
            m.addFloatArg(data[i]);
            m.addFloatArg(data[i+1]);
            m.addFloatArg(data[i+2]);
            
            osc->sendMessage(m);
            
        }
    } else if(updateMethod == "packed") { // does this break a size limit
        // Send enitre frame as one OSC message
        
        ofxOscMessage m;
        m.setAddress("/p");
        
        for(int i = 0; i < data.size(); i++) {
            m.addFloatArg(data[i]);
        }
        
        osc->sendMessage(m);
        
    } else if(updateMethod == "compressed") {
        // TODO: send a compressed frame over osc and decompress it on clients
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
	for(int i=0; i<clients.size(); i++) {

        // Send new frame
       //if(ofGetFrameNum() % 2 == 1)
           
        clients[i].update();
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
   
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
    
    ofPopMatrix();
    
    
    ofPushMatrix();
    ofTranslate(400, 500);
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
    switch (key)
    {
        case 'g':
        {
            gui->toggleVisible();
        }
            break;
        default:
            break;
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

void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	
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

}
