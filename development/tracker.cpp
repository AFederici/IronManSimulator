

#include <iostream>
#include "/Users/aj/Code/126/opencv/include/opencv2/opencv.hpp"
using namespace cv;

int main()
{
	//mat is just a "image container" / matrix of values type thing
	cv::Mat frame;
	cv::VideoCapture cap(0); //0 is camera index
	const char* windowName = "Tracker";
	cv::namedWindow(windowName); 
	int x = 10;
	int y = 20;
	int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
    double fontScale = 1.5;
    int thickness = 2;
    Point textOrg(100/5, 100/1.2);
    std::string someText = "Place Hand Below";
    std::vector<cv::Vec3b> pixels;
    Mat hsvFrame;
    MatND hist;
	while (1)
	{
		cap >> frame;
		std::cout << frame.at<cv::Vec3b>(cv::Point(50,250)) << std::endl;

		//Because the R, G, and B components of an object’s color in a digital 
		//image are all correlated with the amount of light hitting the object, 
		//image descriptions in terms of those components make object discrimination difficult.
		//Descriptions in terms of hue/lightness/chroma or hue/lightness/saturation are 
		//often more relevant.
		//'Hue' represents the color, 'saturation' represents the amount to which that respective color 
		//is mixed with white and 'value' represents the amount to which that 
		//respective color is mixed with black.
		cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

		for(int i = 50; i < 191; i+=5){
			for(int j = 250; j < 391; j+= 5){
				pixels.push_back(hsvFrame.at<cv::Vec3b>(i,j));
			}
		}
		/*
		// Quantize the hue to 30 levels
	    // and the saturation to 32 levels
	    int hbins = 30, sbins = 32;
	    int histSize[] = {hbins, sbins};
	    // hue varies from 0 to 179, see cvtColor
	    float hranges[] = { 0, 180 };
	    // saturation varies from 0 (black-gray-white) to
	    // 255 (pure spectrum color)
	    float sranges[] = { 0, 256 };
	    const float* ranges[] = { hranges, sranges };
	    // we compute the histogram from the 0-th and 1-st channels
	    int channels[] = {0, 1};

	    // need to convert pixels back to mat format instead of vector
	    cv::calcHist( &pixels, 1, channels, cv::Mat(), // work on roi, thats the only array, hue and saturation, empty mask
	             hist, 2, histSize, ranges,
	             true, // the histogram is uniform
	             false );

	    */

	    //https://docs.opencv.org/2.4/modules/imgproc/doc/histograms.html
	    /*
		double maxVal=0;
	    minMaxLoc(hist, 0, &maxVal, 0, 0);

	    int scale = 10;
	    Mat histImg = Mat::zeros(32*scale, 30*10, CV_8UC3);

	    for( int h = 0; h < 30; h++ )
	        for( int s = 0; s < 32; s++ )
	        {
	            float binVal = hist.at<float>(h, s);
	            int intensity = cvRound(binVal*255/maxVal);
	            rectangle( histImg, Point(h*scale, s*scale),
	                        Point( (h+1)*scale - 1, (s+1)*scale - 1),
	                        Scalar::all(intensity),
	                        CV_FILLED );
	        }
		*/

		cv::putText(frame, someText, textOrg, fontFace, fontScale, Scalar::all(0), thickness,8);
		cv::rectangle(frame, cv::Point(50,250),cv::Point(110,310), cv::Scalar(0, 0, 255)); 
		cv::rectangle(frame, cv::Point(130,250),cv::Point(190,310), cv::Scalar(0, 0, 255)); 
		cv::rectangle(frame, cv::Point(50,330),cv::Point(110,390), cv::Scalar(0, 0, 255)); 
		cv::rectangle(frame, cv::Point(130,330),cv::Point(190,390), cv::Scalar(0, 0, 255)); 
		cv::imshow(windowName, frame);


		//namedWindow( "H-S Histogram", 1 );
	    //imshow( "H-S Histogram", histImg );

		if (cv::waitKey(30) >= 0) break;
	}
	return 0;
}

