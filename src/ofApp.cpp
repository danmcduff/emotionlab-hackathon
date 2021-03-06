#include "ofApp.h"

#include <iostream>
#include <random>

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
#include <opencv2/imgproc/imgproc.hpp>

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

std::vector<std::string> expressions{"smile", "innerBrowRaise", "browRaise", "browFurrow" , "noseWrinkle",
    "upperLipRaise", "lipCornerDepressor", "chinRaise", "lipPucker", "lipPress",
    "lipSuck", "mouthOpen", "smirk", "eyeClosure"};

std::vector<std::string> emotions{"joy", "fear", "disgust", "sadness", "anger", "surprise", "contempt","valence", "engagement" };

Face f;
int no_faces;

int vocals_count = 0;

std::map<FaceId,Face> facesMap;

bool shock_flag = false;

/** Sample ordered random list of expressions */
std::vector<std::string> random_sample( const std::vector<std::string>& src_list, const int n=5 );

void ofApp::onImageResults(std::map<FaceId,Face> faces, Frame image){

    int facesSize = faces.size();
    
    facesMap = faces;
    no_faces = facesSize;
    imgWidth = image.getWidth();
    imgHeight = image.getHeight();
    imgdataMutex.lock();
    imgdata = image.getBGRByteArray();
    cv::Mat im( imgHeight, imgWidth, CV_8UC3, imgdata.get());
    cv::cvtColor( im, im, CV_BGR2RGB);
    imgdataMutex.unlock();
    int cnt=0;
    
    

}

void ofApp::onImageCapture(Frame image){
    
}



//--------------------------------------------------------------
void ofApp::setup(){
    
    myfont.load("Helvetica.ttf", 12);
    myfontLarge.load("Helvetica.ttf", 32);
    myfontXLarge.load("Helvetica.ttf", 100);
    //ofSetDataPathRoot("../Resources/data/");
	
	// Setup the game objectives
    expression_image_lookup.clear();
    expression_image_lookup[ "smile" ].load("images/smile.png");
    expression_image_lookup[ "smirk" ].load("images/smirk.png");
    expression_image_lookup[ "lipPucker" ].load("images/lipPucker.png");
    expression_image_lookup[ "lipCornerDepressor" ].load("images/lipDepressor.png");
    expression_image_lookup[ "browFurrow" ].load("images/browFurrow.png");
    expression_image_lookup[ "browRaise" ].load("images/browRaise.png");
    expression_image_lookup[ "mouthOpen" ].load("images/mouthOpen.png");
    expression_image_lookup[ "eyeClosure" ].load("images/eyeClosure.png");
    
    replay_button.load("images/replay_button.png");
    shock_button.load("images/shock_button.png");

    startNewGame();
    try
    {
        std::string affdexLicense = "{\"token\":\"\",\"licensor\":\"Affectiva Inc.\",\"expires\":\"2099-01-01\",\"developerId\":\"Affectiva-internal\",\"software\":\"Affdex SDK\"}";

//        path DATA_FOLDER = "data";
		//path DATA_FOLDER = "/Users/dmcduff/src/of_v20150910_osx_release/apps/myApps/affectiva-hackathon_rev02/bin/data/affdex-windows-sdk-iOS/data";
        path DATA_FOLDER = "/Users/dmcduff/src/of_v20150910_osx_release/apps/myApps/affectiva-hackathon_rev02/bin/data/affdex-windows-sdk-iOS/data";
        
        startNewGame();

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
    
    //currentFrame.flipPixels(imgdata.get(), true, false);
    //`currentFrame.fli
    
    imgdataMutex.lock();
    currentFrame.setFromPixels(imgdata.get(), imgWidth, imgHeight, ofImageType::OF_IMAGE_COLOR, false);
    imgdataMutex.unlock();
    
    windowWidth =  ofGetWidth();
    float widthRecal = float(imgWidth)/float(windowWidth);
    windowHeight = ofGetHeight();
    float heightRecal = float(imgHeight)/float(windowHeight);
    
    currentFrame.resize(windowWidth,windowHeight);
    currentFrame.mirror(false,true);
    //ofSetColor(200,200,0);
    currentFrame.draw(0,0);
    
    if(shock_flag==false){
        ofSetColor(255,50);
    }
    else{
        ofSetColor(255);
    }
    shock_button.draw(windowWidth-170, 50, 100,100);
    ofSetColor(255);
    
    ofSetColor(255);
    ofFill();
    ofRect(windowWidth/2, 0,10,windowHeight);
    ofNoFill();
    
    string player_1 = "Player 1";
    string player_2 = "Player 2";
    
    ofSetColor(255,0,0);
    myfontLarge.drawString(player_1, 50, 70);
    ofSetColor(0,0,255);
    myfontLarge.drawString(player_2, 20+windowWidth/2, 70);
    ofSetColor(255);
    
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
        
        float face_x1;
        float face_y1;
        float face_x2;
        float face_y2;
        float face_x3;
        float face_y3;
        
        int count=0;
        ofSetColor(255);
        ofNoFill();
        for (auto& point : points)	//Draw face feature points.
        {
            
            if(count==5){
                //ofCircle(point.x/widthRecal, point.y/heightRecal, 3.0f);
                face_x1 = windowWidth-point.x/widthRecal;
                face_y1 = point.y/heightRecal;
            }
            else if(count==10){
                //ofCircle(point.x/widthRecal, point.y/heightRecal, 3.0f);
                face_x2 = windowWidth-point.x/widthRecal;
                face_y2 = point.y/heightRecal;
            }
            else if(count==2){
                //ofCircle(point.x/widthRecal, point.y/heightRecal, 3.0f);
                face_x3 = windowWidth-point.x/widthRecal;
                face_y3 = point.y/heightRecal;
            }
            
            if(count==5){
                //ofCircle(point.x/widthRecal, point.y/heightRecal, 3.0f);

                head_motion = (point.y-temp_x)*(point.y-temp_x);
                prev_x = point.x;
                prev_y = point.y;
                temp_x = point.y;
            }
            count++;
            
        }
		
		// Determine player and perform player specific logic
		const bool is_player1 = (face_x1+face_x2+face_x3)/3.0f < windowWidth/2;
		if( is_player1 ){
            ofSetColor(255,0,0);
        }
        else{
            ofSetColor(0,0,255);
        }
        ofRect(face_x1, face_y1-0.2*(face_y3-face_y1), face_x2-face_x1, 1.2*(face_y3-face_y1));
        
        count=0;
        
		// Update objective vector for players and stop on end of game
		int& player_index = is_player1 ? player1_index : player2_index;
		std::vector< bool >& player_objective = is_player1 ? player1_objective : player2_objective;
		const std::string& target_expression = expression_objective[ player_index ];
		
		if( player_objective.back())   // Handle 2-player mode
		{
			values = (float *)&f.expressions;
			for( string expression : expressions )
			{
				// For objective for this player for this frame
				if( expression == target_expression && *values >= 90.0f )
				{
					player_objective[player_index] = false;
					player_index++;
					break;
				}
				values++;
			}
		}
		
		// Drawing & display code
		values = (float *)&f.expressions;
        int line_height = myfont.getLineHeight();
        for (string expression : expressions)
        {
            if(expression=="smirk"){
                smirk_value = smirk_value + int(*values);
            }

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
    
    ofSetColor(255);
    for( unsigned int i_obj = 0; i_obj < expression_objective.size(); i_obj++ )
    {
        ofImage& im = expression_image_lookup[ expression_objective[ i_obj ] ];
        bool is_current_obj = i_obj == player1_index;
        ofSetColor( 255, 255, 255, is_current_obj ? 255 : 100 );
        int x =50 + is_current_obj*50;
        int y = 100+i_obj*(windowHeight-200) / expression_objective.size();
        im.draw(x, y, 50 + is_current_obj*50,50+ is_current_obj*50);

        is_current_obj = i_obj == player2_index;
        ofSetColor( 255, 255, 255, is_current_obj ? 255 : 100 );
        x =20 + is_current_obj*50 + windowWidth/2;
        y = 100+i_obj*(windowHeight-200) / expression_objective.size();
        im.draw(x, y, 50 + is_current_obj*50,50+ is_current_obj*50);
    }
    
	// Check for win-conditions&&
	if( player1_objective.back() == false && player2_objective.back() == false && endGame==true)
	{
		// TIE
		std::cout << "TIE GAME!!!!" << std::endl;
        ofSetColor(255);
        screenshot_img.draw(0,0);
        ofSetColor(255);
	}
	else if (player1_objective.back() == false && endGame==false)
	{
		// Player 1 wins
		std::cout << "Player 1 wins!!!" << std::endl;
        pavlokEvent(1);
        winning_player=1;
        string winner_string = "WINNER!";
        endGame=true;
        
        string timestamp = ofGetTimestampString();
        screenshot_img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
        screenshot_img.save("screenshots/screenshot_"+timestamp+".png");
	}
	else if (player2_objective.back() == false && endGame==false)
	{
		// Player 2 wins
		std::cout << "Player 2 wins!!!" << std::endl;
        pavlokEvent(2);
        winning_player=2;
        endGame=true;
        
        string timestamp = ofGetTimestampString();
        screenshot_img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
        screenshot_img.save("screenshots/screenshot_"+timestamp+".png");
	}
    else if (endGame==true){
        ofSetColor(255);
        screenshot_img.draw(0,0);
        ofSetColor(255);
    }
    
    string winner_string = "WINNER!";
    ofSetColor(255);
    if(winning_player==1)
    {
        ofFill();
        ofSetColor(255,70);
        ofRect(windowWidth/2,0,windowWidth/2,windowHeight);
        //myfontXLarge.drawString(winner_string, 20,windowHeight/2);
    }
    else if(winning_player==2)
    {
        ofFill();
        ofSetColor(255,100);
        ofRect(0,0,windowWidth/2,windowHeight);
        //myfontXLarge.drawString(winner_string, windowWidth/2+20,windowHeight/2);
    }
    ofSetColor(255);
    
    if(endGame==true){
        replay_button.draw(windowWidth/2-200, windowHeight/2-200, 400, 400);
        //ofSetColor(0,255,0);
        //ofFill();
        //ofRect(windowWidth/2-200, windowHeight/2-100, 400, 200);
        //ofSetColor(255);
        //string reply_text = "Play Again!";
        //myfontLarge.drawString(reply_text, windowWidth/2-100, windowHeight/2+10);
        ofSetColor(255);
        //ofNoFill();
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
    if(x>windowWidth/2-200 && x<windowWidth/2-200+400 && y>windowHeight/2-200 && y<windowHeight/2+200 && endGame==true){
        startNewGame();
    }
    
    if(x>windowWidth-120 && x<windowWidth-20 && y>20 && y<120 ){
        shock_flag = shock_flag ^ true;
    }
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


    return new_expression;
}

std::vector<std::string> random_sample( const std::vector<std::string>& src_list, const int n )
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<std::string> shuffled_list = src_list;
	std::shuffle( shuffled_list.begin(), shuffled_list.end(), g );
    if( n > src_list.size() )
    {
        std::vector<std::string> sample_remainder = random_sample( src_list, n-src_list.size());
        shuffled_list.insert( shuffled_list.end(), sample_remainder.begin(), sample_remainder.end());
    }
    else
    {
        shuffled_list.resize( n );
    }
    return shuffled_list;
}

void ofApp::pavlokEvent(int player_no)
{


}

void ofApp::startNewGame()
{
    std::vector<std::string> possible_expression_objectives;
    for( const auto& exp_lookup_pair : expression_image_lookup )
    {
        possible_expression_objectives.push_back( exp_lookup_pair.first );
    }
    expression_objective = random_sample( possible_expression_objectives, 35 );
    player1_objective = std::vector< bool >( expression_objective.size(), true );
    player2_objective = std::vector< bool >( expression_objective.size(), true );
    player1_index = 0;
    player2_index = 0;
    
    endGame = false;
    winning_player = 0;
}
