#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxXmlSettings.h"
#include "FrameTimer.h"

#define PORT 12345

class ofApp : public ofBaseApp{

	public:
		static ofColor MainColor[5];

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void updateOsc();
		
		//ofShader _shader_glitch;
		ofImage _img_logo;
		ofxOscReceiver _receiver;

		vector<string> _poem;
   		vector<FrameTimer> _timer_poem;
		vector<float> _poem_size;
		vector<float> _poem_offset;

		string _poem_str;

		ofxTrueTypeFontUC _font;

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
};
