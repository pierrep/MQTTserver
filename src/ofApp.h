#pragma once

#include "ofMain.h"
#include "ofxMosquitto.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#define HOST "localhost"
#define PORT 12345
#define PORT2 12346

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    ofxMosquitto* mosquitto;
    void onMosquittoConnect(int &rc);
    void onMosquittoMessageReceived(ofxMosquittoMessage &msg);
    
	string messageStr;
    bool messageArrived;
	ofxOscSender sender;
	ofxOscReceiver receiver;
	ofxXmlSettings	xml;
	string channel;
	string outchannel;
};
