# ofxStepSequencer

openFrameworks addon basic bang step-sequencer with tap tempo, external MIDI sync and pattern presets.



## Screenshots

![Alt text](/ofxStepSequencer.gif?raw=true "ofxStepSequencer.gif")
![Alt text](/screenshot1.JPG?raw=true "screenshot1")
![Alt text](/screenshot2.JPG?raw=true "screenshot2")
![Alt text](/screenshot3.JPG?raw=true "screenshot3")



## Usage

Create your project with OF ProjectGenerator as usual.
Recommended to look example.

```c++
ofApp.h:

#include "ofxStepSequencer.h"

ofxStepSequencer SEQ;

//local notes_params to receive bang/bools trigged from sequencer
ofParameter<bool> notes_params[NUM_SEQ_NOTES];

//callback listener
void Changed_SEQ_NOTES(ofAbstractParameter& e);

//--

ofApp.cpp:

//setup()
SEQ.setup();

//callbacks
ofAddListener(SEQ.TARGET_NOTES_paramsGroup.parameterChangedE(), this, &ofApp::Changed_SEQ_NOTES);

//update()
SEQ.update();

//draw()
SEQ.draw();
SEQ.draw_CLOCK();
```



## Features

- internal beat-clock and external midi clock sync. (check ofxBeatClock info)
- 8 pattern presets.
- helper tool to clone selected preset to the other presets. 
- Use ctrl + mouse click to clone preset to another one.
- helper tools to randomize patterns and clone one bar to the all other 3.



## Requeriments

https://github.com/moebiussurfing/ofxSequencer (fork)
https://github.com/moebiussurfing/ofxPresetDataGrid
https://github.com/moebiussurfing/ofxBeatClock
https://github.com/2bbb/ofxJsonUtils
https://github.com/danomatika/ofxMidi
https://github.com/castovoid/ofxDawMetro
https://github.com/moebiussurfing/ofxGuiExtended2 (fork)



## Tested system

- OF 0.11
- Visual Studio 2017
- macOS / HighSierra



## About

An addon by MoebiusSurfing.
Thanks to developers of all the other addons used, of course.
Specially:
@genekogan for https://github.com/genekogan/ofxSequencer
@npisanti for https://github.com/npisanti/ofxGuiPresetSelector 
@castovoid for https://github.com/castovoid/ofxDawMetro
@frauzufall for https://github.com/frauzufall/ofxGuiExtended
@transat for gui theme



## TODO:

- allow resize pattern step sizes and note amount. (Now is 12 notes and 16 beats)
- improve ofxSequencer integration. I used a lot of workarounds to avoid all my big-skills-limitations. Any help is welcome!
- add not only bang/bool types as int or float as the original ofxSequencer can handle. Any collaboration on this will be welcome!
- some OSC in/out control.


PLEASE FEEL FREE TO ADD MODIFICATIONS OR FEATURES AND TO SEND ME PULL REQUESTS. THANKS!
