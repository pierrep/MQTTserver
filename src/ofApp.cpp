#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	xml.loadFile("settings.xml");
	string host_ip = xml.getValue("NETWORK:IP","23.239.4.79");
	int port = xml.getValue("NETWORK:PORT",1883);
	channel = xml.getValue("NETWORK:CHANNEL","ap/toPlatform/cr");
	outchannel = xml.getValue("NETWORK:OUTCHANNEL","ap/fromPlatform/cr");
	string name = xml.getValue("NETWORK:NAME","CaseStudyLibrary");

	ofLogNotice("ofxMosquitto") << "Host IP: " << host_ip << " Port: " << port;
	
	mosquitto = new ofxMosquitto("MQTT_Client_"+name,host_ip,port,true);
	//mosquitto->getVersion();
	ofLogNotice("ofxMosquitto") << "Client ID = " << mosquitto->getClientID();
	ofLogNotice("ofxMosquitto") << "Channel = " << channel;
	mosquitto->setAutoReconnect(true);
   //mosquitto->setup(host_ip, port);

    ofAddListener(mosquitto->onConnect, this, &ofApp::onMosquittoConnect);

	mosquitto->connect();
	
	/* OSC */
	sender.setup(HOST, PORT);
	receiver.setup(PORT2);

	messageArrived = false;
}

//--------------------------------------------------------------
void ofApp::update()
{
	if(messageArrived) {
		messageArrived = false;

		ofxOscMessage m;
		m.setAddress("/mqtt");
		m.addStringArg(messageStr);
		sender.sendMessage(m);
	}

	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
		if(m.getAddress() == "/resources"){
			int resid = m.getArgAsInt32(0);
			stringstream msgstr;
			int gid = m.getArgAsInt32(1);
			msgstr << "{\"" << "group_id\":" << gid << ", " << "\"resource_ids\": [" << resid << "] }";
			string payload = msgstr.str();
			cout << "payload=" << payload << endl;
			mosquitto->publish(NULL,outchannel,payload);
		}
	
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0, 0, 0);
    ofSetColor(255);
    ofDrawBitmapString(messageStr, 10, 20);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	
    mosquitto->disconnect();
}

//--------------------------------------------------------------
void ofApp::onMosquittoConnect(int &rc)
{
    if (MOSQ_ERR_SUCCESS == rc)
    {
        mosquitto->subscribe(NULL, channel);
        ofAddListener(mosquitto->onMessage, this, &ofApp::onMosquittoMessageReceived);
		ofLog() << "subscribed to " << channel;
    }
}

//--------------------------------------------------------------
void ofApp::onMosquittoMessageReceived(ofxMosquittoMessage &msg)
{
    if (msg.payloadlen)
    {
        messageStr = msg.payloadAsString();
		messageArrived = true;
		ofLog() << "msg received:" << messageStr;
    }
}
