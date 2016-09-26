/*
 * Project: PoolTable
 * File: Main.cpp
 * Date: 3/27/2016
 * By: Kento Okamoto
 */

#include <iostream>
#include "opencv2/opencv_modules.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <stdio.h>
#include "find_pool_table.h"
#include "warp.h"
#include "Stitch.h"

#define USEVIDEO	1//Set the value if you are using a video or image

using namespace std;
int main(int argv, char* argc[]){

	printf("Hit ESC key to quit\n");
#if USEVIDEO
	cv::VideoCapture cap("./src/Photos/Scan/IMG_0014.m4v");//1285 or 1367
	//cv::VideoCapture cap(0);
	if (!cap.isOpened()) {          // check if we succeeded
		printf("error - can't open the video file; hit enter key to quit\n");
		cin.ignore().get();
		return EXIT_FAILURE;
	}
	Stitch stitch;
	cv::Mat outputImage = stitch.stitchImages(cap);
	// while (true){
	// 	// Read in the image
	// 	cv::Mat imageInput;
	// 	cap >> imageInput;
	// 	if (imageInput.empty()) break;

	// 	// Resize the image and put it in outputImage
	// 	cv::Mat outputImage;
	// 	cv::resize(imageInput, outputImage, cv::Size(imageInput.cols, imageInput.rows));	
	// 	std::vector<cv::Point> corners = findTable(outputImage);
	// 	if(corners.size() == 4){
	// 		cv::Mat ortho = warpImage(corners, outputImage);
	// 		findBalls(ortho);
	// 	}
		

	// 	//cv::namedWindow("My Image", cv::WINDOW_NORMAL);
	// 	//cv::imshow("Marker", outputImage);
	// 	if (cv::waitKey(1) == 27)  break;  // hit ESC (ascii code 27) to quit

	// }
#else
	cv::Mat image = cv::imread("./src/Photos/IMG_1360.jpg");
	//cv::resize(image, image, cv::Size(image.cols/4, image.rows/4));
	std::vector<cv::Point> corners = findTable(image);
	if(corners.size() == 4){
		cv::Mat ortho = warpImage(corners, image);
		findBalls(ortho);
	}	
	cv::namedWindow("Initial Image",cv::WINDOW_AUTOSIZE);
	cv::imshow("Initial Image", image);
	if (cv::waitKey(0) == 27)  return EXIT_SUCCESS;
#endif
	return EXIT_SUCCESS;
}
