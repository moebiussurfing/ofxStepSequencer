#include "ofxStepSequencer.h"

//TODO:
//adding variable gird size feature

//--------------------------------------------------------------
void ofxStepSequencer::setup(int _NUM_SEQ_BEATS, int _NUM_SEQ_NOTES)
{
	ofSetLogLevel("ofxStepSequencer", OF_LOG_NOTICE);
	ofSetLogLevel("ofxSequencer", OF_LOG_NOTICE);

	//-

#ifdef USE_OFXBEATCLOCK
	ENABLE_ofxBeatClock_MODE = true;
#else
	ENABLE_ofxBeatClock_MODE = false;
#endif

	//-

	//1. SEQUENCER CLASS

	//ofxSequencer object

	sequencer.setup(_NUM_SEQ_BEATS, BPM_INIT, BARS_INIT);

	//TODO: BUG: add step 0

	//--

	//2. SEQ GRID

	TARGET_NOTES_paramsGroup.setName("sequencer TARGET_NOTES_params");

	for (int row = 0; row < _NUM_SEQ_NOTES; row++)
	{
		//target vars
		TARGET_NOTES_params[row].set(ofToString(row), false);
		TARGET_NOTES_paramsGroup.add(TARGET_NOTES_params[row]);

		//-

		//grid sequencer
		sequencer.addRow(&TARGET_NOTES_params[row]);
	}

	//TODO: BUG: add step 0
	//sequencer.addRow(& TARGET_NOTES_params[NUM_SEQ_NOTES] );

	//to trig from inside the class
	//ofAddListener(TARGET_NOTES_paramsGroup.parameterChangedE(), this, &ofxStepSequencer::Changed_SEQ_NOTES);

	//--

	clear();//erase all grid vector and ofxSequencer data grid

	//-

	//default grid position and sizes (if setPosition not called..)

	seqH = 200;
	seqBoxSize = seqH / _NUM_SEQ_NOTES;
	seqW = seqBoxSize * _NUM_SEQ_BEATS;
	seqX = 550;
	seqY = 500;
	sequencer.setPosition(seqX, seqY, seqW, seqH);

	//-

	//GUI

	setup_gui();

	//-

	//FBO IMPROVE DRAW PERFORMANCE

	//TODO:
	//do the draw on refresh only code..
	useFbo = false;
	fbo.allocate(seqW, seqH);
	fbo.begin();
	ofClear(0, 0);
	fbo.end();
	//toRedraw = true;
	toRedraw = useFbo;

	//-

	//GRID NUMBERS FONT DRAW

	string f1;
	fontSize = 8;
	f1 = path_Global + "/fonts/telegrama_render.otf";

	myFont.load(f1, fontSize, true, true);
	myFont.setLineHeight(fontSize);

	//-

	//DEBUG FONT

	string f2;
	ttfSize = 7;
	f2 = path_Global + "/fonts/telegrama_render.otf";
	ttfType.load(f2, ttfSize, true, true);
	ttfType.setLineHeight(ttfSize);

	//--

	//TODO:
	//GRID states auto save load
	if (ENABLE_autoSaveLoad)
	{
		string path_def = "SEQ_defaultPattern.json";
		load_JSON(path_def);
	}

	//--

#ifdef USE_OFXBEATCLOCK

	//BPM CLOCKER

	beatClock.setup();

	//-

	//DEFAULT POSITIONS
	//int squares_w = 200;
	//int ball_radius = 30;
	//int px = 5;
	//int py= 5;
	//setPosition_CLOCK_Gui( px, py, squares_w ) ;
	//setPosition_CLOCK_Squares( px, py + 605, squares_w);
	//setPosition_CLOCK_Ball(px + squares_w * 0.5 - ball_radius, py + 760, ball_radius);

	//-

	//get beat ticks events from outside the class
	beatClock.BPM_beat_current.addListener(this, &ofxStepSequencer::Changed_BPM_beat_current);

	//check changes on beatClock bpm
	beatClock.BPM_Global.addListener(this, &ofxStepSequencer::Changed_BeatClock_BPM);

#endif

	//--

	//PRESET MANAGER

	//GUI

	presetsManager.setup_Gui();
	presetsManager.set_pathKit_Folder("ofxStepSequencer/presets");
	myDataGrid.setName("stepSequencer");
	presetsManager.add(myDataGrid, { '1', '2', '3', '4', '5', '6', '7', '8' });

	//-

	//TODO: easy listener temp solution to trig when save/load is done
	//then will load refresh grid sequencer states

	presetsManager.DONE_save.addListener(this, &ofxStepSequencer::Changed_DONE_save);
	presetsManager.DONE_load.addListener(this, &ofxStepSequencer::Changed_DONE_load);

	//-

	//STARTUP INIT

	presetsManager.load_ControlSettings();//this loads selected preset number and gui state

	//-

	//trick to solve auto load fail because the sorting of xml autoSave after preset selector tag
	presetsManager.PRESET_selected_PRE = -1;
	ofLogNotice("ofxStepSequencer") << "PRESET_selected:" << presetsManager.PRESET_selected;
	presetsManager.PRESET_selected = presetsManager.PRESET_selected;

	//---

	//DEFAULT STARUP SETTINGS

	presetsManager.set_GUI_position(450, 450);//default
	presetsManager.setPosition_ClickerPanel(450, 550, 40);//default
	presetsManager.setVisible_ClickerPanel(true);//default
	presetsManager.setVisible_Gui(true);//default

	//presetsManager.setDelayedLoading(true);

	//--

	//load settings
	ofXml settings;
	settings.load(path_Global + "/" + path_Settings);
	ofDeserialize(settings, params_Transport);

	//--
}

//--------------------------------------------------------------
void ofxStepSequencer::setup()
{
	setup((int)NUM_SEQ_BEATS, (int)NUM_SEQ_NOTES);
}

//---------------------------
void ofxStepSequencer::set_SHOW_Gui(bool state)
{
	SHOW_Gui = state;
	gui_SEQ.getVisible().set(SHOW_Gui);

	presetsManager.setVisible_Gui(SHOW_Gui);
}

//---------------------------
void ofxStepSequencer::setup_gui()
{
	//-

	//THEME

	ofJson confg_Sliders =
	{
		{"height", 40}
	};
	ofJson confg_BigBut =
	{
		{"type", "fullsize"},
		{"text-align", "center"},
		{"height", 40},
	};
	//ofJson conf_Cont =
	//{
	//	//{"direction", "vertical"},
	//};
	//ofJson confg_Sliders =
	//{
	//	//{"show-header", false},
	//	//{"height", 18},
	//};

	//-

	//SEQUENCER CONTROLLER:

	//1. control

	params_HelperTools.setName("SEQ HELPER TOOLS");
	params_HelperTools.add(random_trig.set("RANDOM NOTES", false));
	params_HelperTools.add(noteFrom.set("FROM", 0, 0, NUM_SEQ_NOTES - 1));
	params_HelperTools.add(numNotes.set("MAX AMOUNT", NUM_SEQ_NOTES, 1, NUM_SEQ_NOTES));
	params_HelperTools.add(randomProb.set("PROB", 0.1f, 0, 10));
	params_HelperTools.add(clone_trig.set("CLONE BARS", false));
	params_HelperTools.add(clone_fromBar.set("FROM BAR", 1, 1, 4));
	params_HelperTools.add(clear_trig.set("CLEAR ALL", false));

	group_HelperTools = gui_SEQ.addGroup(params_HelperTools);

	//custom settings
	group_HelperTools->minimize();
	//group_HelperTools->setConfig(conf_Cont);
	//(group_HelperTools->getIntSlider("FROM"))->setConfig(confg_Sliders);
	//(group_HelperTools->getIntSlider("MAX AMOUNT"))->setConfig(confg_Sliders);
	//(group_HelperTools->getFloatSlider("PROB"))->setConfig(confg_Sliders);
	(group_HelperTools->getFloatSlider("PROB"))->setPrecision(1);
	//(group_HelperTools->getIntSlider("FROM BAR"))->setConfig(confg_Sliders);

	//-

	//2. transport

	params_Transport.setName("ofxStepSequencer");
	params_Transport.add(play_trig.set("PLAY", false));
	params_Transport.add(bpm.set("BPM", 120.0f, 30.0f, 400.0f));
	params_Transport.add(loopBar.set("LOOP BAR", false));
	params_Transport.add(startBar.set("startBar", 1, 1, 4));
	params_Transport.add(numBars.set("numBars", 4, 1, 4));
	params_Transport.add(SHOW_Grid.set("SHOW GRID", true));
	params_Transport.add(SHOW_beatClock.set("SHOW BEAT-CLOCK", true));
	params_Transport.add(SHOW_presetsManager.set("SHOW PRESETS", true));

	play_trig.setSerializable(false);

	//#ifdef USE_OFXSEQUENCER_TRANSPORT
	group_Transport = gui_SEQ.addGroup(params_Transport);
	//group_Transport->setConfig(conf_Cont);//disable custom individual settings..
	(group_Transport->getToggle("PLAY"))->setConfig(confg_BigBut);
	(group_Transport->getFloatSlider("BPM"))->setPrecision(2);
	(group_Transport->getFloatSlider("BPM"))->setConfig(confg_Sliders);
	//#endif

	//-

	//default positions
	group_Transport->setPosition(ofPoint(400, 5));
	group_HelperTools->setPosition(ofPoint(400, 600));

	//-

	//custom theme
	group_Transport->loadTheme("theme/theme_bleurgh.json");
	group_HelperTools->loadTheme("theme/theme_bleurgh.json");

	//-

	//listeners

	ofAddListener(params_HelperTools.parameterChangedE(), this, &ofxStepSequencer::Changed_Params);
	ofAddListener(params_Transport.parameterChangedE(), this, &ofxStepSequencer::Changed_Params);

	//--

	//set visible
	set_SHOW_Gui(SHOW_Gui);//by default
}

//------------------------------------
void ofxStepSequencer::update()
{
	//ofxSequencer
	sequencer.update();

	if (useFbo)
		toRedraw = true;

	//-

#ifdef USE_OFXBEATCLOCK
	beatClock.update();

	////TODO:
	////should use the beatClock play toggle and bpm params directly

	////not working..it blocks clock
	////workflow to link addons states
	//if (beatClock.getInternalClockMode())
	//	bpm = beatClock.get_BPM();

	//if (beatClock.isPlaying())
	//{

	//}
#endif


}

//--

//--------------------------------------
void ofxStepSequencer::setPosition_GridSequencer(int x, int y, int w, int h)
{
	//sequencer grid

	//size could be defined with w or h. 
	//pass one of them to 0 to get squares sizes

	//-

	seqX = x;
	seqY = y;

	if (w == 0)
	{
		seqH = h;
		seqBoxSize = seqH / NUM_SEQ_NOTES;//will be used to draw squares or number TARGET_NOTES_params
		seqW = seqBoxSize * NUM_SEQ_BEATS;
	}
	else if (h == 0)
	{
		seqW = w;
		seqBoxSize = seqW / NUM_SEQ_BEATS;//will be used to draw squares or number TARGET_NOTES_params
		seqH = seqBoxSize * NUM_SEQ_NOTES;
	}
	else
	{
		seqW = w;
		seqH = h;
	}

	//-

	sequencer.setPosition(seqX, seqY, seqW, seqH);

	//-

	if (useFbo)
	{
		fbo.allocate(seqW, seqH);
		fbo.begin();
		ofClear(0, 0);
		fbo.end();
		toRedraw = true;
	}

	//-
}

//--------------------------------------
void ofxStepSequencer::setPosition_Gui(int x, int y)
{
	//sequencer gui
	group_Transport->setPosition(ofPoint(x, y));
	float h = group_Transport->getHeight();
	group_HelperTools->setPosition(ofPoint(x, y + h));

	//#ifdef USE_OFXSEQUENCER_TRANSPORT
	//	group_Transport->setPosition(ofPoint(x, y));
	//#else
	//	group_HelperTools->setPosition(ofPoint(x, y));
	//#endif
}

#ifdef USE_OFXBEATCLOCK

//--------------------------------------
void ofxStepSequencer::setPosition_CLOCK_Gui(int x, int y, int w)
{
	beatClock.setPosition_Gui(x, y, w);
}

//--------------------------------------
void ofxStepSequencer::setPosition_CLOCK_Squares(int x, int y, int w)
{
	beatClock.setPosition_BeatBoxes(x, y, w);
}

//--------------------------------------
void ofxStepSequencer::setPosition_CLOCK_Ball(int x, int y, int w)
{
	beatClock.setPosition_BeatBall(x, y, w);
}

#endif

//------------------------------------
void ofxStepSequencer::redraw()
{
	//TODO: move all draw into ofxSequencer

	fbo.begin();
	//ofClear(0, 0);

	//-

	//square border

	ofSetColor(ofColor::white);
	ofSetLineWidth(1);
	ofNoFill();

	//ofDrawRectangle(-1, -1, seqWidth+1, seqHeigh+1);
	ofDrawRectangle(1, 1, seqW - 1, seqH - 1);

	//bar lines

	ofSetLineWidth(2);//vertical bar lines
	ofDrawLine(seqW / 4, 0, seqW / 4, seqH);
	ofDrawLine(seqW / 2, 0, seqW / 2, seqH);
	ofDrawLine(seqW * 3 / 4, 0, seqW * 3 / 4, seqH);

	//erase all bars

	ofSetColor(ofColor::black);
	ofFill();
	ofDrawRectangle(0, 0, seqW, seqH);

	//fill selected bar

	int xBar, yBar, wBar, hBar;
	wBar = seqW / 4;
	xBar = wBar * (clone_fromBar - 1);//selected bar
	yBar = 0;
	hBar = seqH;

	ofColor c;
	c.set(ofColor::white);
	int alpha = 24;
	ofSetColor(c.r, c.g, c.b, alpha);
	ofFill();
	ofDrawRectangle(xBar, yBar, wBar, hBar);

	//-

	//note numbers

	float row_h = seqH / (float)(NUM_SEQ_NOTES);//size h of any row line
	int padX = 0;
	int padY = 5;
	int charW = 23;
	int x, y;

	//note labels
	for (int i = 0; i < NUM_SEQ_NOTES; i++)
	{
		if (i < 9)
			padX = 7;//justify right
		else
			padX = 0;

		x = charW + padX;
		y = (i * row_h) + padY + row_h * 0.5;

		ofSetColor(ofColor::white);

		if (TARGET_NOTES_params[i] == true)
		{
			ofFill();
			ofSetColor(ofColor::white);
			int pad = 0;
			if (i < 9)
			{
				pad = 8;
			}

			//square on active TARGET_NOTES_params
			ofDrawRectangle(x - 1 - pad, y - 11, 18, 12);
			ofNoFill();
			ofSetColor(ofColor::black);
		}

		//text number TARGET_NOTES_params
		myFont.drawString(ofToString(i + 1), x, y);
	}

	//-

	fbo.end();
}

//------------------------------------
void ofxStepSequencer::draw_CLOCK()
{
#ifdef USE_OFXBEATCLOCK
	beatClock.draw();
#endif
}

//------------------------------------
void ofxStepSequencer::draw()//draw sequencer grid
{
	if (SHOW_Grid)
	{
		//--

		ofPushStyle();

		//-

		//sequencer grid

		sequencer.draw();

		//-

		if (useFbo)
		{
			if (toRedraw)
			{
				redraw();
				toRedraw = false;
			}

			fbo.draw(seqX, seqY, seqW, seqH);
		}

		else
		{
			//1. square border

			ofSetColor(ofColor::white);
			ofSetLineWidth(1);
			ofNoFill();
			ofDrawRectangle(seqX - 1, seqY - 1, seqW + 1, seqH + 1);

			//-

			//2. the 3 bar vertical lines (between 4 bars dividers)

			ofSetLineWidth(2);//vertical bar lines
			ofDrawLine(seqX + seqW / 4, seqY, seqX + seqW / 4, seqY + seqH);
			ofDrawLine(seqX + seqW / 2, seqY, seqX + seqW / 2, seqY + seqH);
			ofDrawLine(seqX + seqW * 3 / 4, seqY, seqX + seqW * 3 / 4, seqY + seqH);

			//-

			//3.1 selected current player bar/position transparent rectangle

			int xBar, yBar, wBar, hBar;
			wBar = seqW / 4;
			xBar = seqX + wBar * (clone_fromBar - 1);//selected bar
			yBar = seqY;
			hBar = seqH;

			ofColor c;
			c.set(ofColor::white);
			int alpha = 24;
			ofSetColor(c.r, c.g, c.b, alpha);
			ofFill();
			ofDrawRectangle(xBar, yBar, wBar, hBar);

			//-

			if (loopBar)
			{
				//3.2 make darken the bar out of the defined bar loop
				alpha = 192;
				int widthLoop;
				c.set(8);
				ofSetColor(c.r, c.g, c.b, alpha);

				//right rectangle
				xBar = seqX + wBar * (startBar - 1 + numBars);//from end
				//int limitBars = ;
				if (startBar + numBars <= 4)
				{
					widthLoop = wBar * (4 - (startBar - 1 + numBars));
					ofDrawRectangle(xBar, yBar, widthLoop, hBar);
				}

				//left rectangle
				if (startBar > 1)
				{
					xBar = seqX;//first to start
					widthLoop = wBar * (startBar - 1);
					ofDrawRectangle(xBar, yBar, widthLoop, hBar);
				}
			}

			//-

			//4. margin note numbers

			float row_h = seqH / (float)(NUM_SEQ_NOTES);//size h of any row line
			int padX = 0;
			int padY = 5;
			int charW = 23;
			int x, y;

			for (int i = 0; i < NUM_SEQ_NOTES; i++)
			{
				if (i < 9)
					padX = 7;//justify right
				else
					padX = 0;

				x = seqX - charW + padX;
				y = seqY + (i * row_h) + padY + row_h * 0.5;

				ofSetColor(ofColor::white);

				if (TARGET_NOTES_params[i] == true)
				{
					ofFill();
					ofSetColor(ofColor::white);
					int pad = 0;
					if (i < 9)
					{
						pad = 8;
					}

					//square on active TARGET_NOTES_params
					ofDrawRectangle(x - 1 - pad, y - 11, 18, 12);
					ofNoFill();
					ofSetColor(ofColor::black);
				}

				//text number TARGET_NOTES_params
				myFont.drawString(ofToString(i + 1), x, y);
			}
		}

		//-

		ofPopStyle();

		//--------------------------------------------------------//

		//DEBUG:

		if (DEBUG_state)
		{
			//text number TARGET_NOTES_params
			int linesH = 10;
			int maxLines = 6;
			int x = ofGetWidth() - 200;
			int y = ofGetHeight() - (linesH * maxLines);
			string myString;
			int numLine = 0;

			myString = "sequencer.column " + ofToString(sequencer.column);
			ttfType.drawString(myString, x, y + linesH * numLine++);

			myString = "sequencer.getColumn() " + ofToString(sequencer.getColumn());
			ttfType.drawString(myString, x, y + linesH * numLine++);

			myString = "sequencer.cursor " + ofToString(sequencer.cursor);
			ttfType.drawString(myString, x, y + linesH * numLine++);

			myString = "sequencer.getBpm() " + ofToString(sequencer.getBpm());
			ttfType.drawString(myString, x, y + linesH * numLine++);
		}
	}

	//--------------------------------------------------------//

	//TODO: could split this drawer whith his own function..
	presetsManager.draw();//draw clicker panel

	//draw_CLOCK();
}

//------------------------------------------------
void ofxStepSequencer::random(int noteFrom, int numNotes, float prob)
{
	ofLogNotice("ofxStepSequencer") << "SEQ random:";

	////TODO: could add random merge without erase at first
	//clear();//erase all grid

	for (int n = noteFrom; n < (noteFrom + numNotes) && n < NUM_SEQ_NOTES; n++)
	{
		for (int b = 0; b < NUM_SEQ_BEATS; b++)
		{
			//1. SEQ GRID

			float myRandom = ofRandom(10);

			bool rndB = false;
			if (myRandom < randomProb)
				rndB = true;

			if (rndB)
				myDataGrid.grid[n][b] = 1;
			else
				myDataGrid.grid[n][b] = 0;

			//-

			//2. SEQUENCER CLASS

			//ofLogVerbose("ofxStepSequencer") << "note:" << n << " beat:" << b << " state:" << ( rndB  ? "ON":"OFF" );

			//set value to sequencer
			sequencer.setValue<bool>(n, b, rndB);

			//sequencer.set_GridFromSequencer();//TODO: added.. reduces performance..
			//grid squares

			//-
		}
	}
}

//------------------------------------------------
void ofxStepSequencer::clear()
{
	//erase all grid vector and ofxSequencer data grid

	ofLogNotice("ofxStepSequencer") << "clear:";

	bool bState = false;

	for (int row = 0; row < NUM_SEQ_NOTES; row++)
	{
		for (int col = 0; col < NUM_SEQ_BEATS; col++)
		{
			//1. GRID SEQ

			myDataGrid.grid[row][col] = bState;

			//-

			//2. SEQUENCER CLASS

			sequencer.setValue<bool>(row, col, bState);
			//sequencer.set_GridFromSequencer();//TODO: added..
		}
	}
}

//------------------------------------------------
void ofxStepSequencer::cloneFromBar(int selBar)
{
	//--

	//refresh states before cloning, just in case grid cells are modified by mouse clicks.

	//get values to GRID from sequencer class, like clicked cells, (not only from randomized or cloned by ofxStepSequencer, that they are alreay stored)

	GRID_getFrom_Sequencer();

	//--

	ofLogNotice("ofxStepSequencer") << "SEQ cloneFromBar: " << selBar;

	bool GRID_stepsBar[NUM_SEQ_NOTES][4];//fixed 4 steps per bar

	//-

	//1. prepare only selected states from selected bar (from all 12 TARGET_NOTES_params / 16 steps)

	for (int row = 0; row < NUM_SEQ_NOTES; row++)
	{
		ofLogVerbose("ofxStepSequencer") << " > BAR STEPS FROM NOTE " << row << ":";
		ofLogVerbose("ofxStepSequencer") << "  ";

		for (int col = 0; col < 4; col++)
		{
			int step_inBar = col + (selBar * 4);

			GRID_stepsBar[row][col] = myDataGrid.grid[row][step_inBar];

			ofLogVerbose("ofxStepSequencer") << " " << GRID_stepsBar[row][col];
		}
		ofLogVerbose("ofxStepSequencer") << endl;
	}

	//-

	//2. fill the other 3 bars with cloned steps from bar selected

	for (int n = 0; n < NUM_SEQ_NOTES; n++)
	{
		ofLogVerbose("ofxStepSequencer") << "FILL BAR STEPS FROM NOTE " << n << ":";
		for (int b = 0; b < NUM_SEQ_BEATS; b++)
		{
			int numBar = b / 4;
			bool startsBar = (b % 4 == 0);

			//-

			if (numBar == selBar)
			{
				//do not touch selected origin bar
				ofLogVerbose("ofxStepSequencer") << " > step: " << b << " is selected bar. no touch";
			}
			else
			{
				ofLogVerbose("ofxStepSequencer") << " > step: " << b << " is bar " << numBar
					<< (startsBar ? (" and starts the bar " + ofToString(numBar)) : " ");

				int relative_Bar = b % 4;

				//fill grid squares:
				bool bState;
				bState = GRID_stepsBar[n][relative_Bar];

				//-

				if (bState)
					myDataGrid.grid[n][b] = 1;
				else
					myDataGrid.grid[n][b] = 0;

				//TODO: should use a main method to modify both sequencer/gird on class
				sequencer.setValue<bool>(n, b, bState);
				//sequencer.set_GridFromSequencer();//TODO: added. outperforms
			}
		}
	}
}

//--------------------------------------------------------------
void ofxStepSequencer::Changed_Params(ofAbstractParameter &e) //patch change
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();
		ofLogNotice("ofxStepSequencer") << "Changed_Params '" << name << "': " << e;

		if (name == "PLAY")
		{
			if (play_trig)
				setPlayState(true);
			else
				setPlayState(false);
		}
		else if (name == "RANDOM NOTES")
		{
			if (random_trig)
			{
				random_trig = false;
				random(noteFrom, numNotes, randomProb);
			}
		}
		else if (name == "PROB")
		{
			random(noteFrom, numNotes, randomProb);
		}
		else if (name == "FROM")
		{
			random(noteFrom, numNotes, randomProb);
		}
		else if (name == "MAX AMOUNT")
		{
			random(noteFrom, numNotes, randomProb);
		}
		else if (name == "CLEAR ALL")
		{
			if (clear_trig)
			{
				clear_trig = false;
				clear();
		}
	}
		else if (name == "BPM")
		{
#ifndef USE_OFXBEATCLOCK
			sequencer.setBpm(bpm);
#else
			beatClock.set_DAW_bpm(bpm);
#endif
		}

#ifdef USE_OFXBEATCLOCK
		else if (name == "SHOW BEAT-CLOCK")
		{
			beatClock.set_Gui_visible(SHOW_beatClock);
			beatClock.set_BeatBall_visible(SHOW_beatClock);
		}
#endif
		else if (name == "CLONE BARS")
		{
			if (clone_trig)
			{
				cloneFromBar(clone_fromBar - 1);//to use 1-4 not 0
				clone_trig = false;
			}
		}
		else if (name == "FROM BAR")
		{
			//sequencer.
		}
		else if (name == "SHOW PRESETS")
		{
			presetsManager.setVisible_Gui(SHOW_presetsManager);
			presetsManager.setVisible_ClickerPanel(SHOW_presetsManager);
		}

		//-

		//variable bar duration
		else if (name == "startBar" && loopBar)
		{
			sequencer.reset();
			sequencer.column = ((startBar - 1) * 4);
			//sequencer.setStartBar(startBar);
		}
		else if (name == "numBars" && loopBar)
		{
			//sequencer.setNumBarMode(numBars);
		}
		else if (name == "LOOP BAR")
		{
			if (!loopBar)
			{
				//sequencer.stop();
				//sequencer.reset();
				//resetClock();
			}
			else
			{
				//sequencer.stop();
				sequencer.column = ((startBar - 1) * 4) - 1;//-1 to avoid bug
				//resetClock();
			}
			//sequencer.setLoopBarMode(loopBar);
		}

}
}

//------------------------------------------------
void ofxStepSequencer::setPlayState(bool _state)
{
	if (_state)//play
	{
		if (ENABLE_ofxBeatClock_MODE)
		{
#ifdef USE_OFXBEATCLOCK
			////force beatClock to ofxSeq user gui clock
			//if (beatClock.getInternalClockModeState())
			//{
			//	beatClock.set_DAW_bpm(bpm.get());
			//}

			//TODO:
			if (loopBar)
			{
				//sequencer.reset();
				sequencer.column = ((startBar - 1) * 4) - 1;//-1 to avoid bug
			}

			//sequencer.column = 0;
			beatClock.PLAYER_START();
#endif
		}
		else
		{
			//ofxSequencer clocker
			sequencer.start();
		}

		isPlaying = true;
	}
	else if (!_state)//stop
	{
		if (ENABLE_ofxBeatClock_MODE)
		{

#ifdef USE_OFXBEATCLOCK
			beatClock.PLAYER_STOP();
#endif
			//-

			sequencer.stop();
			sequencer.reset();

			//TODO:
			if (loopBar)
			{
				//sequencer.reset();
				sequencer.column = ((startBar - 1) * 4) - 1;//-1 to avoid bug
			}
			//else
			//{
			//	sequencer.column = -1;//?
			//}
			ofLogNotice("ofxStepSequencer") << "sequencer.column = " << sequencer.column;
		}
		else
		{
			//ofxSequencer clocker
			sequencer.stop();
			sequencer.reset();
		}

		isPlaying = false;
	}
}

//------------------------------------------------
void ofxStepSequencer::randomize()
{
	random(noteFrom, numNotes, randomProb);
}

//------------------------------------------------
bool ofxStepSequencer::getPlayState()
{
	return play_trig.get();
}

//------------------------------------------------
void ofxStepSequencer::togglePlayStop()
{
	play_trig = !play_trig;

	//DISABLE_CALLBACKS = true;
	//bool b = !play_trig.get();
	//DISABLE_CALLBACKS = false;
	//play_trig = b;
}

//------------------------------------------------
void ofxStepSequencer::exit()
{
#ifdef USE_OFXBEATCLOCK
	beatClock.exit();

	//get beat ticks events from outside the class
	beatClock.BPM_beat_current.removeListener(this, &ofxStepSequencer::Changed_BPM_beat_current);

	//check changes on beatClock bpm
	beatClock.BPM_Global.removeListener(this, &ofxStepSequencer::Changed_BeatClock_BPM);
#endif

	//-

	//remove listeners

	ofRemoveListener(params_HelperTools.parameterChangedE(), this, &ofxStepSequencer::Changed_Params);
	ofRemoveListener(params_Transport.parameterChangedE(), this, &ofxStepSequencer::Changed_Params);

	//-

	//TODO: easy listener temp solution to trig when save/load is done
	//then will load refresh grid sequencer states

	presetsManager.DONE_save.removeListener(this, &ofxStepSequencer::Changed_DONE_save);
	presetsManager.DONE_load.removeListener(this, &ofxStepSequencer::Changed_DONE_load);

	//-

	//to trig from inside the class
	//ofRemoveListener(TARGET_NOTES_paramsGroup.parameterChangedE(), this, &ofxStepSequencer::Changed_SEQ_NOTES);

	//--

	if (ENABLE_autoSaveLoad)
	{
		save_JSON(path_def);
	}

	//--

	//save settings
	ofXml settings;
	ofSerialize(settings, params_Transport);
	settings.save(path_Global + "/" + path_Settings);
}

//------------------------------------------------
void ofxStepSequencer::GRID_getFrom_Sequencer()
{
	//TODO: should improve updating all sequencer class together (values params and grids)

	ofLogNotice("ofxStepSequencer") << "GRID_getFrom_Sequencer";

	//--

	//get values to local grid from sequencer class,
	//like clicked cells, (not only from randomized or cloned by ofxStepSequencer, that they are alreay stored)

	//TODO: both are required? should be in other places too ? like when clicking cells..

	//1 get sequencer grid from sequencer draw parameters (just in case)
	//TODO: should improve mirroring both sequencer stored data (values/grid)

	sequencer.set_GridFromSequencer();

	//-

	//2 put from sequencer grid into local grid

	//TODO: could be from sequence grid instead

	//for (int r = 0; r < NUM_SEQ_NOTES; r++)
	for (int r = 0; r < sequencer.grid.size(); r++)
	{
		ofLogVerbose("ofxStepSequencer") << "--- NOTE " << r;

		for (int c = 0; c < sequencer.grid[r].size(); c++)
		{
			bool myVal;

			//-

			//TODO: dont know why must invert c <-> r
			myVal = sequencer.grid[r][c];

			//-

			ofLogVerbose("ofxStepSequencer") << "BEAT " << c << " : " << myVal;

			int myInt;
			if (myVal == true)
			{
				myInt = 1;
			}
			else
			{
				myInt = 0;
			}

			//-

			myDataGrid.grid[r][c] = myInt;

			//-
		}
		ofLogVerbose("ofxStepSequencer") << "----------------------------";
	}
}

//------------------------------------------------
void ofxStepSequencer::save_JSON(string path)
{
	//--

	//get values to GRID from sequencer class, like clicked cells, (not only from randomized or cloned by ofxStepSequencer, that they are alreay stored)

	GRID_getFrom_Sequencer();

	//--

	//save from GRID

	ofLogNotice("ofxStepSequencer") << "StepSEQ save_JSON";

	JSON_grid_Row myRow; //note row

	ofJson json_ROW_Note_Steps;
	ofJson json_GRID_Full_Rows_Steps;

	for (int note = 0; note < NUM_SEQ_NOTES; note++)
	{
		//create one json for each note:

		string name = "note_" + ofToString(note);

		for (int beatStep = 0; beatStep < NUM_SEQ_BEATS; beatStep++)
		{
			//store grid sequencer states to json's temp struct

			myRow.steps[beatStep] = myDataGrid.grid[note][beatStep];
		}

		json_ROW_Note_Steps = ofxJsonUtils::create(name, myRow.steps);

		ofLogVerbose("ofxStepSequencer") << "> DUMP: " + name + " " << json_ROW_Note_Steps.dump();

		//-

		json_GRID_Full_Rows_Steps.push_back(json_ROW_Note_Steps);
		//append note(s) json(s) to grid's json
	}

	//-

	ofSavePrettyJson(path, json_GRID_Full_Rows_Steps);

	//-
}

//------------------------------------------------
void ofxStepSequencer::load_JSON(string path)
{
	ofLogNotice("ofxStepSequencer") << "SEQ StepSEQ load_JSON";

	ofJson js;
	ofFile file(path);
	if (file.exists())
	{
		file >> js;
		int n = 0;

		ofLogVerbose("ofxStepSequencer") << " > DUMP NOTE STEPS STATES";

		for (auto &jsNote : js)
		{
			if (!jsNote.empty())
			{
				string jTag = "note_" + ofToString(n);

				ofLogVerbose("ofxStepSequencer") << "   > " + jTag + " " << jsNote.dump();

				std::vector<int> steps = jsNote[jTag];
				//load json array to vector

				int b = 0;
				for (auto i : steps)
				{
					myDataGrid.grid[n][b] = i;

					b++;
				}

				//-

				//debug:

				string debugLine;
				for (int s = 0; s < NUM_SEQ_BEATS; s++)
				{
					debugLine += ofToString(myDataGrid.grid[n][s]) + " ";
				}
				ofLogVerbose("ofxStepSequencer") << "   > GRID   : " + debugLine;

				//-

				n++;
			}
		}

		//--

		//load sequencer grid:

		//go through each grid square
		for (int n = 0; n < NUM_SEQ_NOTES; n++)
		{
			for (int b = 0; b < NUM_SEQ_BEATS; b++)
			{
				//set value to sequencer grid square
				sequencer.setValue<bool>(n, b, myDataGrid.grid[n][b]);
				sequencer.set_GridFromSequencer();//TODO: added
			}
		}
	}
}

//------------------------------------------------
void ofxStepSequencer::setGRID_square(int note, int beat, bool state)
{
	//set value to ofxSequencer class
	sequencer.setValue<bool>(note, beat, state);

	//TODO: should update sequencer grid together wuith an API
	sequencer.set_GridFromSequencer();

	//-

	//set to local ofxStepSequencer storage

	myDataGrid.grid[note][beat] = state;

	//-
}

#ifdef USE_OFXBEATCLOCK

//--------------------------------------------------------------
void ofxStepSequencer::Changed_BeatClock_BPM(float & value)
{
	ofLogVerbose("ofxStepSequencer") << "Changed_BeatClock_BPM: " << value;

	bpm = value;
}

//--------------------------------------------------------------
void ofxStepSequencer::Changed_BPM_beat_current(int &value)
{
	ofLogVerbose("ofxStepSequencer") << "Changed_BPM_beat_current: " << value;

	if (ENABLE_ofxBeatClock_MODE)
	{
		//TODO:
		//add variable pattern duration
		//limited bar
		int limit = ((startBar - 1) * 4) + (numBars * 4 - 1);
		if (limit > 16)
			limit = 15;

		if (sequencer.getColumn() >= limit && loopBar)
		{
			sequencer.column = (startBar - 1) * 4;
			//sequencer.column = 0;
			//sequencer.stop();
			//sequencer.start();
		}
		else//still remains bars
		{
			sequencer.advance();
		}
	}
}

#endif

//TODO: easy listener temp solution
//--------------------------------------------------------------
void ofxStepSequencer::Changed_DONE_save(bool &DONE_save)
{
	ofLogNotice("ofxStepSequencer") << "Changed_DONE_save: " << ofToString(DONE_save ? "TRUE" : "FALSE");

	if (presetsManager.DONE_save)
	{
		presetsManager.DONE_save = false;

		GRID_getFrom_Sequencer();//get sequencer state before saving in preset manager
	}
}

//--------------------------------------------------------------
void ofxStepSequencer::Changed_DONE_load(bool &DONE_load)
{
	ofLogNotice("ofxStepSequencer") << "Changed_DONE_load: " << ofToString(DONE_load ? "TRUE" : "FALSE");

	//load local grid into sequencer:

	if (presetsManager.DONE_load)
	{
		presetsManager.DONE_load = false;

		//-

		//1. put local grid to sequencer grid:

		//       ofLogNotice("ofxStepSequencer") << "ofxSequencer size rows:" << sequencer.grid.size();
		for (int i = 0; i < sequencer.grid.size(); i++)
		{
			//           ofLogNotice("ofxStepSequencer") << "ofxSequencer size cols:" << sequencer.grid[i].size() ;
			for (int j = 0; j < sequencer.grid[i].size(); j++)
			{
				sequencer.grid[i][j] = (bool)myDataGrid.grid[i][j];
			}
		}

		//-

		//2. refresh from sequencer grid to draw params

		sequencer.set_SequencerFromGrid();

		//-
	}
}

//------------------------------------------------
void ofxStepSequencer::set_GuiAll_Settings(ofJson js)
{
	gui_SEQ.setConfig(js);

#ifdef USE_OFXBEATCLOCK
	//take care if USE_OFXBEATCLOCK is enabled inside class header..
	beatClock.gui.setConfig(js);//..then should not set this config
#endif

	presetsManager.gui.setConfig(js);
}

//------------------------------------------------
//to trig from inside the class
//void ofxStepSequencer::Changed_SEQ_NOTES(ofAbstractParameter& e)
//{
//   string name = e.getName();
//   ofLogVerbose("ofxStepSequencer") << "Changed_SEQ_NOTES " << name << ": " << e;
//
//   int noteNum = ofToInt(name);
//   bool state = false;
//
//   if(e.type() == typeid(ofParameter<bool>).name())
//   {
//       ofParameter<bool> p = e.cast<bool>();
//       state = p;
//       ofLogNotice("ofxStepSequencer") << "Changed_SEQ_NOTES: " << name << (p ? " ON":" OFF");
//
//       //TARGET_NOTES_params [noteNum] = state;
//       //pd.sendNoteOn(midiChan, noteFirst + noteNum, (int)state?127:0);
//   }
//}
