#include "ofApp.h"

#include <iostream>

#include "ProcessStatusListener.h"
#include "AffdexException.h"
#include "Detector.h"
#include "Face.h"
#include "FaceListener.h"
#include "Frame.h"
//#include "FrameDetector.h"
#include "CameraDetector.h"
#include "ImageListener.h"
#include "PhotoDetector.h"
#include "ProcessStatusListener.h"
#include "typedefs.h"

//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

int index2 = 0;

using namespace affdex;
using namespace std;
using namespace cv;

float last_timestamp = -1.0f;
float capture_fps = -1.0f;
float process_last_timestamp = -1.0f;
float process_fps = -1.0f;

auto start_time = std::chrono::system_clock::now();

float head_motion = -1.0f;
float temp_x = -1.0f;

bool Player_1_objective;
bool Player_2_objective;

std::vector<std::string> expressions{"smile", "innerBrowRaise", "browRaise", "browFurrow" , "noseWrinkle",
    "upperLipRaise", "lipCornerDepressor", "chinRaise", "lipPucker", "lipPress",
    "lipSuck", "mouthOpen", "smirk", "eyeClosure"};

std::vector<std::string> emotions{"joy", "fear", "disgust", "sadness", "anger", "surprise", "contempt","valence", "engagement" };
//

Face f;
int no_faces;

int vocals_count = 0;


std::map<FaceId,Face> facesMap;

void ofApp::onImageResults(std::map<FaceId,Face> faces, Frame image){

    int facesSize = faces.size();
    
    facesMap = faces;
    no_faces = facesSize;
    imgWidth = image.getWidth();
    imgHeight = image.getHeight();
    imgdataMutex.lock();
    imgdata = image.getBGRByteArray();
    imgdataMutex.unlock();
    int cnt=0;
    
    

}

void ofApp::onImageCapture(Frame image){
    
}



//--------------------------------------------------------------
void ofApp::setup(){
    
    myfont.load("Helvetica.ttf", 12);
    myfontLarge.load("Helvetica.ttf", 32);
    //ofSetDataPathRoot("../Resources/data/");
    
    try
    {
        std::string affdexLicense = "{\"token\":\"fb33ef9bfb195dac7a5c0b286633084df80974ecc9b36aae39ca3ba56921f815\",\"licensor\":\"Affectiva Inc.\",\"expires\":\"2099-01-01\",\"developerId\":\"Affectiva-internal\",\"software\":\"Affdex SDK\"}";

        //path DATA_FOLDER = "/Users/dmcduff/src/affdexface/data";
        
        path DATA_FOLDER = "/Users/dmcduff/src/of_v20150910_osx_release/apps/myApps/affectiva-hackathon_rev02/bin/data/affdex-windows-sdk-iOS/data";

        const std::vector<int> DEFAULT_RESOLUTION{ 640, 480 };
        std::vector<int> resolution;
        resolution = { 640, 480 };
        int process_framerate = 30;
        int camera_framerate = 15;
        int camera_id = 0;
        
        const int BUFFER_LENGTH = 2;
        const int CAMERA_ID = 0;
        const int CAMERA_FPS = 30;
        const int PROCESS_FPS = 30;
        const int MAX_FACES = 10;
        detector = make_shared<CameraDetector>(CAMERA_ID, CAMERA_FPS, PROCESS_FPS, MAX_FACES);


        detector->setClassifierPath(DATA_FOLDER);
        detector->setLicenseString(affdexLicense.c_str());
        
        detector->setDetectAllEmotions(true);
        detector->setDetectAllExpressions(true);

        
        detector->setImageListener(this);
        detector->start();
        
    }
    catch(AffdexException exception)
    {
        std::cerr << "An Exception occurred: " << exception.getExceptionMessage() << std::endl;
    }
    
    

    
}

//--------------------------------------------------------------
void ofApp::update(){


    std::cout << "Framerate: " << capture_fps << std::endl;

    
    ofSoundUpdate();

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    imgdataMutex.lock();
    currentFrame.setFromPixels(imgdata.get(), imgWidth, imgHeight, ofImageType::OF_IMAGE_COLOR, false);
    imgdataMutex.unlock();
    
    int windowWidth =  ofGetWidth();
    float widthRecal = float(imgWidth)/float(windowWidth);
    int windowHeight = ofGetHeight();
    float heightRecal = float(imgHeight)/float(windowHeight);
    
    currentFrame.resize(windowWidth,windowHeight);
    currentFrame.draw(0,0);
    
    ofSetColor(255);
    ofFill();
    ofRect(windowWidth/2, 0,10,windowHeight);
    ofNoFill();
    
    string player_1 = "Player 1";
    string player_2 = "Player 2";
    myfontLarge.drawString(player_1, 20, 50);
    myfontLarge.drawString(player_2, 20+windowWidth/2, 50);
    
    int facesSize = facesMap.size();
    std::cout << "No. of faces: " << facesSize << std::endl;
    no_faces = facesSize;
    
    int subsamp = 2;
    smile_value=0;
    brow_value=0;
    mouth_value=0;
    frown_value=0;
    smirk_value=0;
    for (auto facePair : facesMap)
    {
        
        f=facePair.second;
        
        float inter_oc = 1.2*f.measurements.interocularDistance;
        float * values = (float *)&f.expressions;

        VecFeaturePoint points = f.featurePoints;
        float prev_x=0;
        float prev_y=0;
        int count=0;
        ofSetColor(255);
        for (auto& point : points)	//Draw face feature points.
        {
            
            if(count==5){
                float face_x1 = point.x;
                float face_y1 = point.y;
            }
            else if(count==10){
                float face_x2 = point.x;
                float face_y2 = point.y;
            }
            else if(count==2){
                
            }
            ofCircle(point.x/widthRecal, point.y/heightRecal, 3.0f);

            
            if(count==5){
                ofCircle(point.x/widthRecal, point.y/heightRecal, 3.0f);

                head_motion = (point.y-temp_x)*(point.y-temp_x);
                prev_x = point.x;
                prev_y = point.y;
                temp_x = point.y;
            }
            
        }
        
        count=0;
        
        int line_height = myfont.getLineHeight();
        
        
        for (string expression : expressions)
        {
            if(expression=="smirk"){
                smirk_value = smirk_value + int(*values);
            }
            
            //Shock 50%
            //https://pavlok.herokuapp.com/api/YAjnVJWw1z/shock/128
            
            //Vibrate 50%
            //https://pavlok.herokuapp.com/api/YAjnVJWw1z/vibro/160
            
            //Shock 100%
            //https://pavlok.herokuapp.com/api/YAjnVJWw1z/shock/255
            
            //Vibrate 100%
            //https://pavlok.herokuapp.com/api/YAjnVJWw1z/vibro/255
            
            //Vibrate 100%
            //https://pavlok.herokuapp.com/api/YAjnVJWw1z/beep/255

            ofSetColor(255);
            int round_val = round(*values/10);
        //
            if(round_val<=2){
                ofSetColor(255,100);
            }
            else{
                ofSetColor(255);
            }
            string new_expression = mapExpressionName(expression);
            //myfont.drawString(new_expression + ": " , (prev_x+inter_oc+80)/widthRecal, (prev_y+line_height*count)/heightRecal);
            
            
            for (int j=0; j<10; j++)
            {
                ofSetColor(255,50);
                ofFill();
                //ofRect((prev_x+inter_oc+80 - (j+2)*7)/widthRecal, (prev_y+line_height*count-5)/heightRecal,5/widthRecal,5/widthRecal);
            }
            for (int j=0; j<round_val; j++)
            {
                ofSetColor(255);
                ofFill();
                //ofRect((prev_x+inter_oc+80 - (j+2)*7)/widthRecal, (prev_y+line_height*count-5)/heightRecal,5/widthRecal,5/widthRecal);
            }
            
            count++;
            values++;
        }

        
        count=0;
        values = (float *)&f.emotions;
        for (string emotion : emotions)
        {
            
            ofSetColor(255);

            
            if(1==1)//(emotion=="valence" || emotion=="engagement")
            {
                
                string new_expression = mapExpressionName(emotion);
                string str = new_expression + ": ";// + std::to_string(int(*values2));
                float string_width = myfont.stringWidth(str);
                
                int round_val = round(*values/10);
                round_val = abs(round_val);
                if(round_val<=2){
                    ofSetColor(255,100);
                }
                else{
                    ofSetColor(255);
                }
                //myfont.drawString(str, (prev_x-string_width-80)/widthRecal, (prev_y+line_height*count)/heightRecal);

                ofSetColor(255);
                
                for (int j=0; j<10; j++)
                {
                    ofSetColor(255,100);
                    ofFill();
                    //ofRect((prev_x-80 + (j+2)*7)/widthRecal, (prev_y+line_height*count-5)/heightRecal,5/widthRecal,5/widthRecal);
                }
                for (int j=0; j<round_val; j++)
                {
                    ofSetColor(255);
                    ofFill();
                    //ofRect((prev_x-80 + (j+2)*7)/widthRecal, (prev_y+line_height*count-5)/heightRecal,5/widthRecal,5/widthRecal);
                    ofSetColor(255);
                }
                ofSetColor(255);
                
                if(emotion=="valence")
                {
                    if(int(*values)<0){
                        ofSetColor(255,0,0);
                    }
                    else{
                        ofSetColor(0,255,0);
                    }
                    for (int j=0; j<round_val; j++)
                    {
                        ofFill();
                        //ofRect((prev_x-80 + (j+2)*7)/widthRecal, (prev_y+line_height*count-5)/heightRecal,5/widthRecal,5/widthRecal);
                    }
                    ofSetColor(255);
                    
                    smile_value = smile_value + int(*values);
                }

                
                count++;
            }
            values++;
        }

    
    }
    
    if(facesSize==0)
    {
        smile_value=0;
    }
    else
    {
        smile_value = smile_value/facesSize;
        smile_value = smile_value+10;
    }

    ofNoFill();
    
    
    
    int lengthTimeline = 30;

    if(nums.size()>lengthTimeline*subsamp){
        nums.erase( nums.begin() );
    }
    nums.push_back(smile_value);
    nums_smoothed.assign(nums.size(),0.0f);
    
    int smoothing_int = 10;
    for(int i = smoothing_int; i < nums.size(); i++){
        for(int j = 0; j < smoothing_int; j++){
            nums_smoothed[i] = nums_smoothed[i]+nums[i-j];
        }
        nums_smoothed[i] = nums_smoothed[i]/smoothing_int;
    }
    
    


}

ofApp::~ofApp()
{
    std::cerr << "Stopping FrameDetector Thread" << endl;
    detector->stop();	//Stop frame detector thread
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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

//--------------------------------------------------------------
string ofApp::mapExpressionName(string expression){
    
    string new_expression = "";
    if(expression=="innerBrowRaise"){ new_expression = "In.Brow Raise"; }
    else if(expression=="browRaise"){ new_expression = "Out.Brow Raise"; }
    else if(expression=="browFurrow"){ new_expression = "Brow Furrow"; }
    else if(expression=="noseWrinkle"){ new_expression = "Nose Wrinkle"; }
    else if(expression=="upperLipRaise"){ new_expression = "Lip Raise"; }
    else if(expression=="lipCornerDepressor"){ new_expression = "Frown"; }
    else if(expression=="chinRaise"){ new_expression = "Chin Raise"; }
    else if(expression=="lipPucker"){ new_expression = "Lip Pucker"; }
    else if(expression=="lipPress"){ new_expression = "Lip Press"; }
    else if(expression=="lipSuck"){ new_expression = "Lip Suck"; }
    else if(expression=="mouthOpen"){ new_expression = "Mouth Open"; }
    else if(expression=="smirk"){ new_expression = "Smirk"; }
    else if(expression=="eyeClosure"){ new_expression = "Blink"; }
    else if(expression=="smile"){ new_expression = "Smile"; }
    
    else if(expression=="joy"){ new_expression = "Joy"; }
    else if(expression=="fear"){ new_expression = "Fear"; }
    else if(expression=="disgust"){ new_expression = "Disgust"; }
    else if(expression=="sadness"){ new_expression = "Sadness"; }
    else if(expression=="anger"){ new_expression = "Anger"; }
    else if(expression=="surprise"){ new_expression = "Surprise"; }
    else if(expression=="contempt"){ new_expression = "Contempt"; }
    else if(expression=="valence"){ new_expression = "Valence"; }
    else if(expression=="engagement"){ new_expression = "Express."; }
    
    else{}

std::vector<std::string> emotions{"joy", "fear", "disgust", "sadness", "anger", "surprise", "contempt", "valence", "engagement" };

    return new_expression;
}

void ofApp::pavlokEvent()
{
    if(smirk_value>90){
        ofHttpResponse resp = ofLoadURL("https://pavlok.herokuapp.com/api/YAjnVJWw1z/vibro/160");
        cout << resp.data << endl;
    }
    
    
    if(smirk_value>90){
        ofHttpResponse resp = ofLoadURL("https://pavlok.herokuapp.com/api/jhn3gGBxgN/vibro/160");
        cout << resp.data << endl;
    }
}