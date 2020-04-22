#pragma once

#include "ofMain.h"

#include "ofxStepSequencer.h"

//uncomment if you added this addon to handle window settings
//#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

//TODO:
//adding variable grid size (notes & steps) feature
#define NUM_SEQ_NOTES 12//max & fixed size
#define NUM_SEQ_BEATS 16//max & fixed size

class ofApp : public ofBaseApp {
    
public:

#ifdef USE_ofxWindowApp
	ofxWindowApp windowApp;
#endif

    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);

	//--

    ofxStepSequencer SEQ;
	void SEQ_Gui_Customize();

    //--

    ofxGui gui_Panel_Notes;
    ofxGuiContainer* gui_Notes;

    //--

    //local receivers in ofApp, bool params to receive states triggered from the running sequencer
    ofParameter<bool> notes_params[NUM_SEQ_NOTES];

    //CALLBACKS MODE A
    void Changed_SEQ_NOTES(ofAbstractParameter& e);
    //callback listener for inside class ofxStepSequencer
	//this callback is required to receive the sequencer states when playing, 
	//and to receive to the target bool params

    //CALLBACKS MODE B
    //ofParameterGroup notes_paramsGroup;
    //void Changed_notes_paramsGroup(ofAbstractParameter &e);
	//this callback should be required if we want to control target params from user mouse clicks too,
	//not only from sequencer addon when playing.
	//if you don't add this callback, the user clicks to the target notes will not be catched.

	//-
};
