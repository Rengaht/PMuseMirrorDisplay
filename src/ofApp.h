#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxXmlSettings.h"
#include "FrameTimer.h"
#include "PPoem.h"

#define PORT 12345

class ofApp : public ofBaseApp{

	public:
		static ofColor MainColor[5];

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

		void updateOsc();
		void sendReset();		
		//ofShader _shader_glitch;
		ofImage _img_logo;
		ofxOscReceiver _receiver;

		/*vector<string> _poem;
   		vector<FrameTimer> _timer_poem;
		vector<float> _poem_scale;
		vector<ofVec2f> _poem_offset;
		vector<float> _line_height;*/
		list<PPoem> _poem;

		string _poem_str;


		string cvtstr(const string& str);

		/*string ws2s(const wstring& wstr);*/
		/*wstring s2ws(const string& str);*/

		float _last_millis;
		float _dmillis;

		FrameTimer _timer_display,_timer_fade;
		FrameTimer _timer_blink;

		enum DisplayMode {SLEEP, POEM};
		DisplayMode _mode;
		void setMode(DisplayMode set_);

		//ofxPostProcessing _postfx;
		//ofEasyCam _camera;
		ofShader _shader_blurx,_shader_blury,_shader_glitch;
		ofFbo _fbo1,_fbo2,_fbo_tmp;

		int _color_glitch1,_color_glitch2;
		void randomGlitch();

		int _index_display;
		vector<string> _str_ip;
		void loadXmlSetting();
};
