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

#define USEVIDEO	0//Set the value if you are using a video or image

using namespace std;
int main(int argv, char* argc[]){

	printf("Hit ESC key to quit\n");
#if USEVIDEO
	// cv::VideoCapture cap("./src/Photos/NewScan/File_009.mp4");//1285 or 1367
	// //cv::VideoCapture cap(0);
	// if (!cap.isOpened()) {          // check if we succeeded
	// 	printf("error - can't open the video file; hit enter key to quit\n");
	// 	cin.ignore().get();
	// 	return EXIT_FAILURE;
	// }
	// Stitch stitch;
	// cv::Mat outputImage = stitch.stitchImages(cap);
	// if(outputImage.empty()){
	// 	exit(EXIT_FAILURE);
	// }
	// std::vector<cv::Point> corners = findTable(outputImage);
	// if(corners.size() == 4){
	// 		std::cout << "Found Corners" << std::endl;
	// 		cv::Mat ortho = warpImage(corners, outputImage);
	// 		vector<int> compression_params;
	// 		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	// 		compression_params.push_back(9);
	// 		imwrite("Ortho.png", ortho,compression_params );


	// 		imshow("Orthonormal", ortho);
	// 		if(waitKey(0) == 27){
	// 				exit(EXIT_FAILURE);
	// 		}
	// }
	// while (true){
	// 	// Read in the image
	// 	cv::Mat imageInput;
	// 	cap >> imageInput;
	// 	if (imageInput.empty()) break;

	// 	// Resize the image and put it in outputImage
	// 	cv::Mat outputImage;
	// 	cv::resize(imageInput, outputImage, cv::Size(imageInput.cols, imageInput.rows));	
	// 	std::vector<cv::Point> corn
	ers = findTable(outputImage);
	// 	if(corners.size() == 4){
	// 		cv::Mat ortho = warpImage(corners, outputImage);
	// 		findBalls(ortho);
	// 	}
		

	// 	//cv::namedWindow("My Image", cv::WINDOW_NORMAL);
	// 	//cv::imshow("Marker", outputImage);
	// 	if (cv::waitKey(1) == 27)  break;  // hit ESC (ascii code 27) to quit

	// }
#else
	//cv::Mat image = cv::imread("./src/Photos/Double/1/IMG_0106.jpg");
	//cv::Mat image2 = cv::imread("./src/Photos/Double/1/IMG_0107.jpg");
	//if(image.empty() == true){
	//		std::cout << "Failed " << std::endl;
	//}
	//cv::Mat image2 = cv::imread("./src/Photos/NewScan/Room00.jpeg");
	std::vector<Mat> images;
	images.push_back(cv::imread("./src/Photos/Exposure/1/IMG_0169.jpg"));
	images.push_back(cv::imread("./src/Photos/Exposure/1/IMG_0168.jpg"));
	//images.push_back(cv::imread("./src/Photos/Exposure/1/IMG_0167.jpg"));
	//images.push_back(cv::imread("./src/Photos/Exposure/1/IMG_0166.jpg"));
	Stitch stitch;
	cv::Mat outputImage = stitch.stitchImages(images);
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	imwrite("final.png", outputImage,compression_params );

	// vector<int> compression_params;
	// compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	// compression_params.push_back(9);

	cv::resize(outputImage, outputImage, cv::Size(outputImage.cols/2, outputImage.rows/2));
	imshow("Stitch", outputImage);
	waitKey(0);
	//std::vector<cv::Point> corners = findTable(image);
	//if(corners.size() == 4){
	//	cv::Mat ortho = warpImage(corners, image);
	//	imshow("Ortho", ortho);
	//	if(waitKey(0) == 27){
	//			exit(EXIT_FAILURE);
	//	}
	//	findBalls(ortho);
	//}	
	//cv::namedWindow("Initial Image",cv::WINDOW_AUTOSIZE);
//	cv::imshow("Initial Image", image);
	//if (cv::waitKey(0) == 27)  return EXIT_SUCCESS;
#endif
	return EXIT_SUCCESS;
}
