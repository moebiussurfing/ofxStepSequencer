//
// Created by moebiussurfing on 5/15/19.
//

#pragma once

#include "ofMain.h"
#include "ofxJsonUtils.h"

#define NUM_SEQ_NOTES 12//max & fixed size
#define NUM_SEQ_BEATS 16//max & fixed size


class DataGrid {

public:
    DataGrid();

    std::string name;
    int	id;

	void setup(int nNotes, int nBeats);///for custom grid sizes
	int NumSeqNotes = 12;
	int NumSeqBeats = 16;

    //-

    // DATA

    vector < vector<int> > grid;
    // int grid[NUM_SEQ_NOTES][NUM_SEQ_BEATS];

    //-

    void save_JSON(string path);
    void load_JSON(string path);

    void dump_grid();
    void randomize_grid();

    string subTag;

    void setName(std::string name);
    std::string getName() const;
};

