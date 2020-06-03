//
// Created by moebiussurfing on 5/15/19.
//

#include "DataGrid.h"

DataGrid::DataGrid()
{
    ofSetLogLevel("DataGrid", OF_LOG_NOTICE);

    ofLogVerbose("DataGrid") << "CONSTRUCTOR";
    name = "";
    id = 0;
    subTag = "row";//text tag for any row (aka subjson) into json file

    //-

    // init data vector

    grid.resize(NUM_SEQ_NOTES);

    for(int i=0 ; i < grid.size() ; i++)
    {
        grid[i].resize(NUM_SEQ_BEATS);

        for(int j=0 ; j < grid[i].size() ; j++) //init
        {
            grid[i][j] = 0;
        }
    }
}

//------------------------------------------------
void DataGrid::setup(int nNotes, int nBeats)///for custom grid sizes
{
	//init data vector
	grid.clear();
	grid.resize(nNotes);

	for (int i = 0; i < grid.size(); i++)
	{
		grid[i].resize(nBeats);

		for (int j = 0; j < grid[i].size(); j++) //init
		{
			grid[i][j] = 0;
		}
	}
}

//------------------------------------------------
void DataGrid::setName(const std::string _name){
    name = _name;
}

//------------------------------------------------
std::string DataGrid::getName() const {
    return name;
}

//------------------------------------------------
void DataGrid::save_JSON(string path)
{
    ofLogNotice("DataGrid") << "save_JSON";

    ofJson JSON_row;//temp row(note) json for the slow mode parser
    ofJson JSON_grid;

    //-

    // A. slow mode with tags

    // TODO: should define data vector with open size..

    // create one json for each row aka note:
    for (int note = 0;  note < NUM_SEQ_NOTES; note++)
    {
        string name = subTag + ofToString(note);

        // JSON_row = ofxJsonUtils::create( kv( grid[note] ) );
        JSON_row = ofxJsonUtils::create( name, grid[note] );

        ofLogVerbose("DataGrid") << "DUMP: " + name + " " << JSON_row.dump();

        //-

        JSON_grid.push_back(JSON_row);
    }

    //-

    // B. fast mode vector without tags
    //    JSON_grid = ofxJsonUtils::create( kv( grid ) );

    //-

    ofSavePrettyJson(path, JSON_grid);
}


//------------------------------------------------
void DataGrid::dump_grid()
{
    ofLogNotice("DataGrid") << "dump_grid";

    for (int note = 0; note < NUM_SEQ_NOTES; note++) {

        string str;
        for (int s = 0; s < NUM_SEQ_BEATS ; s++)
        {
            str += ofToString( grid[note][s] ) + " ";
        }
        ofLogVerbose("DataGrid") << "NOTE: " << note << " BEATS: " + str;
    }
}

//------------------------------------------------
void DataGrid::randomize_grid() {

    ofLogNotice("DataGrid") << "randomize_grid";

    //-

//    id = 1;
//    name = "grid_" + ofToString(id) + ".json";

    //-

    // random initiate

    string str;

    // TODO: should define data vector with open size..

    for( size_t n = 0; n < NUM_SEQ_NOTES; n++ )
    {
        //ofLogVerbose("DataGrid") << subTag + ofToString(n ) + " -- ";
        // cout << subTag << n << " -- ";

        for( size_t b = 0; b < NUM_SEQ_BEATS; b++ )
        {
            int iState = (int) ofRandom(0, 2);

            // cout << "b" << b << ":" <<iState << " ";
            str +=  "b" + ofToString(b) + ":" + ofToString(iState) + " ";

            grid[n][b] = iState;
        }
        // cout << endl;

        //ofLogVerbose("DataGrid") << str;
    }
    //-
}

//------------------------------------------------
void DataGrid::load_JSON(string path)
{
    ofLogNotice("DataGrid") << "load_JSON";

    //-

    ofJson js;
    ofFile file( path );
    if(file.exists())
    {
        file >> js;
        ofLogVerbose("DataGrid") << " > DUMP GRID: ";

        int n = 0;
        for (auto &js_Row: js) {
            if (!js_Row.empty()) {
                string js_Tag = subTag + ofToString(n);
                ofLogVerbose("DataGrid") << "subJSON:" + js_Tag + ": " << js_Row.dump();

                std::vector<int> steps = js_Row[js_Tag];
                //load json array to vector
                int b = 0;
                for (auto i : steps) {
                    grid[n][b] = i;
                    b++;
                }
                n++;
            }
        }
    }

    //-

    // fast mode do not works. maybe because using array not vectors..

    //    ofJson js = ofxJsonUtils::loadFromFile(path);
    //    ofLogNotice("DataGrid") << "DUMP GRID: " << js.dump();
    //    ofxJsonUtils::load( json, kv( grid[][] ) );

    //-
}
