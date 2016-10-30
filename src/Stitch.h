/* Responsible for stiching images together */

#pragma once
#ifndef STITCH_H
#define STITCH_H

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/stitching.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>

using namespace cv;


class Stitch{
private:
	Mat outputImage;
	static const int FRAME_SKIP = 40;

public:
	/*
	 * Constructor
	 */
	Stitch(){};
	
	/*
	 * Function that combines all of the subfunctions into one
	 */
	cv::Mat stitchImages(std::vector<cv::Mat> input);

	/*
	 * Extract the SIFT features
	 */
	void extractSIFT(Mat frame1, Mat frame2); // Probably should return sift features

	/*
	 * Find Closes matching features from image output
	 */
	void findFeatures();

	/*
	 * Fit Homography
	 */
	void fitHomography();
	 /*
	  * stich frame to outputImage
	  */
	void stichFrame();

	/*
	 * Remove black borders and convert them to transparent
	 */
	void removeBlack(cv::Mat& input);

};


#endif
