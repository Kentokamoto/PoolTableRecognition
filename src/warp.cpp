
#include "find_pool_table.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;



template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  // initialize original index locations
	vector<size_t> idx(v.size());
	for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

  // sort indexes based on comparing values in v
		sort(idx.begin(), idx.end(),
			[&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

	return idx;
}

Mat warpImage(vector<Point>& corners, Mat src){
	Point2f srcTri[3];
	Point2f dstTri[3];


		// Create a list of "ortho" square corner points.
		// Pool Table is 1:2 ratio.


	vector<cv::Point2f> squareOrtho;
	squareOrtho.push_back(cv::Point2f(0, 0));
	squareOrtho.push_back(cv::Point2f(0, 750));
	squareOrtho.push_back(cv::Point2f(1500, 0));
	squareOrtho.push_back(cv::Point2f(1500, 750));

	// Organize the corner points so we can distinguish long side and short side.
	vector<Point2f> correctedCorner;
	vector<double> length;
	for(size_t i = 1; i < corners.size(); i++){
		double temp = 0.0;
		temp = ptDistance(corners[0], corners[i]);
		length.push_back(temp);
	}
	Point2f temp(corners[0].x, corners[0].y);
	correctedCorner.push_back(temp);
	circle(src, corners[0],4,Scalar(255,255,0),1,10,0);
	for(auto i: sort_indexes(length)){
		Point2f temp(corners[i+1].x, corners[i+1].y);
		correctedCorner.push_back(temp);
	}


	// line(warp_dst,correctedCorner.at(0), correctedCorner.at(2), Scalar(255,0,0),3, 8, 0); //Blue
	// line(warp_dst,correctedCorner.at(2), correctedCorner.at(3), Scalar(0,255,0),3, 8, 0); //Green
	// line(warp_dst,correctedCorner.at(3), correctedCorner.at(1), Scalar(0,0,255),3, 8, 0); //Red
	// line(warp_dst,correctedCorner.at(1), correctedCorner.at(0), Scalar(255,255,0),3, 8, 0); //Cyan

	//imshow("Boundary", warp_dst);
	// vector<Point2f> correctedCorner;


	cv::Mat M = cv::getPerspectiveTransform(correctedCorner, squareOrtho);

	cv::Mat imageSquare;
	const int cellSize = 80;
	cv::Size imageSquareSize(20 * cellSize, 10 * cellSize);
	cv::warpPerspective(src, imageSquare, M, imageSquareSize);
	cv::imshow("OrthoPhoto", imageSquare);

	return imageSquare;

}

void findBalls(Mat src){
	Mat src_gray,color_dst;
	cvtColor( src, src_gray, CV_BGR2GRAY );
	//GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
	cvtColor( src_gray, color_dst, CV_GRAY2BGR );
	// imshow("", color_dst);
	vector<Vec3f> circles;


	Mat backOnly,ballOnly;
	assert(src.type() == CV_8UC3);

	inRange(src, Scalar(27, 27, 35), Scalar(110, 110, 110), backOnly);
	ballOnly = 255-backOnly;

	//Show Image
	cv::imshow("redonly", ballOnly);


}