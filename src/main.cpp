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
	cv::VideoCapture cap("Photos/IMG_1284.MOV ");

	if (!cap.isOpened()) {          // check if we succeeded
		printf("error - can't open the camera; hit any key to quit\n");
		system("PAUSE");
		return EXIT_FAILURE;
	}

	while (true){
		cv::Mat imageInput;
		cap >> imageInput;
		if (imageInput.empty()) break;

		cv::Mat imageInputGray;
		cv::cvtColor(imageInput, imageInputGray, cv::COLOR_BGR2GRAY);

		cv::imshow("Marker", imageInputGray);
	}
	return EXIT_SUCCESS;
}
