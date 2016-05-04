
#include "find_pool_table.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


void warpImage(vector<Point>& corners, Mat src){
	Point2f srcTri[3];
	Point2f dstTri[3];

	Mat rot_mat( 2, 3, CV_32FC1 );
	Mat warp_mat( 2, 3, CV_32FC1 );
	Mat warp_dst, warp_rotate_dst;


		// Create a list of "ortho" square corner points.
		// Pool Table is 1:2 ratio.

		
		vector<cv::Point2f> squareOrtho;
		squareOrtho.push_back(cv::Point2f(0, 0));
		squareOrtho.push_back(cv::Point2f(0, 500));
		squareOrtho.push_back(cv::Point2f(1000, 0));
		squareOrtho.push_back(cv::Point2f(1000, 500));
		

		vector<Point2f> correctedCorner;
		for(size_t i = 0; i < corners.size(); i++){
			Point2f temp(corners[i].x, corners[i].y);
			correctedCorner.push_back(temp);
		}

		cv::Mat M = cv::getPerspectiveTransform(correctedCorner, squareOrtho);

		cv::Mat imageSquare;
		const int cellSize = 50;
		cv::Size imageSquareSize(20 * cellSize, 10 * cellSize);
		cv::warpPerspective(src, imageSquare, M, imageSquareSize);
		cv::imshow("OrthoPhoto", imageSquare);

}