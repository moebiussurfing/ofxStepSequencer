//ofxPresetDataGrid.h
//Nicola Pisanti, MIT License, 2016
//
//changes by moebiussurfing

#include "ofxPresetDataGrid.h"

//------------------------------------------------
ofxPresetDataGrid::ofxPresetDataGrid()
{
	//ofSetLogLevel("ofxPresetDataGrid")
	ofSetLogLevel(OF_LOG_VERBOSE);

	DISABLE_CALLBACKS = true;

	modeKey = OF_KEY_CONTROL;
	bKeys = false;
	keysNotActivated = true;
	bKeySave = false;

	//-

	grids.reserve(32);//TODO: this multidimension is for multiple panels ?

	//-

	lastIndices.reserve(32);//TODO: this multidimension is for multiple panels ?
	keys.reserve(32);//TODO: this multidimension is for multiple panels ?

	lastMouseButtonState = false;
	bDelayedLoading = false;

	//-

	//TODO: easy listener temp solution for ofxStepSequencer integration
	DONE_load.set("DONE LOAD", false);
	DONE_save.set("DONE SAVE", false);

	//-

	myTTF = path_Global + "/fonts/telegrama_render.otf";
	sizeTTF = 8;
	myFont.load(myTTF, sizeTTF, true, true);

	//-

	//PRESETS OF DEVICE

	PRESET_selected.set("PRESETS", 1, 1, num_presets);

	params.setName("PRESETS");
	params.add(PRESET_selected);
	bSave.set("SAVE", false);
	bAutoLoad.set("AUTO LOAD", false);
	bAutoSave.set("AUTO SAVE", false);
	bCloneToRight.set("CLONE >", false);
	params.add(bSave);
	params.add(bAutoLoad);
	params.add(bAutoSave);
	params.add(bCloneToRight);

	ofAddListener(params.parameterChangedE(), this, &ofxPresetDataGrid::Changed_Params);

	//----

	//startup

	PRESET_selected_PRE = -1;

	DISABLE_CALLBACKS = false;

	//----
}

//------------------------------------------------
ofxPresetDataGrid::~ofxPresetDataGrid()
{
	save_ControlSettings();

	ofRemoveListener(params.parameterChangedE(), this, &ofxPresetDataGrid::Changed_Params);
}

//------------------------------------------------
int ofxPresetDataGrid::getGuiIndex(string name) const {

	for (size_t i = 0; i < grids.size(); ++i)
	{
		string myName = grids[i]->getName();
		if (myName == name) {
			return i;
		}
		return -1;
	}
}

//------------------------------------------------
string ofxPresetDataGrid::presetName(string guiName, int presetIndex)
{
	string folder;
	folder = "/"; //without subfolder. must ends with "/"
	//folder = "/patterns/"; //using subfolder

	return (folder + guiName + "_preset_" + ofToString(presetIndex) + ".json");
}

//------------------------------------------------
void ofxPresetDataGrid::add(DataGrid & grid, int numPresets)
{
	//add a gui for preset saving

	grids.push_back(&grid);

	lastIndices.push_back(0);
	newIndices.push_back(0);
	presets.push_back(numPresets);

	//-

	num_presets = numPresets;
	(group->getIntSlider("PRESETS"))->setMax(num_presets);
}

//------------------------------------------------
void ofxPresetDataGrid::add(DataGrid & grid, initializer_list<int> keysList)
{
	add(grid, keysList.size());

	keys.resize(grids.size());
	int i = grids.size() - 1;

	keys[i].reserve(keysList.size());

	for (const int & key : keysList) keys[i].push_back(key);

	if (keysNotActivated)
		addKeysListeners();
}

//------------------------------------------------
void ofxPresetDataGrid::save(int presetIndex, int guiIndex)
{
	if (guiIndex >= 0 && guiIndex < (int)grids.size())
	{
		ofLogNotice("ofxPresetDataGrid") << "DONE_save";
		DONE_save = true;
		//will make in SEQ: get sequencer state before saving in preset manager
		//GRID_getFrom_Sequencer();

		//-

		std::string n = presetName(grids[guiIndex]->getName(), presetIndex);

		ofLogNotice("ofxPresetDataGrid") << "save(presetIndex, guiIndex): " << presetIndex << ", " << guiIndex;
		ofLogNotice("ofxPresetDataGrid") << "grids[guiIndex]->getName(): " << grids[guiIndex]->getName();
		ofLogNotice("ofxPresetDataGrid") << "n: " << n;

		grids[guiIndex]->save_JSON(pathKitFolder + "/" + n);
	}
}

//------------------------------------------------
void ofxPresetDataGrid::save(int presetIndex, string guiName)
{
	int guiIndex = getGuiIndex(guiName);

	if (guiIndex >= 0 && guiIndex < (int)grids.size())
	{
		ofLogNotice("ofxPresetDataGrid") << "DONE_save";
		DONE_save = true;
		//will make in SEQ: get sequencer state before saving in preset manager
		//GRID_getFrom_Sequencer();

		//-

		ofLogNotice("ofxPresetDataGrid") << "save( presetIndex, guiName): " << presetIndex << ", " << guiName;
		ofLogNotice("ofxPresetDataGrid") << "guiIndex = getGuiIndex(guiName): " << guiIndex;

		string n = presetName(guiName, presetIndex);

		grids[guiIndex]->save_JSON(pathKitFolder + "/" + n);
	}
}

//------------------------------------------------
void ofxPresetDataGrid::load(int presetIndex, int guiIndex)
{
	if (guiIndex >= 0 && guiIndex < (int)grids.size())
	{
		ofLogNotice("ofxPresetDataGrid") << "load(presetIndex, guiIndex): " << presetIndex << ", " << guiIndex;

		string str = presetName(grids[guiIndex]->getName(), presetIndex);
		grids[guiIndex]->load_JSON(pathKitFolder + "/" + str);

		lastIndices[guiIndex] = presetIndex;

		//grids[guiIndex]->dump_grid();

		//-

		ofLogNotice("ofxPresetDataGrid") << "DONE_load";
		DONE_load = true;
	}
}

//------------------------------------------------
void ofxPresetDataGrid::load(int presetIndex, string guiName)
{
	int guiIndex = getGuiIndex(guiName);

	if (guiIndex >= 0 && guiIndex < (int)grids.size()) {

		ofLogNotice("ofxPresetDataGrid") << "load(presetIndex, guiName): " << presetIndex << ", " << guiName;

		string n = presetName(guiName, presetIndex);
		grids[guiIndex]->load_JSON(pathKitFolder + "/" + n);

		lastIndices[guiIndex] = presetIndex;

		//-

		ofLogNotice("ofxPresetDataGrid") << "DONE_load";
		DONE_load = true;
	}
}

//------------------------------------------------
int ofxPresetDataGrid::getPresetIndex(int guiIndex) const
{
	if (guiIndex > 0 && guiIndex < (int)grids.size()) {
		return lastIndices[guiIndex];
	}
	else {
		return -1;
	}
}

//------------------------------------------------
int ofxPresetDataGrid::getPresetIndex(string guiName)const
{
	int guiIndex = getGuiIndex(guiName);

	if (guiIndex > 0 && guiIndex < (int)grids.size())
	{
		return lastIndices[guiIndex];
	}
	else
	{
		return -1;
	}
}

//------------------------------------------------
void ofxPresetDataGrid::setModeKey(int key)
{
	modeKey = key;
}

//----------------------------------------------------------------
void ofxPresetDataGrid::keyPressed(ofKeyEventArgs& eventArgs)
{
	if (bKeys && ENABLE_shortcuts)
	{
		const int & key = eventArgs.key;

		if (key == modeKey)
		{
			bKeySave = true;
			return;
		}

		for (size_t i = 0; i < keys.size(); ++i)
		{
			for (size_t k = 0; k < keys[i].size(); ++k)
			{
				if (key == keys[i][k])
				{
					if (bKeySave)
					{
						save(k, i);
					}
					else
					{
						if (bDelayedLoading)
						{
							//newIndices[i] = k;
							//ofLogNotice("ofxPresetDataGrid") << "newIndices[i] = k;" <<  k << ", " << i;
							//PRESET_selected = 1 + k;
						}
						else
						{
							//load( k, i );
							//ofLogNotice("ofxPresetDataGrid") << "load( k, i ):" <<  k << ", " << i;

							PRESET_selected = 1 + k;//first row/gui only
						}
					}
					return;
				}
			}
		}
	}
}

//------------------------------------------------
void ofxPresetDataGrid::keyReleased(ofKeyEventArgs& eventArgs)
{
	if (eventArgs.key == modeKey && ENABLE_shortcuts) bKeySave = false;
}

//------------------------------------------------
void ofxPresetDataGrid::addKeysListeners()
{
	ofAddListener(ofEvents().keyPressed, this, &ofxPresetDataGrid::keyPressed);
	ofAddListener(ofEvents().keyReleased, this, &ofxPresetDataGrid::keyReleased);
	keysNotActivated = false;
	bKeys = true;
}

//------------------------------------------------
void ofxPresetDataGrid::setPosition_ClickerPanel(int x, int y, int cellSize)
{
	this->x = x;
	this->y = y;
	this->cellSize = cellSize;
}

//------------------------------------------------
void ofxPresetDataGrid::draw(int x, int y, int cellSize)
{
	setPosition_ClickerPanel(x, y, cellSize);
	draw();
}

//---------------------------------------------------------------------
void ofxPresetDataGrid::draw()
{
	if (SHOW_ClickPanel)
	{
		if (!lastMouseButtonState && ofGetMousePressed())
		{
			mousePressed(ofGetMouseX(), ofGetMouseY());
		}
		lastMouseButtonState = ofGetMousePressed();

		ofPushMatrix();
		ofPushStyle();

		ofSetLineWidth(1.0f);
		ofSetColor(ofColor::white);
		ofNoFill();
		ofTranslate(x, y);

		for (size_t i = 0; i < keys.size(); ++i)
		{
			size_t k = 0;
			for (; k < keys[i].size(); ++k)
			{
				ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

				if (!myFont.isLoaded())
					ofDrawBitmapString(ofToString((char)keys[i][k]), cellSize*k + 8, cellSize*i + 18);
				else
					myFont.drawString(ofToString((char)keys[i][k]),
						cellSize * k + 0.5f * cellSize - 0.25f * sizeTTF,
						cellSize * i + 0.5f * cellSize + 0.5f * sizeTTF);

				if (lastIndices[i] == k) ofDrawRectangle(cellSize * k + 4, cellSize * i + 4, cellSize - 8, cellSize - 8);
			}
			for (; k < presets[i]; ++k)
			{
				ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);
				if (lastIndices[i] == k) ofDrawRectangle(cellSize * k + 4, cellSize * i + 4, cellSize - 8, cellSize - 8);
			}

			//save button
			ofDrawRectangle(cellSize * k, cellSize * i, cellSize, cellSize);

			//label
			if (!myFont.isLoaded())
				ofDrawBitmapString("SAVE", cellSize*k + 10, cellSize*i + 18);
			else
				myFont.drawString("SAVE", cellSize * k + sizeTTF - 2, cellSize * i + cellSize * 0.5f + 0.5 * sizeTTF);

			k++;

			//-

			//kit name
			//ofDrawBitmapString( grids[i]->getName(), cellSize*k+8, cellSize*i+18 );

			//-
		}

		ofPopStyle();
		ofPopMatrix();
	}
}

//-----------------------------------------------------
void ofxPresetDataGrid::mousePressed(int x, int y)
{
	x -= this->x;
	y -= this->y;

	int xIndex = x / cellSize;
	int yIndex = y / cellSize;

	xIndex = (x > 0) ? xIndex : -1;
	yIndex = (y > 0) ? yIndex : -1;

	//if (xIndex != -1 && yIndex != -1)
	//	ofLogNotice("ofxPresetDataGrid") << "mousePressed - xIndex, yIndex: " << xIndex << ", " << yIndex;

	//-

	//custom DataGrid class

	if (yIndex >= 0 && yIndex < (int)grids.size())
	{
		//row buttons, preset selctors
		if (xIndex >= 0 && xIndex < presets[yIndex])
		{
			//load
			if (bDelayedLoading)
			{
				//               newIndices[yIndex] = xIndex;
				//               ofLogNotice("ofxPresetDataGrid") << "newIndices[yIndex] = xIndex:" <<  yIndex << " = " << xIndex;
			}
			else
			{
				//               load( xIndex, yIndex);
				//               ofLogNotice("ofxPresetDataGrid") << "load( xIndex, yIndex):" <<  xIndex << ", " << yIndex;

				PRESET_selected = 1 + xIndex;
			}
		}

		//last button (save button)
		else if (xIndex == presets[yIndex])
		{
			ofLogNotice("ofxPresetDataGrid") << "saveButton: ( lastIndices[yIndex], yIndex ): " << lastIndices[yIndex] << ", " << yIndex;

			//save

			//save( lastIndices[yIndex], yIndex );

			////save( k, i);//this is from keyPressed
			////PRESET_selected = 1 + yIndex;

			doSave(lastIndices[yIndex]);
		}
	}

	//-
}

//------------------------------------------------
void ofxPresetDataGrid::setDelayedLoading(bool active)
{
	bDelayedLoading = active;
}

//------------------------------------------------
void ofxPresetDataGrid::delayedLoad(int presetIndex, int guiIndex)
{
	if (guiIndex >= 0 && guiIndex < (int)grids.size())
	{
		newIndices[guiIndex] = presetIndex;
	}
}

//------------------------------------------------
void ofxPresetDataGrid::delayedLoad(int presetIndex, string guiName)
{
	int guiIndex = getGuiIndex(guiName);

	if (guiIndex >= 0 && guiIndex < (int)grids.size())
	{
		newIndices[guiIndex] = presetIndex;
	}
}

//------------------------------------------------
void ofxPresetDataGrid::delayedUpdate()
{
	for (size_t i = 0; i < grids.size(); ++i)
	{
		if (newIndices[i] != lastIndices[i])
		{
			load(newIndices[i], i);
		}
	}
}

//------------------------------------------------
void ofxPresetDataGrid::toggleKeysControl(bool active)
{
	bKeys = active;
}

//--

//PRESET GUI MANAGER

//------------------------------------------------
void ofxPresetDataGrid::Changed_Params(ofAbstractParameter &e)
{
	if (!DISABLE_CALLBACKS)
	{
		string name = e.getName();

		ofLogNotice("ofxPresetDataGrid") << "Changed_Params '" << name << "': " << e;

		if (name == "SAVE" && (bSave)) {
			ofLogNotice("ofxPresetDataGrid") << "SAVE: " << e;
			doSave(PRESET_selected - 1);
		}

		else if (name == "CLONE >" && (bCloneToRight))
		{
			ofLogNotice("ofxPresetDataGrid") << "CLONE >: " << e;
			doCloneRight(PRESET_selected - 1);
		}

		else if (name == "PRESETS" && (PRESET_selected != PRESET_selected_PRE))
		{
			ofLogNotice("ofxPresetDataGrid") << "Changed PRESET_selected: " << e;

			//load
			if (bDelayedLoading)//TODO: not implemented
			{
				ofLogNotice("ofxPresetDataGrid") << "bDelayedLoading: " << bDelayedLoading;

				//byKey
				//newIndices[i] = k;
				//ofLogNotice("ofxPresetDataGrid") << "newIndices[i] = k;" <<  k << ", " << i;

				//byMousePressed
				//ofLogNotice("ofxPresetDataGrid") << "newIndices[yIndex] = xIndex:" <<  yIndex << " = " << xIndex;
			}

			else
			{
				//byKey
	//           load( k, i );
	//           ofLogNotice("ofxPresetDataGrid") << "load( k, i ):" <<  k << ", " << i;

				//byMousePressed
	//           load( xIndex, yIndex);
	//           ofLogNotice("ofxPresetDataGrid") << "load( xIndex, yIndex):" <<  xIndex << ", " << yIndex;

				//-

				if (bAutoSave)
				{
					ofLogNotice("ofxPresetDataGrid") << "AUTO SAVE: " << e;
					doSave(PRESET_selected_PRE - 1);

					//DONE_save = true;//get sequencer state before saving in preset manager
					//save(lastIndices[PRESET_selected_PRE], 0);//save on back from ofxStepSequencerlistener
				}

				//-

				PRESET_selected_PRE = PRESET_selected;
				int xIndex = PRESET_selected - 1;
				int yIndex = 0;
				//this should handle when using multiple kits together
				//yIndex = ?

				//-

				if (bAutoLoad)
				{
					load(xIndex, yIndex);
					ofLogNotice("ofxPresetDataGrid") << "load( xIndex, yIndex): " << xIndex << ", " << yIndex;

					DONE_load = true;//TODO: to refresh in SEQ sequencer from grid..
				}
				else
				{
					lastIndices[yIndex] = xIndex;//this is to move clicker selector
					ofLogNotice("ofxPresetDataGrid") << "lastIndices[yIndex]: " << xIndex;
					ofLogNotice("ofxPresetDataGrid") << "bAutoLoad: " << bAutoLoad;
				}
			}
		}
	}
}

//--

//GUI CONTROL PANEL

//------------------------------------------------
void ofxPresetDataGrid::setup_Gui()
{
	gui_w = 200;
	gui_slider_h = 16;
	gui_slider_big_h = 18;
	gui_button_h = 22;

	//-

	//container
	confCont =
	{
		//{"direction", "vertical"},
		//{"width", gui_w},
		//{"padding", 0},
		//{"margin", 0},
	};

	//toggle
	confItem_toggle =
	{
		//{"height", gui_slider_h},
	};

	//sliders & button
	confItem =
	{
		//{"type", "fullsize"},
		//{"height", gui_slider_h},
	};

	//big sliders
	confItem_Big =
	{
		//{"type", "fullsize"},
		//{"height", gui_slider_big_h},
	};

	//big buttons
	confItem_Fat =
	{
		//{"type", "fullsize"},
		//{"height", gui_button_h},
		//{"text-align", "center"},
	};

	//-

	//customize panel name to class mode

	string name_Panel;
	name_Panel = "SEQ PATTERNS";

	group = gui.addGroup(name_Panel);//, confCont);
	group->add<ofxGuiIntSlider>(PRESET_selected);//, confItem_Big);
	group->add<ofxGuiButton>(bSave);//, confItem_toggle);
	group->add<ofxGuiToggle>(bAutoLoad);//, confItem_toggle);
	group->add<ofxGuiToggle>(bAutoSave);//, confItem_toggle);
	group->add<ofxGuiButton>(bCloneToRight);//, confItem_toggle);

	//-

	group->setPosition(600, 550);

	//TODO:
	group->loadTheme("theme/theme_bleurgh.json");
	group->minimize();
}

//------------------------------------------------
void ofxPresetDataGrid::set_GUI_position(int x, int y)
{
	group->setPosition(x, y);
}

//------------------------------------------------
void ofxPresetDataGrid::setVisible_Gui(bool visible)
{
	SHOW_Gui = visible;
	gui.getVisible().set(SHOW_Gui);
}

//------------------------------------------------
void ofxPresetDataGrid::setVisible_ClickerPanel(bool visible)
{
	SHOW_ClickPanel = visible;
}

//------------------------------------------------
void ofxPresetDataGrid::load_ControlSettings()
{
	ofXml settings;
	settings.load(path_Global + "/" + pathControl);
	ofDeserialize(settings, params);

	ofLogNotice("ofxPresetDataGrid") << "load_ControlSettings > PRESET_selected: " << PRESET_selected;
}

//------------------------------------------------
void ofxPresetDataGrid::save_ControlSettings()
{
	ofXml settings;
	ofSerialize(settings, params);
	settings.save(path_Global + "/" + pathControl);
}

//------------------------------------------------
void ofxPresetDataGrid::set_pathKit_Folder(string folder)
{
	pathKitFolder = folder;
}

//------------------------------------------------
void ofxPresetDataGrid::doCloneRight(int patternNum)
{
	ofLogNotice("ofxPresetDataGrid") << "doCloneRight: patternNum: " << patternNum;
	for (int i = patternNum + 1; i < num_presets; i++)
	{
		save(i, 0);//only 1 row (gui) / data content
	}
}

//------------------------------------------------
void ofxPresetDataGrid::doSave(int patternNum)
{
	ofLogNotice("ofxPresetDataGrid") << "doSave: patternNum: " << patternNum;
	save(patternNum, 0);//only 1 row (gui) / data content
}
