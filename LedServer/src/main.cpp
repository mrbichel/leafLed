#include "ofMain.h"
#include "testApp.h"

#include "testApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1200,1000,OF_WINDOW);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());
	
}
