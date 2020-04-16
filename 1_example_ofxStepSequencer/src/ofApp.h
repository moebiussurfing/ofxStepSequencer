#pragma once

#include "ofMain.h"

#include "ofxStepSequencer.h"

//uncomment if you added this addon to handle window settings
#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

//TODO: allow change this grid sizes..
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

    //local notes_params to trig bang/bools trigged from sequencer
    ofParameter<bool> notes_params[NUM_SEQ_NOTES];

    //callback listener for inside class ofxStepSequencer
    //CALLBACKS MODE A
    void Changed_SEQ_NOTES(ofAbstractParameter& e);

    //CALLBACKS MODE B
    //ofParameterGroup notes_paramsGroup;
    //void Changed_notes_paramsGroup(ofAbstractParameter &e);

	//-
};
