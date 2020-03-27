#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetBackgroundColor(48);
	ofSetLogLevel("ofApp", OF_LOG_NOTICE);

#ifdef USE_ofxWindowApp
	//windowApp.setSettingsFps(60);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif

	//------------------------------------------------------

	//STEP SEQUENCER
	SEQ.setup();
	SEQ_Gui_Customize();

	//------------------------------------------------------

	//LOCAL TARGET NOTES PARAMS (toggle bools) & GUI MONITOR

	//customize
	ofJson confg_Cont =
	{
		{"width", 40}
	};
	ofJson confg_Button =
	{
		{"type", "fullsize"},
		{"text-align", "right"},
		{"height", 21},
		//{"width", 100},
		//{"text-align", "center"},
	};

	gui_Notes = gui_Panel_Notes.addContainer();
	gui_Notes->setName("LOCAL NOTES");
	gui_Notes->setConfig(confg_Cont);

	//notes_paramsGroup.setName("NOTES PARAMS");//CALLBACKS MODE B

	//--

	//ofApp local toggles
	for (int col = 0; col < NUM_SEQ_NOTES; col++)
	{
		string nStr = ofToString(col + 1);
		notes_params[col].set(nStr, false);
		//notes_paramsGroup.add(notes_params[col]);//CALLBACKS MODE B
		gui_Notes->add<ofxGuiToggle>(notes_params[col]);

		//customize
		gui_Notes->getControl(nStr)->setConfig(confg_Button);
	}

	gui_Notes->setPosition(880, 10);
	gui_Notes->loadTheme("theme/theme_bleurgh.json");

	//--

	//CALLBACKS MODE A:
	//listener to directly check inside class ofxStepSequencer
	ofAddListener(SEQ.TARGET_NOTES_paramsGroup.parameterChangedE(), this, &ofApp::Changed_SEQ_NOTES);

	//--

	////CALLBACKS MODE B:
	////listener for local ofApp toggle param variables
	//ofAddListener(notes_paramsGroup.parameterChangedE(), this, &ofApp::Changed_notes_paramsGroup);

	//--
}

//--------------------------------------------------------------
void ofApp::SEQ_Gui_Customize()
{
	//optional customize layout

	//THEME
	int grid_w = 400;
	SEQ.setPosition_GridSequencer(465, 10, grid_w, 0);
	SEQ.setPosition_Gui(220, 5);

	//BEAT CLOCK TRANSPORT
	int squares_w = 200;
	int ball_radius = 30;
	int px = 5;
	int py = 5;
	SEQ.setPosition_CLOCK_Gui(px, py, squares_w);
	SEQ.setPosition_CLOCK_Squares(px, py + 600, squares_w);//beat squares
	SEQ.setPosition_CLOCK_Ball(px + squares_w * 0.5 - ball_radius, py + 800, ball_radius);//beat ball

	//PRESET MANAGER
	//SEQ.presetsManager.set_pathKit_Folder("patterns/kit_1");//startup buggy..
	SEQ.presetsManager.set_GUI_position(465, 350 + grid_w / 9 + 10);//default
	SEQ.presetsManager.setPosition_ClickerPanel(465, 350, grid_w / 9);//default
	SEQ.presetsManager.setVisible_ClickerPanel(true);//default
	SEQ.presetsManager.setVisible_Gui(true);//default

	//-

	//load last GRID settings
	//path = "SEQ_default.json";
	//SEQ.load_JSON(path);
}

//--------------------------------------------------------------
void ofApp::update()
{
	SEQ.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
	SEQ.draw();
	SEQ.draw_CLOCK();
}

//--------------------------------------------------------------
void ofApp::exit()
{
	//save current preset steps
	//SEQ.save_JSON(path);

	SEQ.exit();
	ofRemoveListener(SEQ.TARGET_NOTES_paramsGroup.parameterChangedE(), this, &ofApp::Changed_SEQ_NOTES);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch (key)
	{

	case ' ':
	{
		SEQ.togglePlayStop();
	}
	break;

	case ',':
		SEQ.randomize();
		break;

	case '.':
		SEQ.clear();
		break;

	case OF_KEY_LEFT:
		SEQ.sequencer.stepBack();
		break;

	case OF_KEY_RIGHT:
		SEQ.sequencer.advance();
		break;

#ifdef USE_OFXBEATCLOCK
	case 't':
		SEQ.beatClock.Tap_Trig();
		break;
#endif

	default:
		break;
	}
}

//--------------------------------------------------------------
//CALLBACKS MODE A:
//listener to check inside class ofxStepSequencer
void ofApp::Changed_SEQ_NOTES(ofAbstractParameter& e)
{
	string name = e.getName();
	int noteNum = ofToInt(name);
	bool state = false;

	if (e.type() == typeid(ofParameter<bool>).name())
	{
		ofParameter<bool> p = e.cast<bool>();
		state = p;
		ofLogVerbose("ofApp") << "Changed_SEQ_NOTES: " << name << (p ? " ON" : " OFF");

		notes_params[noteNum] = state;

		//-

		//here you can trig midi notes_params i.e.
		//pd.sendNoteOn(midiChan, noteFirst + noteNum, (int)state?127:0);
	}
}

////--------------------------------------------------------------
////CALLBACKS MODE B:
////listener for local ofApp toggle param variables
//void ofApp::Changed_notes_paramsGroup(ofAbstractParameter &e)
//{
//	string name = e.getName();
//	int noteNum = ofToInt(name);
//	bool state = false;
//
//	if (e.type() == typeid(ofParameter<bool>).name())
//	{
//		ofParameter<bool> p = e.cast<bool>();
//		state = p;
//		ofLogVerbose("ofApp") << "Changed_notes_paramsGroup " << name << (p ? " ON" : " OFF");
//	}
//}
