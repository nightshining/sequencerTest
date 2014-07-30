#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"
#include "piano.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void audioIn(float * input, int bufferSize, int nChannels);
    
    ofSoundStream soundStream;
		
    ofxAudioUnit bandpass, delay;
    ofxAudioUnitTap srcTap, bpTap, dlyTap;
    
    ofPolyline waveform1, waveform2, waveform3;
    
    float time, feedback, centerFreq, bandWidth;
    
    //sampler/synth signal chain
    ofxAudioUnitSampler sampler;
    ofxAudioUnitTap sTap;
    ofPolyline sWaveform;
    int midiNote, prog;
    bool notePlaying;
    float cutoff, resonance; //sampler para
    
    //sample/drums
    ofxAudioUnitSampler drumSampler;
    ofxAudioUnitTap drumTap;
    ofPolyline drumWaveform;

    
    //writing out the classes again is just for easy to read code
    ofxAudioUnitMixer mixer;
    ofxAudioUnitOutput output;
    
    piano myPiano;
    
    //sequencing
    int duration, alarmMS;
    int duration2, alarmMS2;
    
    vector<int> majorScale; 

};
