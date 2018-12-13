#include "ofApp.h"

int main() {
	ofSetupOpenGL((720 * 2) + 100, 512, OF_WINDOW);
	ofRunApp(new ofApp());
}

/*
 Next steps -
 	Final step for hand - add the red and blue contours and average
 	Once hand is finished, simply get the centroid and just track its x position and put a hand
 	or something where that spot is. very simple laser shoots colored boxes
*/
