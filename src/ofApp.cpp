#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
	//this is the camera
	cam.setup(720,512);
	ofSetFrameRate(30);
	arm.load("arm.png");
	background.load("background.jpg");
	energyShot.load("animation.png");
	alien.load("alien.png");
	
	isAlienDead = true;
	alienLocation.y = 10;
	difficulty = 0;
	
	someText = "Place Hand Below - ";
	
	//stores the min and max values for H, S, B
	c0 = {0,0,0,0};
	c1 = {0,0,0,0};
	c2 = {0,0,0,0};
	
	decision = cv::Point2f(0,0);
	
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
	
	//NEW BELOW
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
		}
		
		//if slightly after the calibration time but shouldnt continue forever
		if (count >= runTime && count <= runTime + error){
			generateColorBounds();
		}
		
		//currently not working, will possibly be taken out but the goal is just
		//to create a binary image based on a given foreground color
		if (count >= runTime){
			updateEnergyShot();
			updateAlien();
			checkHit();
		}
		
		//second screen calculations done here
		frameDifferences();
		calcDiffContours();
	

		points.clear();
		vels.clear();
		minDist = 100000;
		//after all calculations do the actual contour findings
		if (count > runTime + error){
			ofColor c = ofColor::fromHsb(c0[0], c1[0], c2[0]);
			calcColorContours(c);
			handLocDecision();
		}
		count++;
	}
}

void ofApp::draw() {
	
	ofSetColor(255);
	ofNoFill();
	if (count < runTime + error){
		cam.draw(0,0);
		std::string returnText = someText + std::to_string(runTime + error - count);
		ofDrawBitmapStringHighlight(returnText, 50, 50);
		ofSetColor(255,0,0);
	
		ofDrawRectangle(50, 250, 140, 140);
		ofSetColor(255);
	}
	else{
		ofClear(ofColor(0,0,0));
		ofSetColor(255);
		diff.draw(0, 0);
		
		float diffRed = diffMean[0];
		float diffGreen = diffMean[1];
		float diffBlue = diffMean[2];
		
		ofSetColor(255, 0, 0);
		ofDrawRectangle(0, 0, diffRed, 10);
		ofSetColor(0, 255, 0);
		ofDrawRectangle(0, 15, diffGreen, 10);
		ofSetColor(0, 0, 255);
		ofDrawRectangle(0, 30, diffBlue, 10);
		int enumerator = 0;
		
		for (cv::Point2f p : points){
			ofSetColor(255,0,0);
			if (((vels[enumerator][0] *  vels[enumerator][0]) + (vels[enumerator][1] * vels[enumerator][1])) > 20) {
				ofDrawCircle(p.x, p.y, 10);
			}
			
			enumerator++;
		}

		
		//red
		ofSetColor(255,0,0);
		ofSetLineWidth(6);
		contourFinder.draw();
		if (redAvg.x > 0){
			ofDrawCircle(redAvg.x, redAvg.x, 50);
		}
		//blue
		ofSetColor(0,0,255);
		cont.draw();
		if (midCont.x > 0){
			ofDrawCircle(midCont.x, midCont.x, 20);
		}
		if (blueAvg.x > 0){
			ofDrawCircle(blueAvg.x, blueAvg.x, 50);
		}
		
		ofSetColor(0,200,200);
		ofDrawCircle(decision.x, decision.x, 50);
		ofSetColor(255);
		ofFill();
		background.draw(720, 0, 700, 500);
		energyShot.draw(energyShotLocation.x , energyShotLocation.y, 30, 200);
		arm.draw( 1400 -  decision.x , 250, 180, 270);
		alien.draw(alienLocation.x, alienLocation.y, 160 - difficulty, 160 - difficulty);
	}
	
	ofSetColor(0, 255, 0);
	ofSetLineWidth(3);
	for (auto p : motionPixels){
		int xShifted = p[0] + 60 < 1440 ? p[0] + 60 - 720 : p[0] - 720;
		ofDrawSphere(xShifted , p[1], 1);
	}
	ofSetColor(255);
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
}


void ofApp::frameDifferences(){
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
	cont.findContours(diff);
	cont.setSortBySize(true);
	midCont = cont.size() > 0 ? cont.getCenter(cont.size()-1) : cv::Point2f(0,0);
	if (cont.size()-1 > 1){
		for (int i = cont.size()-1; i > int((cont.size() - 1)/2); i--){
			blueAvg += cont.getAverage(i);
		}
		blueAvg.x = blueAvg.x / ((cont.size()-1) - int((cont.size() - 1)/2));
		blueAvg.y = blueAvg.y / ((cont.size()-1) - int((cont.size() - 1)/2));
	}
	else{
		blueAvg = cv::Point2f(0,0);
	}
	blueRunningAvg.push_back(blueAvg);
	if (blueRunningAvg.size() > 20){
		blueRunningAvg.pop_front();
	}
	blueAvg = cv::Point2f(0,0);
	for (auto it=blueRunningAvg.begin(); it!=blueRunningAvg.end(); ++it){
		blueAvg += *it;
	}
	blueAvg.x = blueAvg.x / (1.0 * blueRunningAvg.size());
	blueAvg.y = blueAvg.y / (1.0 * blueRunningAvg.size());
	
}

void ofApp::calcColorContours(ofColor c){
	contourFinder.setTargetColor(c, TRACK_COLOR_HS);
	contourFinder.setFindHoles(true);
	contourFinder.findContours(cam);
	contourFinder.setSortBySize(true);
	redAvg = cv::Point2f(0,0);
	
	if (contourFinder.size() > 0){
		int last = contourFinder.size() - 1;
		redAvg += contourFinder.getCenter(last);
		points.push_back(contourFinder.getCentroid(last));
		vels.push_back(contourFinder.getVelocity(last));
		double testDist = customDist(points[last], centerMovement[0], centerMovement[1]);
		if (testDist < minDist){
			minDist = testDist;
			target = cv::Point(centerMovement[0], centerMovement[1]);
		}
	}

	else{
		redAvg = cv::Point2f(0,0);
	}
	
	redRunningAvg.push_back(redAvg);
	if (redRunningAvg.size() > 20){
		redRunningAvg.pop_front();
	}
	redAvg = cv::Point2f(0,0);
	for (auto it=redRunningAvg.begin(); it!=redRunningAvg.end(); ++it){
		if (it->x > 20){
			redAvg += 0.2 * (*it);
		}
		
		redAvg += *it;
	}
	redAvg.x = redAvg.x / (1.0 * redRunningAvg.size());
	redAvg.y = redAvg.y / (1.0 * redRunningAvg.size());
}

void ofApp::calcDiffContours(){
	centerMovement = {0,0,0,0};
	motionPixels.clear();
	motionThresh = 510;
	int beginningFrameX = 720;
	int endingFrameX = 1440;
	int beginningFrameY = 0;
	int endingFrameY = 512;
	for (int i = beginningFrameX; i < endingFrameX; i  += 2){
		for (int j = beginningFrameY; j < endingFrameY; j += 2){
			ofColor col = diff.getColor(i, j);
			std::vector<double> holder(2,0);
			if (col[0] + col[1] + col[2] > motionThresh){
				holder[0] = i;
				holder[1] = j;
				centerMovement[2] += i;
				centerMovement[3] += j;
				motionPixels.push_back(holder);
			}
		}
	}
	
	centerMovement[0] = centerMovement[2] / motionPixels.size();
	centerMovement[1] = centerMovement[3] / motionPixels.size();
	
}

double ofApp::customDist(cv::Point2f p, double x, double y){
	int xAdjusted = p.x + 720;
	return std::sqrt( ((xAdjusted - x) * (xAdjusted - x)) + ((p.y - y) * (p.y - y)));
}

void ofApp::handLocDecision(){
	decision = redAvg.y > 100 ? ((0.2 * blueAvg) + (1.8 * redAvg)) / 2 : blueAvg;
}

void ofApp::updateEnergyShot(){
	energyShotLocation.y = 270 - ((count * 7) % 500);
	if (270 - ((count * 7) % 500) > 260){
		energyShotLocation.x = 1450 - decision.x;
	}
}

void ofApp::updateAlien(){
	if (isAlienDead){
		int max = 1420;
		int min = 760;
		int range = max - min + 1;
		alienLocation.x = rand() % range + min;
		isAlienDead = false;
		difficulty += 10;
	}
}

void ofApp::checkHit(){
	if (energyShotLocation.x + 15 <= alienLocation.x + (160 - difficulty) && energyShotLocation.x + 15 >= alienLocation.x){
		if (energyShotLocation.y < 50 and energyShotLocation.y > 20){
			isAlienDead = true;
		}
	}
}
