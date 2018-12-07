#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
	//this is the camera
	cam.setup(720,512);
	ofSetFrameRate(30);
	
	
	//placeholder for largest contour
	largestContour = 0;
	
	someText = "Place Hand Below";
	
	//stores the min and max values for H, S, B
	c0 = {0,0,0,0};
	c1 = {0,0,0,0};
	c2 = {0,0,0,0};
	
	//used for looping through and collecting data
	count = 0;
	runTime = 400;
	error = 50;
	
	//Setting up the contour finder
	//140 threshold is pretty neutral, not strict but not relaxed with matching either
	contourFinder.setMinAreaRadius(70);
	contourFinder.setMaxAreaRadius(1200);
	contourFinder.setThreshold(140);
	contourFinder.setUseTargetColor(true);
	
	// imitate() will set up previous and diff
	// so they have the same size and type as cam
	imitate(previous, cam);
	imitate(diff, cam);
}

void ofApp::update() {
	
	cam.update();
	if (cam.isFrameNew()){
		
		copy(cam, duplicateCam);
		
		//Because the R, G, and B components of an object’s color in a digital
		//image are all correlated with the amount of light hitting the object,
		//image descriptions in terms of those components make object discrimination difficult.
		//Descriptions in terms of HSV or HSB are usually better
		//'Hue' represents the color
		//'Saturation' represents the amount to which that respective color
		
		//if you haven't calibrated for long enough keep doing it
		if (count > error && count < runTime){
			calibrate();
			std::cout << previous.getColor(50,50) << ": " << count << std::endl;
		}
		
		//if slightly after the calibration time but shouldnt continue forever
		if (count >= runTime && count <= runTime + error){
			generateColorBounds();
		}
		
		//currently not working, will possibly be taken out but the goal is just
		//to create a binary image based on a given foreground color
		if (count >= runTime){
			cv::Mat test = toCv(duplicateCam);
			img.setImageType(OF_IMAGE_GRAYSCALE);
			cv::inRange(test, Scalar(0, 0, 0), Scalar(0, 0, 0), test);
			ofxCv::toOf(test, img);
			img.update();
		}

		points.clear();
		vels.clear();
		//after all calculations do the actual contour findings
		if (count > runTime + error){
			ofColor c = ofColor::fromHsb(c0[0], c1[0], c2[0]);
			contourFinder.setTargetColor(c, TRACK_COLOR_HS);
			contourFinder.setFindHoles(true);
			contourFinder.findContours(cam);
			contourFinder.setSortBySize(true);
			//int range = 15 > contourFinder.size() ? 0 : contourFinder.size() - 15;
			for (int i = contourFinder.size() - 1; i > 0; i--){
				points.push_back(contourFinder.getCentroid(i));
				vels.push_back(contourFinder.getVelocity(i));
			}
		}
		
		count++;
		
		//second screen calculations done here
		calcDifferences();
	}
	
	
}

void ofApp::draw() {
	ofSetColor(255);
	cam.draw(0,0);
	if (count < runTime + error){
		ofSetColor(0,0,0);
		ofDrawBitmapString(someText, 50, 200);
		ofSetColor(255,0,0);
		ofNoFill();
		ofDrawRectangle(50, 250, 140, 140);
	}
	else{
		ofClear(ofColor(0,0,0));
		int enumerator = 0;
		
		for (cv::Point2f p : points){
			ofSetColor(255,0,0);
			if (((vels[enumerator][0] *  vels[enumerator][0]) + (vels[enumerator][1] * vels[enumerator][1])) > 20) {
				ofDrawCircle(p.x, p.y, 10);
			}
			
			enumerator++;
		}
		
		for (cv::Vec2f v : vels){
			std::cout << v << std::endl;
		}
		ofSetColor(255);
		contourFinder.draw();

	}
	ofSetColor(255);
	
	//second part
	ofSetColor(255);
	diff.draw(720, 0);
	
	// use the [] operator to get elements from a Scalar
	float diffRed = diffMean[0];
	float diffGreen = diffMean[1];
	float diffBlue = diffMean[2];
	
	ofSetColor(255, 0, 0);
	ofDrawRectangle(720, 0, diffRed, 10);
	ofSetColor(0, 255, 0);
	ofDrawRectangle(720, 15, diffGreen, 10);
	ofSetColor(0, 0, 255);
	ofDrawRectangle(720, 30, diffBlue, 10);
}

//adds the average HSB to a vector for the given frame this function is called during
void ofApp::calibrate(){
	float hue = 0;  // The hue value to set.
	float saturation = 0; // The saturation value to set.
	float brightness = 0; // The brightness value to set.
	float h = 0;
	float s = 0;
	float b = 0;
	std::vector<float> hsb(3,0);
	//go over roi and collect data
	for (int i = 50; i < 190; i++){
		for (int j = 250; j < 390; j++){
			duplicateCam.getColor(i,j).getHsb(hue, saturation, brightness);
			h += hue;
			s += saturation;
			b += brightness;
		}
	}
	hsb[0] = h / (140 * 140);
	hsb[1] = s / (140 * 140);
	hsb[2] = b / (140 * 140);
	pixels.push_back(hsb);
}

//goes over all the frame calibrate was called on and averages the averages and stores
//bounds based on some assumed error
void ofApp::generateColorBounds(){
	for (auto vec : pixels){
		//std::cout << vec << " - ";
		c0[3] += vec[0];
		c1[3] += vec[1];
		c2[3] += vec[2];
	}
	int l = pixels.size();
	c0[0] = (c0[3] * .8) / (16 * l);
	c1[0] = (c1[3] * .8) / (16 * l);
	c2[0] = (c2[3] * .8) / (16 * l);
	
	c0[1] = c0[3] * 1.5 / (16 * l);
	c1[1] = c1[3] * 1.5 / (16 * l);
	c2[1] = c2[3] * 1.5 / (16 * l);
	std::cout << c0[0] << "," << c1[0] << "," << c2[0] << std::endl;
	std::cout << c0[1] << "," << c1[1] << "," << c2[1] << std::endl;
}


void ofApp::calcDifferences(){
	// take the absolute difference of prev and cam and save it inside diff
	absdiff(cam, previous, diff);
	diff.update();
	
	// like ofSetPixels, but more concise and cross-toolkit
	copy(cam, previous);
	
	// mean() returns a Scalar. it's a cv:: function so we have to pass a Mat
	diffMean = mean(toCv(diff));
	
	// you can only do math between Scalars,
	// but it's easy to make a Scalar from an int (shown here)
	diffMean *= Scalar(50);
}
