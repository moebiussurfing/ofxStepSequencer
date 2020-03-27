///TODO:

///+ maybe copy presetManager system with dataGrid class?

///+ shift steps left rigth up down

///+ ofxBeatClock
///+ add callback to play toggle linking
///+ group all bpm's into one
///+ add buffer clock besides timer
///+ re sync trig

///+ monitor too triggered notes from outside (OSC/API) to the grid cells
///+ helpers to set widgets/panels layout

//----------------------------------------------------------

#pragma once

#include "ofMain.h"

#include "ofxGuiExtended2.h"
#include "ofxJsonUtils.h"
#include "ofxSequencer.h"

#define USE_OFXBEATCLOCK
//comment to disable and use internal ofxSequencer / ofxBpm clock tick timer instead of ofxBeatClock
//default mode (uncommented) will use ofxBeatClock for clock tick timer

#ifdef USE_OFXBEATCLOCK
#include "ofxBeatClock.h"
#endif

#define NUM_SEQ_NOTES 12//max & fixed size
#define NUM_SEQ_BEATS 16//max & fixed size

#define BPM_INIT 120
#define BARS_INIT 4

#ifndef USE_OFXBEATCLOCK
#define USE_OFXSEQUENCER_TRANSPORT
//to use ofxSequencer clock by ofxBPM
//comment to use my own ofxBeatClock clock system. This one is called INTERNAL and based in timers too, so it could drop and be inestable sometimes. Also it has MIDI IN CLOCK mode as EXTERNAL clock.
#endif

#include "DataGrid.h"
#include "ofxPresetDataGrid.h"

//-

class ofxStepSequencer
{

public:

	//debug cols and transport
	bool DEBUG_state = false;

	ofxStepSequencer() {}
	~ofxStepSequencer() {}

	//-

	void setup();
	void update();
	void draw();
	void exit();

	void setup_gui();
	void draw_CLOCK();

	//-

	//API

public:

	void setPosition_GridSequencer(int x, int y, int width, int heigh);
	void setPosition_Gui(int x, int y);

#ifdef USE_OFXBEATCLOCK
	void setPosition_CLOCK_Gui(int x, int y, int w);
	void setPosition_CLOCK_Squares(int x, int y, int w);
	void setPosition_CLOCK_Ball(int x, int y, int w);
	//bool bBallAutoPos = true;
	//void setPosition_CLOCK_Ball_Auto(bool b)
	//{
	//	bBallAutoPos = b;
	//}
#endif

	bool getPlayState();

	void setPlayState(bool state);
	void togglePlayStop();
	
	void randomize();
	void clear();

private:
	void random(int noteFrom, int numNotes, float prob);//num TARGET_NOTES_params, 2 = 50%
	void cloneFromBar(int selBar);
	bool SHOW_Gui = true;

public:
	void set_SHOW_Gui(bool state);

	//apply settings for font in all gui panels together
	void set_GuiAll_Settings(ofJson js);

	//-

	//SEQUENCER

public:

	ofxSequencer sequencer;

	//-

	//TARGET (NOTES TRIGGERS) OF SEQUENCER DATA

public:

	ofParameter<bool> TARGET_NOTES_params[NUM_SEQ_NOTES];//state of each note/row
	//TODO: BUG: add step 0
	//ofParameter<bool> TARGET_NOTES_params[NUM_SEQ_NOTES+1];//state of each note/row

	ofParameterGroup TARGET_NOTES_paramsGroup;

	//TODO: change only bool mode to int or floats too
	//ofParameter<bool> step0_row[NUM_SEQ_NOTES];//state of each note/row

	//-

	//method to use from outside
	void setGRID_square(int note, int beat, bool state);//to the store array

	//-

	//get values to GRID from sequencer class, like clicked cells, (not only from randomized or cloned by ofxStepSequencer, that they are alreay stored)
	void GRID_getFrom_Sequencer();

	//TODO: make functionm to refresh when listens preset manager changes..
	//void GRID_set_Sequencer_From_DATA_class();

	//--

private:

	bool DISABLE_CALLBACKS = false;

	string path_Global = "ofxStepSequencer";
	string path_Settings = "settings.xml";

	ofParameter<bool> SHOW_beatClock;
	ofParameter<bool> SHOW_presetsManager;
	ofParameter<bool> SHOW_Grid;

	//-

private:

	//JSON settings:

	struct JSON_grid_Row //one row note
	{
		//init NUM_SEQ_BEATS = 16
		//TODO: allow variable lenghts row/col
		//TODO: allow ofxJsonUtils mode... easy parsers
		std::vector <int> steps{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	};

	void save_JSON(string path);
	void load_JSON(string path);

	bool ENABLE_autoSaveLoad = false;//disable auto save load
	string path_def;

	//----

	//to trig from inside the class
	//void Changed_SEQ_NOTES(ofAbstractParameter& e);

	//-

	//GUI AND PARAMETERS

	ofxGui gui_SEQ;

	ofxGuiGroup2* group_HelperTools;
	ofParameterGroup params_HelperTools;
	ofParameter<bool> random_trig;
	ofParameter<float> randomProb;
	ofParameter<int> noteFrom;
	ofParameter<int> numNotes;
	ofParameter<bool> clear_trig;
	ofParameter<bool> clone_trig;
	ofParameter<int> clone_fromBar;

	ofxGuiGroup2* group_Transport;
	ofParameterGroup params_Transport;
	ofParameter<bool> isPlaying = false;
	ofParameter<bool> play_trig;
	ofParameter<float> bpm;

	void Changed_Params(ofAbstractParameter& e);

	//-

	//GUI & GRID THEME FONT

	string myTTF;//gui font path
	int sizeTTF;
	ofTrueTypeFont myFont;
	int fontSize;

	//-

	//DEBUG FONT

	int ttfSize;
	ofTrueTypeFont ttfType;

	//-

	//SEQUENCER GRID POSITION AND SIZE:

	int seqW;
	int seqH;
	int seqX;
	int seqY;
	int seqBoxSize;

	//-----

	//TODO: fbo to improve redraw performance..
	//TODO: maybe should mix both SEQ and ofxSequencer together, drawings too

	ofFbo fbo;
	bool toRedraw;
	void redraw();
	bool useFbo;
	//ofFbo fboNotes;

	//--

	//BPM CLOCK

#ifdef USE_OFXBEATCLOCK
public:
	ofxBeatClock beatClock;
	void Changed_BPM_beat_current(int & value);
#endif

private:
	//choice between internal ofxSequencer clock vs ofxBeatClock clocker
	bool ENABLE_ofxBeatClock_MODE;

	//--

public:
	ofxPresetDataGrid presetsManager;

	//--

public:
	//TODO: 
	//public callbacks. easy listeners
	void Changed_DONE_load(bool & DONE_load);
	void Changed_DONE_save(bool & DONE_save);

	//--

private:
	//DATA custom class
	DataGrid myDataGrid;

	//--
};
