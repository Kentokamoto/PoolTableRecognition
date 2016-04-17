/*
 * Project: PoolTable
 * File: Main.cpp
 * Date: 3/27/2016
 * By: Kento Okamoto
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>


using namespace std;

int main(int argv, char* argc[]){

	printf("Hit ESC key to quit\n");
	cv::VideoCapture cap("./src/Photos/IMG_1284.MOV");
	//cv::VideoCapture cap(0);
	if (!cap.isOpened()) {          // check if we succeeded
		printf("error - can't open the video file; hit enter key to quit\n");
		cin.ignore().get();
		return EXIT_FAILURE;
	}

	while (true){
		cv::Mat imageInput;
		cap >> imageInput;
		if (imageInput.empty()) break;

		cv::Mat imageInputGray;
		cv::cvtColor(imageInput, imageInputGray, cv::COLOR_BGR2GRAY);cv::namedWindow("My Image", cv::WINDOW_AUTOSIZE );
		cv::Mat outputImage;
		cv::resize(imageInput, outputImage, cv::Size(imageInputGray.cols/2, imageInputGray.rows/2));	
		cv::namedWindow("My Image", cv::WINDOW_NORMAL);
		cv::imshow("Marker", outputImage);

		if (cv::waitKey(1) == 27)  break;  // hit ESC (ascii code 27) to quit

	}
	return EXIT_SUCCESS;
}
