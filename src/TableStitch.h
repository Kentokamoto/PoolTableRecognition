#pragma once
#ifndef TABLE_STITCH_H
#define TABLE_STITCH_H

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <vector> 

using namespace cv;

class TableStitch{
	Mat stitchedTable;

	public:
		TableStitch();
		Mat getStitchedTable();
		void setStitchedTable(Mat input);
		//void findBalls(Mat src);
		void compute(Mat left);


};




#endif