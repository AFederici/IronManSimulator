

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
	std::vector<Scalar> pixels;
	Mat hsvFrame;
	MatND hist;
	std::vector<double> c0(4,0);
	std::vector<double> c1(4,0);
	std::vector<double> c2(4,0);
	int count = 0;
	int runTime = 60;
	int error = 3;
	while (1)
	{
		cap >> frame;
		cvtColor(frame, hsvFrame, COLOR_BGR2HSV);
		if (count < runTime + error){
			std::cout << hsvFrame.at<cv::Vec3b>(cv::Point(50,250)) << ": " << count << std::endl;
		}
		else{
			if (count % 10 == 0){
				Scalar m,d;
				cvtColor(frame(Rect(50,250,140,140)), hsvFrame, COLOR_BGR2HSV);
				meanStdDev(hsvFrame, m, d);
				std::cout << m[0] << "  " << c0[0] << "," << c0[1] << std::endl;
				std::cout << m[1] << "  " << c1[0] << "," << c1[1] << std::endl;
				std::cout << m[2] << "  " << c2[0] << "," << c2[1] << std::endl;
				if (m[0] >= c0[0] && m[0] <= c0[1] && m[1] >= c1[0] && m[1] <= c1[1]){
					std::cout << "YES" << std::endl;
				}
				else{
					std::cout << "NO" << std::endl;
				}
				
				cv::inRange(hsvFrame, cv::Scalar(c0[0], c1[0], 60), cv::Scalar(c0[1], c1[1], 255), hsvFrame); //black and white
				// Pre processing
				//Even after adjusting the interval there is too much noise. We need to apply special techniques like median
				//blur to remove isolated dots and dilate to fill “holes” which look like white noise
				int blurSize = 5;
				int elementSize = 5;
				cv::medianBlur(hsvFrame, hsvFrame, blurSize);
				Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
				cv::dilate(hsvFrame, hsvFrame, element);
				// Contour detection
				//there are some errors so we need to find the largest connected component
				/*The “findContours” function expects to receive a vector of vector of points, or, in another words, a vector of polygons. There are many detection methods. Here I chose “CV_RETR_EXTERNAL”, which means it will just return the most external contour, ignoring eventual contours that are inside the most external region. I then compare the areas of the returned polygons (through the “contourArea” function) to get the largest and then draw it on screen (through the “drawContours” function). */
				std::vector<std::vector<cv::Point> > contours;
				std::vector<cv::Vec4i> hierarchy;
				cv::findContours(hsvFrame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
				size_t largestContour = 0;
				for (size_t i = 1; i < contours.size(); i++)
				{
					if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])){
						largestContour = i;
					}
				}
				cv::drawContours(frame, contours, largestContour, cv::Scalar(0, 0, 255), 1);
				
				
			}
		}
		
		
		//Because the R, G, and B components of an object’s color in a digital
		//image are all correlated with the amount of light hitting the object,
		//image descriptions in terms of those components make object discrimination difficult.
		//Descriptions in terms of hue/lightness/chroma or hue/lightness/saturation are
		//often more relevant.
		//'Hue' represents the color, 'saturation' represents the amount to which that respective color
		//is mixed with white and 'value' represents the amount to which that
		//respective color is mixed with black.
		if (count > 10 && count % 2 == 0 && count < runTime){
			
			cvtColor(frame(Rect(50,250,140,140)), hsvFrame, COLOR_BGR2HSV);
			Scalar m,d;
			meanStdDev(hsvFrame, m, d);
			pixels.push_back(m);
		}
		
		//need to calculate ideal hsv values for the hand, don't wanna hardcode the values
		//for a more robost program usable by anyone
		if (count >= runTime && count <= runTime + error){
			for (auto vec : pixels){
				std::cout << vec << " - ";
				c0[3] += vec[0];
				c1[3] += vec[1];
				c2[3] += vec[2];
			}
			int l = pixels.size();
			c0[0] = (c0[3] * .7) / (4 * l);
			c1[0] = (c1[3] * .7) / (4 * l);
			c2[0] = (c2[3] * .7) / (4 * l);
			c0[1] = c0[3] * 1.3 / (4 * l);
			c1[1] = c1[3] * 1.3 / (4 * l);
			c2[1] = c2[3] * 1.3 / (4 * l);
		}
		//probably replacing all the historgram code with just some crude averaging
		
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
		if (count < runTime + error){
			cv::putText(frame, someText, textOrg, fontFace, fontScale, Scalar::all(0), thickness,8);
			cv::rectangle(frame, cv::Point(50,250),cv::Point(110,310), cv::Scalar(0, 0, 255));
			cv::rectangle(frame, cv::Point(130,250),cv::Point(190,310), cv::Scalar(0, 0, 255));
			cv::rectangle(frame, cv::Point(50,330),cv::Point(110,390), cv::Scalar(0, 0, 255));
			cv::rectangle(frame, cv::Point(130,330),cv::Point(190,390), cv::Scalar(0, 0, 255));
		}
		
		cv::imshow(windowName, frame);
		
		
		//namedWindow( "H-S Histogram", 1 );
		//imshow( "H-S Histogram", histImg );
		
		count++;
		if (cv::waitKey(30) >= 0) break;
	}
	return 0;
}

