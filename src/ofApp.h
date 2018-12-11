#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void calibrate();
	void generateColorBounds();
	void calcDifferences();
	void calcDiffContours();
	double customDist(cv::Point2f p, double x, double y);
	
	ofVideoGrabber cam; //grabs data from camera
	ofPixels duplicateCam; 	//used for storing same data as cam but allow it to be changed
	ofxCv::ContourFinder contourFinder;
	std::string someText; //the text displayed on the screen
	std::vector<double> c0; //channel 0 (Hue) max and min values
	std::vector<double> c1; //channel 1 (Saturation) max and min values
	std::vector<double> c2; //channel 2 (Brightness) max and min values
	
	float avgX = 0; //average x component for velocity
	float avgY = 0; //average y component for velocity
	
	std::vector<std::vector<float>> pixels; //collection of pixels
	int count; //used to check frame count
	int runTime; //time to calibrate sensors
	int error; //margin of time to do additional waiting and calculations
	
	size_t largestContour;
	
	cv::Mat output;
	ofImage img;
	
	std::vector<cv::Point2f> points; //store the centroid points given contours
	std::vector<cv::Vec2f> vels; //stores contoured points velocities
	
	ofPixels previous; //used to store the previous frame
	ofImage diff; //stores the difference between previous and the camera
	
	cv::Scalar diffMean; //stores color information (Vec4f)
	
	
	//NEW BELOW
	std::vector<std::vector<double>> toDraw;
	int colorThresh;
	std::vector<double> centerMovement;
	double minDist;
	int closestCont;
	ofxCv::ContourFinder cont;
	cv::Point target;
};
