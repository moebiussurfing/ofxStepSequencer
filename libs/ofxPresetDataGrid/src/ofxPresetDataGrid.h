
//ofxPresetDataGrid.h
//original addon by Nicola Pisanti, MIT License, 2016
//
//changes and customized by moebiussurfing:
//
//LOG:
//+ switched from ofxGui to ofxGuiExtended
//+ switched preset management from ofxGuiPanel to ofParametersGroup
//+ added custom DATA class DataGrid
//+ integrated to ofxStepSequencer
//+ added control gui panel: slider selector, cloner, save
//
//TODO:
//+ could make tween when changing params
//+ save full kit of presets to avoid blocking main thread when switching presets
//because json loadings
//+ use pointers between classes (ofxSequencer) to share the data struct
//from DataGrid class,
//ofxStepSequencer, or wherever is used ... almost done. but maybe should add listeners
//re players when preset are loaded/saved..

#pragma once

#include "ofMain.h"
#include "ofxGuiExtended2.h"

#include "DataGrid.h"

#define NUM_OF_PRESETS 8

class ofxPresetDataGrid {

public:

	bool DISABLE_CALLBACKS = false;

    ofxPresetDataGrid();
    ~ofxPresetDataGrid();

	//-

	string path_Global = "ofxStepSequencer";
	string pathControl = "presetsControl.xml";

    //-

    //add a gui for preset saving
    void add( DataGrid & grid, int numPresets=8 );
    //adds and activate key switch
    void add( DataGrid & grid, initializer_list<int> keysList );

    //-

    //save to a preset
    void save( int presetIndex, int guiIndex=0 );
    void save( int presetIndex, string guiName );

    //loads an already saved preset
    void load( int presetIndex, int guiIndex=0 );
    void load( int presetIndex, string guiName );

    //get the last loaded preset
    int getPresetIndex( int guiIndex ) const;
    int getPresetIndex( string guiName ) const;

    //set the key you have to hold for saving, default is OF_KEY_CONTROL
    void setModeKey( int key );

    //-

    //API

    //set graphics position
    //cellsize is the size of each preset button
    void setPosition_ClickerPanel( int x, int y, int cellSize );

    //-

    //draw some info, when the gui is drawn you can also click on the button to change / save presets
    void draw();
    void draw( int x, int y, int cellSize );

    //-

    //DELAYED LOADING

    //if you set it to true the preset will be loaded only when you call (false is the default behavior)
    void setDelayedLoading( bool active );
    //make preset change effective when setDelayedLoading() is set to true
    void delayedUpdate();
    //if setDelayedLoading() is set to true stages a load action
    void delayedLoad( int presetIndex, int guiIndex=0 );
    void delayedLoad( int presetIndex, string guiName );

    //switch on or off the control with the keys
    void toggleKeysControl( bool active );

    //-

    //TODO: easy listener temp solution for ofxStepSequencer integration
    ofParameter<bool> DONE_load;
    ofParameter<bool> DONE_save;

    //-

private:

    int getGuiIndex(string name ) const;
    string presetName( string guiName, int presetIndex );

    //-

    //DATA

    //B. custom DataGrid class
    vector<DataGrid*> grids;

    //-

    vector<int> lastIndices;
    vector<int> presets;

    void addKeysListeners();
    void keyPressed( ofKeyEventArgs& eventArgs);
    void keyReleased( ofKeyEventArgs& eventArgs );

    bool bKeys;
    vector<vector<int>> keys;
    bool keysNotActivated;
    int modeKey;
    bool bKeySave;

    int x;
    int y;
    int cellSize;

    bool lastMouseButtonState;
    void mousePressed( int x, int y );

    bool bDelayedLoading;
    vector<int> newIndices;

    //-

    ofTrueTypeFont	myFont;
    string myTTF;//gui font for all gui theme
    int sizeTTF;

    //-

public:

    //GUI

    ofJson confCont, confItem, confItem_toggle, confItem_Big, confItem_Fat;
    void setup_Gui();
    void Changed_Params(ofAbstractParameter &e);

    //-

    //control panel to selec presets, clone, save..
    ofxGui gui;
    ofParameterGroup params;
    ofxGuiGroup2 * group;
    
    ofParameter<int> PRESET_selected;
    int PRESET_selected_PRE = -1;
    ofParameter<bool> bSave;
    ofParameter<bool> bAutoSave;
    ofParameter<bool> bAutoLoad;
    ofParameter<bool> bCloneToRight;
    void doCloneRight(int patternNum);
    void doSave(int patternNum);
    int num_presets;

    //-

    void load_ControlSettings();
    void save_ControlSettings();

    //-

    //API

    void set_GUI_position(int x, int y);
    void setVisible_Gui(bool visible);
    void setVisible_ClickerPanel(bool visible);
    void set_pathKit_Folder(string folder);
    string pathKitFolder = "myKit";//default kit folder to store patterns

    //-

    bool SHOW_Gui;
    bool SHOW_ClickPanel;
    bool ENABLE_shortcuts = true;

    int gui_w;
    int gui_slider_h;
    int gui_slider_big_h;
    int gui_button_h;

    //--
};
