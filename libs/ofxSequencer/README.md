# ofxSequencer

[FORK] addon for OpenFrameworks for creating an auto-updating sequencer. a couple of features:

## MODIFICATIONS:
- I made this fork to use into: https://github.com/moebiussurfing/ofxStepSequencer. Feel free to check for it!
- Added some methods to get and set values from the grid to store/load presets.
- It only handles bool types.
- A lot of improvements could be made because of my C++ skills absence. It has been hard for me to add these features. I am sure that my changes are not well done or could be better done. Sorry!

## TODO:
- Allow resize pattern step sizes and note amount. (Now is 12 notes and 16 beats)
- Improve ofxSequencer integration. I used a lot of workarounds to avoid all my big-skills-limitations. Any help is welcome!
- Add not only bang/bool types like int or float as the original ofxSequencer can handle. Any collaboration on this will be welcome!

//------------------------------------------------------------------------------------------------------------

 - can be discrete (all cells 1 or 0) or continuously valued.
 - interacts with the mouse. drag the mouse over a cell to change the value, or click it to snap to 0/1
 - updates automatically, in its own thread, sends notifier to your app (based on ofxBpm)

requires [ofxBpm](https://github.com/mirrorboy714/ofxBpm).