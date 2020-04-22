#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetBackgroundColor(48);
	ofSetLogLevel("ofApp", OF_LOG_NOTICE);

#ifdef USE_ofxWindowApp
	windowApp.setSettingsFps(60);//onlty required on first use, before xml is present
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

	//optional customize
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

	//ofApp local params. bool/toggles target receivers from sequencer
	for (int col = 0; col < NUM_SEQ_NOTES; col++)
	{
		//define param
		string nStr = ofToString(col + 1);
		notes_params[col].set(nStr, false);

		//add param to gui panel
		gui_Notes->add<ofxGuiToggle>(notes_params[col]);
		//notes_paramsGroup.add(notes_params[col]);//CALLBACKS MODE B

		//customize added button
		gui_Notes->getControl(nStr)->setConfig(confg_Button);
	}

	//layout and theme
	gui_Notes->setPosition(880, 10);
	gui_Notes->loadTheme("theme/theme_bleurgh.json");

	//--

	//CALLBACKS MODE A:
	//listener to directly check inside class ofxStepSequencer and receive states
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
	SEQ.setPosition_CLOCK_Gui(px, py, squares_w);//main gui. titled as 'ofxStepSequencer'
	SEQ.setPosition_CLOCK_Squares(px, py + 630, squares_w);//beat squares
	SEQ.setPosition_CLOCK_Ball(px + squares_w * 0.5 - ball_radius, py + 830, ball_radius);//beat ball for visual feedback

	//PRESET MANAGER
	SEQ.presetsManager.set_GUI_position(465, 350 + grid_w / 9 + 10);//gui titled as 'SEQ PATTERNS'
	SEQ.presetsManager.setPosition_ClickerPanel(465, 350, grid_w / 9);//mouse clicker preset selector
	SEQ.presetsManager.setVisible_ClickerPanel(true);//show
	SEQ.presetsManager.setVisible_Gui(true);//show

	//-

	//TODO: customize data folder. startup buggy..
	//SEQ.presetsManager.set_pathKit_Folder("patterns/kit_1");

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
	SEQ.draw();//grid sequencer
	SEQ.draw_CLOCK();//visual feedback: beat ball, beat squares and text info
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

		//transport control
	case ' ':
	{
		SEQ.togglePlayStop();
	}
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

	//helpers
	case ',':
		SEQ.randomize();
		break;

	case '.':
		SEQ.clear();
		break;

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
