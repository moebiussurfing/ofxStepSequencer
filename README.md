# ofxStepSequencer

openFrameworks addon as a basic (bang) step-sequencer manager. Includes tap tempo, internal clock and external MIDI sync with pattern presets manager.
Mainly based on ofxSequencer from @genekogan, ofxMidi from @danomatika and ofxDawMetro from @castovoid among others.



## Screencast

![Alt text](/ofxStepSequencer.gif?raw=true "ofxStepSequencer.gif")



## Screenshots

![Alt text](/screenshot1.JPG?raw=true "screenshot1")
![Alt text](/screenshot2.JPG?raw=true "screenshot2")



## Usage

Create the example project or your own with OF ProjectGenerator as usual.

Recommended to look at the example.



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

//-

//update()
SEQ.update();

//-

//draw()
SEQ.draw();
SEQ.draw_CLOCK();

//-

//callback receiver from sequencer
void ofApp::Changed_SEQ_NOTES(ofAbstractParameter& e)
{
	...
}
```



## Features

- Internal beat-clock and external midi clock sync. (check ofxBeatClock info)
- Preset manager with 8 patterns presets.
- Helper tool to clone selected preset to the other next presets of your kit. 
- Use ctrl + mouse-click to clone preset to the clicked preset position.
- Helper tools to randomize patterns and clone one bar to all other 3.
- API methods to toggle hide/show gui and some others. Please check into the addon to know more.


## Requeriments

https://github.com/moebiussurfing/ofxBeatClock  
https://github.com/danomatika/ofxMidi  
https://github.com/castovoid/ofxDawMetro  
https://github.com/2bbb/ofxJsonUtils  
https://github.com/moebiussurfing/ofxGuiExtended2 (fork)

Included on the example but not mandatory:  
Optional to handle window settings between sessions:  
https://github.com/moebiussurfing/ofxWindowApp  



## Tested systems

- OF 0.11
- Visual Studio 2017
- macOS / HighSierra



## About

An addon by MoebiusSurfing, 2020.
Kind of beta state but I decided to make it public. Please post issues or problems to solve that I'll reply.  
Thanks to developers of all the other addons used, of course.  
Specially:  
@genekogan for https://github.com/genekogan/ofxSequencer  
@npisanti for https://github.com/npisanti/ofxGuiPresetSelector  
@castovoid for https://github.com/castovoid/ofxDawMetro  
@frauzufall for https://github.com/frauzufall/ofxGuiExtended  
@transat for GUI theme  



## TODO:

- Repair BUG original from ofxSequencer where sometimes mouse-clicks are ignored. Any help is welcome!  
- Allow resize pattern step sizes and note amount. (Now is fixed to 12 notes and 16 beats)  
- Improve ofxSequencer integration. I used a lot of workarounds to avoid all my big-skills-limitations. Any help is welcome!  
- Add not only bang/bool types like int or float as the original ofxSequencer can handle. Any help is welcome!  
- Some OSC in/out control.  
- Repair little BUG that create some rare xml files into presets folder.  


<br/>

**PLEASE FEEL FREE TO ADD MODIFICATIONS OR FEATURES AND TO SEND ME ISSUES/PULL REQUESTS. THANKS!**