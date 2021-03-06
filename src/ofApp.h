#pragma once

#include "ofMain.h"
//#include "Detector.h"
//#include "FrameDetector.h"

#include "mutex"

#include "ProcessStatusListener.h"
#include "AffdexException.h"
#include "Detector.h"
#include "Face.h"
#include "FaceListener.h"
#include "Frame.h"
#include "CameraDetector.h"
#include "ImageListener.h"
#include "PhotoDetector.h"
#include "ProcessStatusListener.h"
#include "typedefs.h"


//#include "ofxFPSCamera.h"
//#include "ofxOpenCv.h"
//#include "ofxCvHaarFinder.h"

///#include "ofxOpenCv.h"

using namespace affdex;
using namespace std;

class ofApp : public ofBaseApp, public affdex::ImageListener{
	public:
        ~ofApp();
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        string mapExpressionName(string expression);
        void pavlokEvent(int player_no);
        void startNewGame();
    
        shared_ptr<CameraDetector> detector;
        shared_ptr<ImageListener> listenPtr;
        shared_ptr<byte> imgdata;
        std::mutex imgdataMutex;
    
        void onImageResults(std::map<FaceId,Face> faces, Frame image);
        void onImageCapture(Frame image);
    
        ofTrueTypeFont myfont;
        ofTrueTypeFont myfontLarge;
        ofTrueTypeFont myfontXLarge;
        string x;
        int smile_value;
        int brow_value;
        int frown_value;
        int smirk_value;
        int mouth_value;
        int noofframes;
        ofImage currentFrame;
        int imgWidth;
        int imgHeight;
    
        vector<float> nums;
        vector<float> nums_smoothed;
	
        std::vector< bool > player1_objective;
        std::vector< bool > player2_objective;
        std::vector< std::string> expression_objective;
        int player1_index = -1;
        int player2_index = -1;

        std::map< std::string, ofImage > expression_image_lookup;
    
        int windowWidth;
        int windowHeight;
    
    bool endGame;
    int winning_player;
    
    ofImage replay_button;
    ofImage shock_button;
    
    ofImage     screenshot_img;
    
};
