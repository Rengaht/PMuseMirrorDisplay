#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
//	ofSetupOpenGL(1280,720,OF_WINDOW);			// <-------- setup the GL context

	ofGLESWindowSettings settings;
	settings.glesVersion=2;
	ofCreateWindow(settings);	

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
