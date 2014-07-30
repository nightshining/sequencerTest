#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    soundStream.setup(this, 0, 2, 44100, 1, 4); //hack to get timing right
    soundStream.start();
    
    //sequencing
    duration = 1000; //ms
    alarmMS = ofGetElapsedTimeMillis() + duration;
    
    //sequencing
    duration2 = 250; //ms
    alarmMS2 = ofGetElapsedTimeMillis() + duration2;
    
    bandpass = ofxAudioUnit('aufx', 'bpas', 'appl');
    bandpass.printParameterList();
    
    delay = ofxAudioUnit('aufx', 'dely', 'appl');
    delay.printParameterList();

    //sampler/synth
    sampler = ofxAudioUnitSampler('aumu', 'aut1', 'Alfa'); 
    sampler.loadCustomPresetAtPath(ofFilePath::getAbsolutePath("SK Pad PedShop.aupreset"));
    sampler.printParameterList();
    sampler.showUI();
    
    drumSampler = ofxAudioUnitSampler('aumu', 'dls ', 'appl');
    drumSampler.setChannel(10);
    drumSampler.setBank(120, 0);
    drumSampler.setProgram(0);
    drumSampler.showUI();
    
    //Mixer
    mixer.setInputBusCount(2);
    drumSampler.connectTo(mixer, 0);
    sampler.connectTo(mixer, 1);
    
    mixer.connectTo(output);

    output.start();

    midiNote = 0;
    prog = 0;
    notePlaying = false;
    
    myPiano.setup();
    
    //scale
    int scaleDegrees[7] = { 0, 2, 3, 5, 7, 8, 11 };
    majorScale.assign(scaleDegrees, scaleDegrees + 7);
    
    
    for (int i = 0; i < majorScale.size(); i++) {
        
        cout << "scale degree: " << i << majorScale[i] << endl;
        
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    srcTap.getLeftWaveform(waveform1, ofGetWidth() / 3, ofGetHeight());
    bpTap.getLeftWaveform(waveform2, ofGetWidth() / 3, ofGetHeight());
    dlyTap.getLeftWaveform(waveform3, ofGetWidth() / 3, ofGetHeight());
   
    //sampler wave
    sTap.getLeftWaveform(sWaveform, ofGetWidth(), ofGetHeight());
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackgroundGradient(ofColor::gray, ofColor::whiteSmoke, OF_GRADIENT_CIRCULAR);
    
    //wave1
    ofSetColor(ofColor::crimson);
    waveform1.draw();
    
    //wave2
    ofSetColor(ofColor::teal);
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 3, 0);
    waveform2.draw();
    ofPopMatrix();
    
    //wave3
    ofSetColor(ofColor::whiteSmoke);
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 3 * 2, 0);
    waveform3.draw();
    ofPopMatrix();
    
    //sampler Wave
    ofSetColor(ofColor::whiteSmoke);
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 3 * 2, 0);
    sWaveform.draw();
    ofPopMatrix();
    
    ofSetColor(ofColor::crimson);

    ofDrawBitmapString("BandPass: Center Frequency: " + ofToString(centerFreq) + "\nBandPass: Band Width: " + ofToString(bandWidth) + "\nDelay: Time: " + ofToString(time) + "\nDelay: Feedback: " + ofToString(feedback), 20, 50);
    
    //ofToString is like COUT on screen!
    
    myPiano.draw();
    
}


void testApp::audioIn(float * input, int bufferSize, int nChannels) {
    
    //sequencing
    if (ofGetElapsedTimeMillis() > alarmMS) {
        int difference = alarmMS - ofGetElapsedTimeMillis();
        int midiDrumNote = majorScale[ofRandom(0, 6)];
        int vel = ofRandom(100, 127);
        sampler.midiNoteOn(midiDrumNote, vel);
        alarmMS = ofGetElapsedTimeMillis() + duration;
        
    }
    
    /*
    //sequencing 2
    if (ofGetElapsedTimeMillis() > alarmMS2) {
        int difference = alarmMS2 - ofGetElapsedTimeMillis();
        int midiDrumNote = ofRandom(35, 81);
        int vel = ofRandom(20, 80);
        drumSampler.midiNoteOn(midiDrumNote, vel);
        alarmMS2 = ofGetElapsedTimeMillis() + duration2;
        
    }
    */
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    myPiano.keyPressed(key);
    
    if (!notePlaying) //! means NOT 
    {
        
    notePlaying = true;
    midiNote = myPiano.getNote(key);
    sampler.midiNoteOn(midiNote, 80);
    
    
    }
    
    switch (key) {
        
        case 'b':
            bandpass.showUI();
            break;
        case 'd':
            delay.showUI();
            break;
        case ' ':
            sampler.showUI();
            break;
        case OF_KEY_LEFT:
            if (prog > 0) prog--;
            sampler.setProgram(prog);
            break;
        case OF_KEY_RIGHT:
            if (prog < 127) prog++;
            sampler.setProgram(prog);
            break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
    myPiano.keyReleased(key);
    
    if (notePlaying) {
        
    sampler.midiNoteOff(midiNote, 0); // note, vel
    notePlaying = false;    
    
    }

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
    centerFreq = ofMap(x, 0, ofGetWidth(), 20, 22050, true); 
    bandWidth = ofMap(y, 0, ofGetHeight(), 100, 12000, true);
    //TRUE says Clamp YES nothing below 20 nothing above 22050
    
    bandpass.setParameter(kBandpassParam_CenterFrequency, kAudioUnitScope_Global, centerFreq);
    //1 says which para to effect, 2 globally, 3 values to send in
    bandpass.setParameter(kBandpassParam_Bandwidth, kAudioUnitScope_Global, bandWidth);
    
    //SAMPLER PARA
    cutoff = ofMap(x, 0, ofGetWidth(), 0.0, 1.0, true);
    resonance = ofMap(y, 0, ofGetHeight(), 0.0, 0.7, true);
    
    sampler.setParameter(57, kAudioUnitScope_Global, cutoff);
    sampler.setParameter(58, kAudioUnitScope_Global, resonance);
    

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    time = ofMap(x, 0, ofGetWidth(), 0, 2, true); 
    feedback = ofMap(y, 0, ofGetHeight(), -100, 100, true);
    
    delay.setParameter(kDelayParam_DelayTime, kAudioUnitScope_Global, time);
    //1 says which para to effect, 2 globally, 3 values to send in
    delay.setParameter(kDelayParam_Feedback, kAudioUnitScope_Global, feedback);
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    int midiDrumNote = ofRandom(35, 81);
    drumSampler.midiNoteOn(midiDrumNote, 90);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
