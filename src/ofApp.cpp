#include "ofApp.h"


ofColor ofApp::MainColor[5]={ofColor(249,0,161),ofColor(255,115,0),ofColor(255,224,0),ofColor(22,226,5),ofColor(0,55,255)};
ofxTrueTypeFontUC PPoem::PoemFont;

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowShape(1280,720);
	ofHideCursor();

	loadXmlSetting();

	cout<<"listening for osc messages on port "<<PORT<<"\n";
	_receiver.setup(PORT);

	_img_logo.load("muse.png");
	PPoem::PoemFont.loadFont("NotoSansCJKtc-Regular.otf",200,true,true);

	_last_millis=ofGetElapsedTimeMillis();
	_dmillis=0;


	_timer_display=FrameTimer(3000);
	_timer_blink=FrameTimer(1000);
	
	//_postfx.init(ofGetWidth(),ofGetHeight());
	//_postfx.createPass<BloomPass>()->setEnabled(true);
	
	_shader_blurx.load("shadersES2/shaderBlurX");
	
	//_shader_blury.load("shadersES2/shaderBlurY");
	_shader_glitch.load("shadersES2/glitch");
	
	_fbo_tmp.allocate(ofGetWidth(),ofGetHeight());
	_fbo1.allocate(ofGetWidth(),ofGetHeight());
	_fbo2.allocate(ofGetWidth(),ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update(){
	
	_dmillis=ofGetElapsedTimeMillis()-_last_millis;
	_last_millis+=_dmillis;

	switch(_mode){
		case SLEEP:
			updateOsc();
			_timer_blink.update(_dmillis);
			break;
		case POEM:
			for(auto& t:_poem) t.update(_dmillis);

			_timer_display.update(_dmillis);
			_timer_fade.update(_dmillis);
			if(_timer_fade.val()==1) setMode(DisplayMode::SLEEP);
			break;	
	}
	

	
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetBackgroundColor(0);

	if(_mode==DisplayMode::SLEEP) return;

	_fbo_tmp.begin();
	ofClear(0);
	//ofSetBackgroundColor(0);

	if(_mode==DisplayMode::POEM){
		//int len=_poem.size();
		float fadea_=1-_timer_fade.val();
		for(auto& p:_poem){
			p.draw(fadea_);
		}

	}else{

	}
	_fbo_tmp.end();

	_fbo1.begin();
	ofClear(0);
	_shader_glitch.begin();
	_shader_glitch.setUniform1f("amount",3);
	_shader_glitch.setUniform1f("phi",ofRandom(-100,100));
	_shader_glitch.setUniform1f("angle",PI*sin(ofGetFrameNum()%40/40+((ofRandom(20)<1)?0:ofRandom(-10,10))));
	_shader_glitch.setUniform1f("windowWidth",ofGetWidth());
	_shader_glitch.setUniform1f("windowHeight",ofGetHeight());
	_shader_glitch.setUniformTexture("tex0",_fbo_tmp.getTexture(),0);
	_shader_glitch.setUniform3f("glitchColor1",MainColor[_color_glitch1].r,MainColor[_color_glitch1].g,MainColor[_color_glitch1].b);
	_shader_glitch.setUniform3f("glitchColor2",MainColor[_color_glitch2].r,MainColor[_color_glitch2].g,MainColor[_color_glitch2].b);
		_fbo_tmp.draw(0,0);
	_shader_glitch.end();
	_fbo1.end();

	_fbo2.begin();
	ofClear(0);
		_shader_blurx.begin();
		//_shader_blurx.setUniform1f("blurAmnt",blur_/(float)ofGetHeight());
		_shader_blurx.setUniform1f("windowWidth",ofGetWidth());
		_shader_blurx.setUniform1f("windowHeight",ofGetHeight());
		_shader_blurx.setUniformTexture("tex0",_fbo1.getTexture(),0);
			_fbo1.draw(0,0);
		_shader_blurx.end();
	_fbo2.end();
	_fbo1.begin();
	ofClear(0);
		_shader_blurx.begin();
		//_shader_blurx.setUniform1f("blurAmnt",blur_/(float)ofGetWidth());
		//_shader_blurx.setUniform1f("windowWidth",ofGetWidth());
		//_shader_blurx.setUniform1f("windowHeight",ofGetHeight());
		_shader_blurx.setUniformTexture("tex0",_fbo2.getTexture(),0);
			_fbo2.draw(0,0);
		_shader_blurx.end();
	_fbo1.end();
/*	_fbo2.begin();
	ofClear(0);
		_shader_blurx.begin();
		//_shader_blurx.setUniform1f("blurAmnt",blur_/(float)ofGetHeight());
		_shader_blurx.setUniformTexture("tex0",_fbo1.getTexture(),0);
			_fbo1.draw(0,0);
		_shader_blurx.end();
	_fbo2.end();
	_fbo1.begin();
	ofClear(0);
		_shader_blurx.begin();
		//_shader_blurx.setUniform1f("blurAmnt",blur_/(float)ofGetHeight());
		_shader_blurx.setUniformTexture("tex0",_fbo2.getTexture(),0);
			_fbo2.draw(0,0);
		_shader_blurx.end();
	_fbo1.end();
*/	

		
	_fbo1.draw(0,0);
//	_fbo_tmp.draw(0,0,ofGetWidth()/4,ofGetHeight()/4);
//	_fbo2.draw(0,ofGetHeight()/4,ofGetWidth()/4,ofGetHeight()/4);
	
	ofDrawBitmapString(ofToString(ofGetFrameRate()),0,20);
	ofDrawBitmapString(ofToString(_timer_fade.val()),0,40);

}

void ofApp::updateOsc(){
	

	while(_receiver.hasWaitingMessages()){
		ofxOscMessage m;
		_receiver.getNextMessage(m);
		// check for mouse moved message
		if(m.getAddress() == "/poem"){

			_poem.clear();
			auto b=m.getArgAsBlob(0);
			//_poem_str=m.getArgAsString(0);
			_poem_str=string(b.getData());

			int delay_=m.getArgAsInt(1);
			int fadein_=m.getArgAsInt(2);
			int show_=m.getArgAsInt(3);
			int fadeout_=m.getArgAsInt(4);

			_color_glitch1=m.getArgAsInt(5);
			_color_glitch2=m.getArgAsInt(6);

			auto p_=ofSplitString(_poem_str,"|");
			int len=p_.size()-1;
			ofLog()<<"len=  "<<len;
			if(len<1) continue;

			float t=0;
			float inter_=fadein_/len;
			float h=0;
			//float hei_=ofGetHeight()/len;
			float w=ofGetWidth();

			for(int i=0;i<len;++i){

				float hei=(i==len-1)?ofGetHeight()-h:ofRandom(.5,1.3)*ofGetHeight()/len;
				float d_=ofRandom(-.2,.2)*inter_;
				float in_=min(ofRandom(.7,1.5)*inter_,fadein_-t);

				float dd_=ofClamp(t+d_,0,fadein_);
				_poem.push_back(PPoem(p_[i],h,hei,delay_+dd_,in_));
				//_poem.push_back(PPoem(p_[i],ofGetHeight()/len*i,ofGetHeight()/len,0,1000));


				t+=in_+d_;
				h+=hei;
			}
			//_timer_display=FrameTimer(t,delay_+fadein_);
			_timer_fade=FrameTimer(fadeout_,show_+fadein_+delay_);


			ofLog()<<"get poem: "<<delay_<<" "<<fadein_<<" "<<show_<<" "<<fadeout_;
			setMode(DisplayMode::POEM); 
		}
	}

}

void ofApp::setMode(DisplayMode set_){
	_mode=set_;
	switch(_mode){
		case SLEEP:
			_timer_blink.restart();
			sendReset();
			break;
		case POEM:
			_timer_display.restart();
			_timer_fade.restart();
			for(auto& t:_poem) t.restart();
			break;
	}

}

void ofApp::randomGlitch(){
	_color_glitch1=floor(ofRandom(5));
	_color_glitch2=fmod(_color_glitch1+floor(ofRandom(4)),5);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'r':
		case 'R':
			setMode(DisplayMode::SLEEP);
			break;
	}
}

void ofApp::loadXmlSetting(){
	ofxXmlSettings param_;
	param_.loadFile("Pdata.xml");
	_str_ip.push_back(param_.getValue("IP_FACE",""));
	_str_ip.push_back(param_.getValue("IP_POEM",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY1",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY2",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY3",""));
	_index_display=param_.getValue("INDEX_DISPLAY",0);
}

void ofApp::sendReset(){
	
	if(_index_display>0) return;
	
	ofLog()<<"send reset!";
	for(int i=0;i<2;++i){
		ofxOscSender sender;
		sender.setup(_str_ip[0],PORT);
	
		ofxOscMessage message;
		message.setAddress("/reset");
		message.addIntArg(_index_display);
		sender.sendMessage(message);
	}
}

//string ofApp::ws2s(const wstring& wstr){
//	static std::locale loc("");
//	auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstr);
//}
//
//wstring ofApp::s2ws(const string& str){
//	static std::locale loc("");
//	auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(str);
//}

string ofApp::cvtstr(const string& str){
	return str;
}
