#include "ofApp.h"


ofColor ofApp::MainColor[5]={ofColor(249,0,161),ofColor(255,115,0),ofColor(255,224,0),ofColor(22,226,5),ofColor(0,55,255)};

//--------------------------------------------------------------
void ofApp::setup(){

	cout<<"listening for osc messages on port "<<PORT<<"\n";
	_receiver.setup(PORT);

	_img_logo.load("muse.png");
	_font.loadFont("NotoSansCJKtc-Regular.otf",200,true,true);

	_last_millis=ofGetElapsedTimeMillis();
	_dmillis=0;


	_timer_display=FrameTimer(3000);
	_timer_blink=FrameTimer(1000);
	
	//_postfx.init(ofGetWidth(),ofGetHeight());
	//_postfx.createPass<BloomPass>()->setEnabled(true);
	
	_shader_blurx.load("shadersES2/shaderBlurX");
	_shader_blury.load("shadersES2/shaderBlurY");
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
			for(auto& t:_timer_poem) t.update(_dmillis);

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
/*	_shader_glitch.begin();
	_shader_glitch.setUniform1f("amount",5);
	_shader_glitch.setUniform1f("angle",TWO_PI*sin(ofGetFrameNum()%30/30.0));
	_shader_glitch.setUniform1f("windowWidth",ofGetWidth());
	_shader_glitch.setUniform1f("windowHeight",ofGetHeight());
	_shader_glitch.setUniformTexture("tex0",_fbo1.getTexture(),0);
*/	if(_mode==DisplayMode::POEM){
		int len=_poem.size();
		int h_=0;
		float hei_=_font.getLineHeight();
		for(int i=0;i<len;++i){
			ofPushStyle();
			ofSetColor(255,255*_timer_poem[i].val()*(1-_timer_fade.val()));

			ofPushMatrix();
			ofTranslate(_poem_offset[i],h_);
			ofScale(_poem_size[i]/hei_,_poem_size[i]/hei_);
				_font.drawStringAsShapes(_poem[i],0,hei_);
			ofPopMatrix();

			ofPopStyle();

			h_+=_poem_size[i];
		}

	}else{
		/*for(int i=0;i<100;++i){
			ofSetColor(ofRandom(255),255,255);
			ofDrawCircle(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()),20);
		}
		_img_logo.draw(0,0,ofGetWidth(),ofGetHeight());*/
	}
//	_shader_glitch.end();

	_fbo_tmp.end();
	
	_fbo2.begin();
	ofClear(0);
		_shader_blurx.begin();
		//_shader_blurx.setUniform1f("blurAmnt",blur_/(float)ofGetHeight());
		_shader_blurx.setUniform1f("windowWidth",ofGetWidth());
		_shader_blurx.setUniform1f("windowHeight",ofGetHeight());
		_shader_blurx.setUniformTexture("tex0",_fbo_tmp.getTexture(),0);
			_fbo_tmp.draw(0,0);
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
	_fbo2.begin();
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
	

		
	_shader_glitch.begin();
	_shader_glitch.setUniform1f("amount",3);
	_shader_glitch.setUniform1f("angle",TWO_PI*sin(ofGetFrameNum()%120/120.0+ofRandom(-2,2)));
	_shader_glitch.setUniform1f("windowWidth",ofGetWidth());
	_shader_glitch.setUniform1f("windowHeight",ofGetHeight());
	_shader_glitch.setUniformTexture("tex0",_fbo1.getTexture(),0);
	_fbo1.draw(0,0);
	//_fbo_tmp.draw(0,0);
	_shader_glitch.end();

	ofDrawBitmapString(ofToString(ofGetFrameRate()),0,20);
}

void ofApp::updateOsc(){
	

	while(_receiver.hasWaitingMessages()){
		ofxOscMessage m;
		_receiver.getNextMessage(m);
		// check for mouse moved message
		if(m.getAddress() == "/poem"){

			_poem.clear();
			_timer_poem.clear();
			_poem_size.clear();

			_poem_str=m.getArgAsString(0);
			int delay_=m.getArgAsInt(1);
			int show_=m.getArgAsInt(2);
	

			auto p_=ofSplitString(_poem_str,"/");
			int len=p_.size();
			ofLog()<<"len=  "<<len;
			if(len<1) continue;

			float t=0;
			float inter_=show_*.5/len;	
			float h=0;
			//float hei_=ofGetHeight()/len;
			float w=ofGetWidth();

			for(int i=0;i<len;++i){

				auto rec_=_font.getStringBoundingBox(p_[i],0,0);
				float hei_=w/rec_.width*rec_.height;

				float d_=ofRandom(-.2,.2)*inter_;
				float in_=min(ofRandom(.7,1.5)*inter_,show_-t);

				float line_=(i==len-1)?ofGetHeight()-h:ofRandom(.5,1.2)*hei_;				
				
				_poem.push_back(p_[i]);
				_timer_poem.push_back(FrameTimer(in_,delay_+t+d_));
				_poem_size.push_back(line_);
				
				float s_=line_/rec_.height*rec_.width;
				_poem_offset.push_back(ofRandom(-0.5*line_,w-s_+0.5*line_));
				//ofLog()<<in_<<" "<<delay_+t+d_<<" "<<line_;

				t+=in_+d_;
				h+=line_;
			}
			_timer_display=FrameTimer(t,delay_);
			_timer_fade=FrameTimer(5000,show_*.5+t+delay_);

			ofLog()<<"get poem: "<<delay_<<" "<<show_;
			setMode(DisplayMode::POEM); 
		}
	}

}

void ofApp::setMode(DisplayMode set_){
	_mode=set_;
	switch(_mode){
		case SLEEP:
			_timer_blink.restart();
			break;
		case POEM:
			_timer_display.restart();
			_timer_fade.restart();
			for(auto& t:_timer_poem) t.restart();
			break;
	}

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

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
