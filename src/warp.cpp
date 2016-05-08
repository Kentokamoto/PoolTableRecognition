
#include "find_pool_table.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


Mat warpImage(vector<Point>& corners, Mat src){
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

	// HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 300, 10, 0, 0 );

	// for( size_t i = 0; i < circles.size(); i++ )
	// {
	// 	Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	// 	int radius = cvRound(circles[i][2]);
 //   // circle center
	// 	circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
 //   // circle outline
	// 	circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
	// }

	// namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
	// imshow( "Hough Circle Transform Demo", src );

}