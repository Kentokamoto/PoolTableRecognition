#pragma once
#ifndef TABLE_STITCH_H
#define TABLE_STITCH_H

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <vector> 
#include <map>

using namespace cv;

class TableStitch{
	Mat stitchedTable;
	std::map<std::pair<float, float> , std::vector<Vec2f> > matchingEdges;
	public:
		TableStitch();
		Mat getStitchedTable();
		void setStitchedTable(Mat input);
		//void findBalls(Mat src);
		void compute(Mat left);
		void organizeLinesAndEdges(std::vector<Vec2f> edges, std::vector<Vec2f> lines, float accTol);
		std::vector<Vec2f> getEdgeCoords(Mat& input);
};




#endif