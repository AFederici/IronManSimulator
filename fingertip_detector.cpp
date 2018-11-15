//
//  fingertip_detector.cpp
//  emptyExample
//
//  Created by AJ Federici on 11/14/18.
//

#include "fingertip_detector.hpp"
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/photo.hpp"
#include "opencv2/video.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
void test(){
	//gotta figure out original format and get it hsv
	cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv); //black and white
	// Pre processing
	//Even after adjusting the interval there is too much noise. We need to apply special techniques like median
	//blur to remove isolated dots and dilate to fill “holes” which look like white noise
	int blurSize = 5;
	int elementSize = 5;
	cv::medianBlur(hsv, hsv, blurSize);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
	cv::dilate(hsv, hsv, element);
}
