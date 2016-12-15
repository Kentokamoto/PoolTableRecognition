
#include "find_pool_table.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

RNG rng(12345);

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

Mat warpImage(vector<Point2i>& corners, Mat src){   

		// Create a list of "ortho" square corner points.
		// Pool Table is 1:2 ratio.


	vector<cv::Point2f> squareOrtho;
	squareOrtho.push_back(cv::Point2f(0, 0));
	squareOrtho.push_back(cv::Point2f(0, 500));
	squareOrtho.push_back(cv::Point2f(500, 0));
	squareOrtho.push_back(cv::Point2f(500, 500));

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
	const int cellSize = 50;
	cv::Size imageSquareSize(20 * cellSize, 10 * cellSize);
	cv::warpPerspective(src, imageSquare, M, imageSquareSize);
	cv::imshow("OrthoPhoto", imageSquare);

	return imageSquare;

}

void findBalls(Mat src){

    /* Convert from Red-Green-Blue to Hue-Saturation-Value */
	Mat hsv;
	cvtColor( src, hsv, CV_BGR2HSV );

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
	int channels[] = {0, 1};

    calcHist( &hsv, 1, channels, Mat(), // do not use mask
    	hist, 1, histSize, ranges,
             true, // the histogram is uniform
             false );
    double maxVal=0;
    double minVal = -1;
    minMaxLoc(hist, &minVal, &maxVal, 0, 0);

    int scale = 10;
    Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);

    for( int h = 0; h < hbins; h++ )
    	for( int s = 0; s < sbins; s++ )
    	{
    		float binVal = hist.at<float>(h, s);
    		int intensity = cvRound(binVal*255/maxVal);
    		rectangle( histImg, Point(h*scale, s*scale),
    			Point( (h+1)*scale - 1, (s+1)*scale - 1),
    			Scalar::all(intensity),
    			CV_FILLED );
    	}

    	namedWindow( "Source", 1 );
    	imshow( "Source", src );

    	namedWindow( "H-S Histogram", 1 );
    	imshow( "H-S Histogram", histImg );
    	waitKey();

    	imshow("Example", hsv);

    	vector<Mat> hsv_channels;
    	split(hsv, hsv_channels);
    	Mat v_Channel = hsv_channels[2];

    	int largest_area=0;
    	int largest_contour_index=0;
    	Rect bounding_rect;

    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    threshold(v_Channel,v_Channel, 0,maxVal,THRESH_OTSU);
    findContours(v_Channel,contours,hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	/// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );

    for( int i = 0; i < contours.size(); i++ )
    	{ approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    		minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    	}


  /// Draw polygonal contour + bonding rects + circles
    	Mat drawing = Mat::zeros( v_Channel.size(), CV_8UC3 );
    	for( int i = 0; i< contours.size(); i++ )
    	{
    		if((int)radius[i] > 40 || (int)radius[i] < 10) continue;
    		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    		drawContours( src, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    		circle( src, center[i], (int)radius[i], color, 2, 8, 0 );
    	}
    	/// Show in a window
  		namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  		imshow( "Contours", src );



    }
