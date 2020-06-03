
## ofxPresetDataGrid

This is an addon helper based on https://github.com/npisanti/ofxGuiPresetSelector.

Created to handle presets into my other addon:

https://github.com/moebiussurfing/ofxStepSequencer.

Thanks a lot to @npisanti, Nicola Pisanti, MIT License 2016.

Changes by MoebiusSurfing in this fork:

+ switched preset management from ofxGui panels to a custom data type used for my ofxStepSequencer.

+ switched from ofxGui/ofxGuiPanel to ofxGuiExtended.


## Required addons:

* ofxJsonUtils
* ofxGuiExtended2.


Added a custom class: "DataGrid.h / .cpp". 
It's a two dimentions bool array grid matrix step-sequencer-like:

>int grid [NUM_SEQ_NOTES][NUM_SEQ_BEATS];

