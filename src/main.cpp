#include "ofApp.h"

int main() {
	ofSetupOpenGL(720 * 2, 512, OF_WINDOW);
	ofRunApp(new ofApp());
}

/*
 Next steps -
 	Try adding blur and stuff
 	Try combining differences with contours - should get rid of face
 	Once hand is finished, simply get the centroid and just track its x position and put a hand
 	or something where that spot is. very simple laser shoots colored boxes
*/
