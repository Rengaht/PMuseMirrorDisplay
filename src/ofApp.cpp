#include "ofApp.h"
#include <locale>
#include <codecvt>


ofColor ofApp::MainColor[5]={ofColor(249,0,161),ofColor(255,115,0),ofColor(255,224,0),ofColor(22,226,5),ofColor(0,55,255)};

//--------------------------------------------------------------
void ofApp::setup(){

	cout<<"listening for osc messages on port "<<PORT<<"\n";
	_receiver.setup(PORT);

	_img_logo.loadImage("muse.png");
	_font.loadFont("NotoSansCJKtc-Regular.otf",60,true,true);

	_last_millis=ofGetElapsedTimeMillis();
	_dmillis=0;


	_timer_display=FrameTimer(3000);
	_timer_blink=FrameTimer(1000);
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
			_timer_display.update(_dmillis);
			if(_timer_display.val()==1) setMode(DisplayMode::SLEEP);
			break;	
	}
	

	
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetBackgroundColor(0);

	if(_mode==DisplayMode::POEM){
		/*int i=1;
		for(auto s:_poem){
			_font.drawString(s,0,i*80);
			i++;
		}*/
		_font.drawString(ws2s(_poem_str),0,80);

	}else{
		_img_logo.draw(0,0);
	}
}

void ofApp::updateOsc(){
	

	while(_receiver.hasWaitingMessages()){
		ofxOscMessage m;
		_receiver.getNextMessage(m);

		// check for mouse moved message
		if(m.getAddress() == "/poem"){

			_poem.clear();
			auto buffer_=m.getArgAsBlob(0);
			string str_=string(buffer_.getData());
			_poem_str=wstring(str_.begin(),str_.end());
			
			/*ofLog()<<"Got poem!";
			auto p_=ofSplitString(str_,"/");

			for(auto s:p_){
				_poem.push_back(s);
			}
			if(_poem.size()>0) */
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

string ofApp::ws2s(const wstring& wstr){
	static std::locale loc("");
	auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(wstr);
}
//
//wstring ofApp::s2ws(const string& str){
//	static std::locale loc("");
//	auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(str);
//}

string ofApp::cvtstr(const string& str){
	return str;
}