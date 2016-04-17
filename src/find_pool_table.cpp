#include <opencv2/opencv.hpp>
#include "find_pool_table.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


void findTable(Mat src){
	Mat src_gray, detected_edges, cdst;	
	char* window_name = "Edge Map";
	int lowThreshold = 20;
	int kernel_size = 3;

	/// Convert the image to grayscale
	cvtColor( src, src_gray, CV_BGR2GRAY );

	/// Create a window
	namedWindow( window_name, CV_WINDOW_AUTOSIZE );

	/// Reduce noise with a kernel 3x3
	blur( src_gray, detected_edges, Size(3,3) );

	/// Canny detector
	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*3, kernel_size );

	imshow( window_name, detected_edges );	
	waitKey(0);

	cvtColor(detected_edges, cdst, CV_GRAY2BGR);

	vector<Vec2f> lines;
	HoughLines(detected_edges, lines, 1, CV_PI/180, 100, 0, 0 );


	for( size_t i = 0; i < lines.size(); i++ )
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
	}
	
	imshow("detected lines", cdst);

}



