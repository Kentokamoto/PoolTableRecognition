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
	std::map<std::pair<float, float> , std::vector<Point2i> > matchingEdges;

	public:
		TableStitch();
		Mat getStitchedTable();
		void setStitchedTable(Mat input);
		//void findBalls(Mat src);
		void compute(Mat image);
		void organizeLinesAndEdges(std::vector<Point2i> edges, std::vector<Vec2f> lines, float accTol);
		std::vector<Vec2f> getEdgeCoords(Mat& input);
		std::vector<Vec2f> findCorrectLines(Mat image, std::vector<Vec2f> houghLines,float rhoOffset, float colorTol);
		std::vector<Point2i> getPockets(Mat image, float gapTol);
};




#endif