
///-------------------------------------------------------------
///addon modified from the original from:
///https://github.com/genekogan/ofxSequencer
///
///TODO: BUG: starting skips step 0... A standby state or step 0 (col -1) should be defined, to jump there when sequencer is stoped. Then, when staring, should jump to step 1 (col = 0)
///shoul be added a new standby -1 step to columns..
///-------------------------------------------------------------

///TODO: BUG: massive overlap on startup ofOrganelle
///should debug: [notice ] ofxSequencer: set_GridFromSequencer
///] ofxSequencer: store_Row_Values: c: 14 = 0
///[verbose] ofxSequencer: store_Row_Values: c: 15 = 0

#pragma once

#include "ofMain.h"
#include "ofxSequencerBpm.h"

//#define NUM_SEQ_NOTES 12//max & fixed size
//#define NUM_SEQ_BEATS 16//max & fixed size

//--

//T ofxSequencerRowBase

struct ofxSequencerRowBase
{
    ofxSequencerRowBase(int cols);
    
    virtual string getName()
    {
        string name; return name;
    }
    
    template<class T> T getMin();
    template<class T> T getMax();
    
    template<class T> T getValue();//my getter
    
    template<class T> void setValue(int idx, T value);

    bool getValue_Cell(int idx);//works?
    
	//TODO:
	virtual void resetParam() { }

    virtual void update(int column) { }
    virtual void update(float cursor) { }
    virtual void randomize() { }
    
    virtual void mousePressed(int col, int x, int y) { }
    virtual void mouseDragged(int col, int y) { }
    virtual void mouseReleased(int col) { }
    
    virtual void draw(int col, int cellWidth, int cellHeight) { }

    int cols;
    
    //-

    //API

    virtual void store_Row_Values() { }

    //DATA

    vector<bool> grid_Rows;
    
};//ofxSequencerRowBase

//------------------

//T ofxSequencerRow

template<typename T>
struct ofxSequencerRow : public ofxSequencerRowBase
{
    ofxSequencerRow(ofParameter<T> * parameter, int cols);
    
    string getName()
    {
        return parameter->getName();
    }
    
    T getMin()
    {
        return parameter->getMin();
    }
    
    T getMax()
    {
        return parameter->getMax();
    }
    
    T getValue()
    {
        return parameter->getValue();
    }
    
    void setValue(int idx, T value)
    {
        values[idx] = value;
    }

    bool getValue_Cell(int idx)//works
    {
        bool myVal = (bool) values[idx];
        ofLogVerbose("ofxSequencer") << "- getValue_Cell(idx) : " << myVal;
        //return parameter->getMax();
        return myVal;//bool
    }
    
    //-
    
    bool get_CellValue(int idx)
    {
        bool myVal = (bool) values[idx];
        //ofLogVerbose("ofxSequencer") << "get_CellValue[idx] : " << myVal;
        return myVal;
    };
    
	//TODO:
	void resetParam();

    void update(int column);
    void update(float cursor);
    
    void randomize();
    void store_Row_Values();
    
    void mousePressed(int col, int x, int y);
    void mouseDragged(int col, int y);
    void mouseReleased(int col);
    void draw(int col, int cellWidth, int cellHeight);
    
    //-
    
    ofParameter<T> * parameter;
    vector<T> values;
    T pValue;
    ofPoint pMouse;
    
    //-
    
}; //ofxSequencerRow

//------------------

//creator row with columns

template<class T>
ofxSequencerRow<T>::ofxSequencerRow(ofParameter<T> * parameter, int cols) : ofxSequencerRowBase(cols)
{
    this->parameter = parameter;
    for (int c=0; c<cols; c++)
    {
        values.push_back(parameter->get());
        
        //--
        
        //init row grid_Rows values vector
        grid_Rows.push_back( (bool) ( parameter->get() ) );
        
        //--
    }
} //ofxSequencerRow

//------------------

//TODO:
//workaround to reset bool param only. 
//should upgrade to use with other types using min or zero/false
template<class T>
void ofxSequencerRow<T>::resetParam()
{
	*parameter = false;
}

//--

//update row parameter in current column

template<class T>
void ofxSequencerRow<T>::update(int column)
{
    *parameter = values[column];
}

//------------------

//update row parameter by cursor

template<class T>
void ofxSequencerRow<T>::update(float cursor)
{
    *parameter = ofLerp(values[(int) floor(cursor)], values[(int) ceil(cursor) % values.size()], cursor - floor(cursor));
}

//------------------

//randomize all columns (steps) in the row

template<class T>
void ofxSequencerRow<T>::randomize()
{
    for (int i = 0; i < values.size(); i++) {
        values[i] = ofRandom(parameter->getMin(), parameter->getMax());
        
        //--
        
        //TODO: add to grid_Rows too
        grid_Rows[i] = (bool) values[i];
    }
}

//------------------

//read (from cells) and store all row columns values (steps) in bool vector grid_Rows

template<class T>
void ofxSequencerRow<T>::store_Row_Values()
{
    for (int c = 0; c < values.size(); c++)
    {
        //TODO: cell is float..

        bool myVal;
        myVal = (bool) get_CellValue(c);//myVal = parameter->get();
        
        ofLogVerbose("ofxSequencer") <<  "store_Row_Values: c: " << c << " = " << myVal;
        
        //store in row
        grid_Rows[c] = myVal;
    }
}

//------------------

//mouse handlers

template<class T>
void ofxSequencerRow<T>::mousePressed(int col, int x, int y)
{
    pMouse.set(x, y);
    pValue = values[col];
}

template<class T>
void ofxSequencerRow<T>::mouseDragged(int col, int y)
{
    
}

template<>
inline void ofxSequencerRow<float>::mouseDragged(int col, int y)
{
    values[col] = ofClamp(pValue - 0.01 * (y - pMouse.y) * (parameter->getMax() - parameter->getMin()), parameter->getMin(), parameter->getMax());
}

template<>
inline void ofxSequencerRow<int>::mouseDragged(int col, int y)
{
    values[col] = ofClamp(pValue - 0.01 * (y - pMouse.y) * (parameter->getMax() - parameter->getMin()), parameter->getMin(), parameter->getMax());
}

//------------------

//drawing functions

template<class T>
void ofxSequencerRow<T>::draw(int col, int cellWidth, int cellHeight)
{
    float rectMult = 1.0 / (parameter->getMax() - parameter->getMin());
    ofDrawRectangle(0, 0,
           cellWidth  * rectMult * (values[col] - parameter->getMin()),
           cellHeight * rectMult * (values[col] - parameter->getMin()));
}

template<>
inline void ofxSequencerRow<bool>::draw(int col, int cellWidth, int cellHeight)
{
    if (values[col])
    {
        ofDrawRectangle(0, 0, cellWidth, cellHeight);
    }
}

//------------------

//mouse handlers

template<class T>
void ofxSequencerRow<T>::mouseReleased(int col)
{
    if (values[col] > 0.5 * (parameter->getMin() + parameter->getMax())){
        values[col] = parameter->getMin();
    }
    else {
        values[col] = parameter->getMax();
    }
}

//------------------

template<>
inline void ofxSequencerRow<bool>::mouseReleased(int col)
{
    values[col] = 1.0 - values[col];
}

//------------------

//rowBase getters

template<class T> T ofxSequencerRowBase::getMin()
{
    return dynamic_cast<ofxSequencerRow<T>&>(*this).getMin();
}

template<class T> T ofxSequencerRowBase::getMax()
{
    return dynamic_cast<ofxSequencerRow<T>&>(*this).getMax();
}

template<class T> T ofxSequencerRowBase::getValue()
{
    return dynamic_cast<ofxSequencerRow<T>&>(*this).getValue();
}

//------------------

//class methods

class ofxSequencer
{
public:
    ofxSequencer();
    ~ofxSequencer();
    
    void setup(int cols, int beatsPerMinute=120, int beatsPerBar=4);
    void setBpm(int beatsPerMinute, int beatsPerBar=4);
    void setSmooth(bool smooth) {this->smooth = smooth;}
    
    int getBpm() {return beatsPerMinute;}
    bool getSmooth() {return smooth;}
    
    //--
    
    template<class T>
    void addRow(ofParameter<T> * parameter);

    //--

    //TODO: BUG: play starts from step 1 instead of from 0 like expected!

    void start();

    void advance();

    //TODO: added transport methods
    //TODO: loop, one trig, loop and back...

    void stepBack();

    void stop();
    void reset();
    void randomize();

    //--

    void set_GridFromSequencer();
    void set_SequencerFromGrid();

    void DEBUG_All_GRID();//show store GRID vector
    
    //--
    
    void update();
    void draw();
    
    void setPosition(int x, int y, int width, int height);

	//TODO:
    void setVisible(bool visible);
	bool isVisible() {
		return bIsVisible;
	};
	bool bIsVisible = true;
    //void toggleVisible();

    void setMouseActive(bool active);
    
    //--
    
    template<class T>
    void setValue(int r, int c, T value) {
        ((ofxSequencerRow<T>*) rows[r])->setValue(c, value);
        toRedraw = true;
    }
    
    //--
    
    template<class T>
    bool get_Value(int r, int c) {
        bool myValue = ((ofxSequencerRow<T>*) rows[r])->get_Value(c);
        
        ofLogVerbose("ofxSequencer") << "-- get_Value " << "r:" << r << " c:" << c << " get_Value:" << myValue;
        return myValue;
    }
    
    int getColumn() {return column;}
    vector<ofxSequencerRowBase*> & getRows() {return rows;}
    
    //--
    
    ofEvent<int> beatEvent;
    
    //--

    //made public..
    float cursor;
    int column;

    //--

    //SEQUENCER DATA

    //DATA GRID STORAGE. DUPLICATED FROM ORIGINAL CLASS.

    vector < vector <bool> > grid;//all cols in all rows

    //TODO: take care with this grid cause is bool not int!

    //------------------
    
private:
    
    void play(void);
    void redraw();
    
    //-
    
    void mousePressed(ofMouseEventArgs &evt);
    void mouseDragged(ofMouseEventArgs &evt);
    void mouseReleased(ofMouseEventArgs &evt);
    
    //-
    
    vector<ofxSequencerRowBase*> rows;
    
    //-
    
    //float cursor;//go to public
    //int column;
    
    int beatsPerMinute;
    int bpmInterval;
    int bpmTime;
    bool smooth;
    ofxSequencerBpm bpm;

    //-
    
    int cols;
    int x, y, width, height;
    float cellWidth, cellHeight;
    
    ofPoint mCell;
    bool draggingCell;
    int draggingFrames;
    
    ofFbo fbo;
    bool toRedraw;

	//--
//
//	ofParameter<bool> loopBar;
//	ofParameter<int> numBars;
//	ofParameter<int> startBar;
//
//public:
//	void setLoopBarMode(bool b)
//	{
//		loopBar = b;
//	}
//	void setNumBarMode(int i)
//	{
//		numBars = i;
//	}
//	void setStartBar(int i)
//	{
//		startBar = i;
//	}
};

//------------------

template<class T>
void ofxSequencer::addRow(ofParameter<T> * parameter)
{
    ofxSequencerRow<T> *newRow = new ofxSequencerRow<T>(parameter, cols);
    rows.push_back(newRow);
    toRedraw = true;
    
    //--
    
    //erase all bools cols for any rows (?)

    vector <bool> myBools;

    for (int c = 0 ; c < cols; c++)
    {
        bool defState = false;
        myBools.push_back(defState);//define cell
    }

    grid.push_back(myBools); //create row
}

//------------------

