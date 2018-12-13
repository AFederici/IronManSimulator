#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
	//3 main OF functions
	void setup();
	void update();
	void draw();
	void calibrate(); //collects color data from the camera and average it over time
	void generateColorBounds(); //generate upper and lower bounds for target color
	void frameDifferences(); //calculates the absolute difference between two frames
	void calcDiffContours(); //calculates the contours of the difference frame
	double customDist(cv::Point2f p, double x, double y); //distance function for point2f
	void handLocDecision(); //with all calculated data decides location for in game hand
	void calcColorContours(ofColor c); //calculates contours of objects of a certain color
	void updateEnergyShot(); //updates position of energy shot
	void updateAlien(); //updates position of alien
	void checkHit(); //checks whether alien and energy shot collide
	
	// IMAGE VARIABLES //
	ofVideoGrabber cam; //grabs data from camera
	ofPixels duplicateCam; 	//used for storing same data as cam but allow it to be changed
	ofPixels previous; //used to store the previous frame
	ofImage diff; //stores the difference between previous and the camera
	ofxCv::ContourFinder contourFinder;
    ofxCv::ContourFinder cont; //alternate contour finder used for various purposes
	std::string someText; //the text displayed on the screen
	std::vector<double> c0; //channel 0 (Hue) max and min values
	std::vector<double> c1; //channel 1 (Saturation) max and min values
	std::vector<double> c2; //channel 2 (Brightness) max and min values
	int count; //used to check frame count
	int runTime; //time to calibrate sensors
	int error; //margin of time to do additional waiting and calculations
	std::vector<std::vector<float>> pixels; //used to store all pixels from frames
	cv::Scalar diffMean; //stores color information (Vec4f)
	
	
	// MOVEMENT VARIABLES //
	float avgX = 0; //average x component for velocity
	float avgY = 0; //average y component for velocity
	std::vector<std::vector<double>> motionPixels; //pixels where motion occurs
	int motionThresh; //threshold for if an object is in motion
	std::vector<cv::Point2f> points; //store the centroid points given contours
	std::vector<cv::Vec2f> vels; //stores contoured points velocities
	std::vector<double> centerMovement; //used to store how much the center of contour move
	double minDist; //used to hold the shortest distance between centers of contours
	cv::Point target; //the point with the minimum distance from a contour
	
	
	cv::Point2f midCont; //center of target contour
	cv::Point2f blueAvg; //average of the contours drawn in blue (diff frame)
	std::deque<cv::Point2f> blueRunningAvg; //running avg of blueAvg
	cv::Point2f redAvg; //average of the contours drawn in blue (color frame)
	std::deque<cv::Point2f> redRunningAvg;//running avg of redAvg
	cv::Point2f decision; //target for arm placement, is calculated from red and blue avg
	
	
	// GRAPHICS VARIABLES //
	ofImage arm;
	ofImage alien;
	ofImage background;
	ofImage energyShot;
	cv::Point2f energyShotLocation;
	cv::Point2f alienLocation;
	bool isAlienDead;
	int difficulty; //as this increases the alien size decreased making it harder to hit
	
};
