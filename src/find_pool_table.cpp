
#include "find_pool_table.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b)  
{  
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];  
	int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];  
	if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))  
	{  
		cv::Point2f pt;  
		pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;  
		pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;  
		//-10 is a threshold, the POI can be off by at most 10 pixels
		if(pt.x<min(x1,x2)-10||pt.x>max(x1,x2)+10||pt.y<min(y1,y2)-10||pt.y>max(y1,y2)+10){  
			return Point2f(-1,-1);  
		}  
		if(pt.x<min(x3,x4)-10||pt.x>max(x3,x4)+10||pt.y<min(y3,y4)-10||pt.y>max(y3,y4)+10){  
			return Point2f(-1,-1);  
		}  
		return pt;  
	}  
	else  
		return cv::Point2f(-1, -1);  
}  

vector<Point> findTable(Mat src){
	Mat src_gray, detected_edges, cdst;	
	char* window_name = "Edge Map";
	double lowThreshold = 50;
	double highThreshold = 150;
	int kernel_size = 3;

	/// Convert the image to grayscale
	cvtColor( src, src_gray, CV_BGR2GRAY );



	/// Reduce noise with a kernel 7x7
	blur( src_gray, detected_edges, Size(8,8) );

	/*
	 * Otsu Thresholding?
	 */
	Mat _img;
	double otsu_thresh_val = cv::threshold(detected_edges, _img, 
			0, 
			255, 
			CV_THRESH_BINARY | CV_THRESH_OTSU);
	//Set the Threshold Values
	lowThreshold = otsu_thresh_val;
	highThreshold = otsu_thresh_val * 1.4;

	/// Canny detector
	Canny( detected_edges, detected_edges, lowThreshold, highThreshold, kernel_size );



	imshow( window_name, detected_edges );	
	waitKey(0);

	cvtColor(detected_edges, cdst, CV_GRAY2BGR);

	vector<Vec2f> lines;
	HoughLines(detected_edges, lines, 1, CV_PI/180, 200,0,0 );

	for( size_t i = 0; i < lines.size(); i++ )
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 10000*(-b));
		pt1.y = cvRound(y0 + 10000*(a));
		pt2.x = cvRound(x0 - 10000*(-b));
		pt2.y = cvRound(y0 - 10000*(a));
		line( cdst, pt1, pt2, Scalar(0,255,255), 1, CV_AA);
	}



	vector<Point> corners = findIntersection(cdst,lines);
	vector<Point> cornerAVG = averagePoints(corners);
	for(size_t i = 0; i < corners.size(); i++){
		//cout << corners.at(i).x << " " << corners.at(i).y << endl;
		circle(cdst, corners.at(i),4,Scalar(255,255,0),1,8,0);

	}
	// Draw Detected Square
	imshow("Detected Lines",cdst);
	return cornerAVG;

}

double ptDistance(Point& a, Point& b){
	double x = (a.x - b.x)*(a.x - b.x);
	double y = (a.y - b.y)*(a.y - b.y);
	return sqrt(x+y);

}

bool myfunction2 (vector<Point> a, vector<Point> b) { return (a.size()>b.size()); }
bool myfunction3 (Point a, Point b) { return (a.x < b.x); }
bool myfunction4 (Point a, Point b) { return (a.y < b.y); 
}

vector<Point> averagePoints(vector<Point> corner){
	sort(corner.begin(),corner.end(), myfunction3);
	sort(corner.begin(),corner.end(), myfunction4);
	vector<Point> retVec;
	vector< vector<Point> > bins;

	/*
	Try this algorithm instead:
	add first element to bins
	For all elements in corner vector,
	for all elements in bins
	if the corner element is near bins, add it
	else add to a new vector
	repeat till the entire corner vector is empty.
	
	*/
	vector<Point> cornerList;

	for (int i = 0; i < 4; ++i){
		if(corner.empty()) return retVec;
		Point temp = corner[0]; //Refine algorithm
		vector<Point> cornerList;
		cornerList.push_back(temp);
		for(unsigned int j = (corner.size()-1); j > 0; j-- ){
			if(ptDistance(corner[j],temp) < 60){
				cornerList.push_back(corner[j]);
				corner.erase(corner.begin() + j);
			}

		}
		corner.erase(corner.begin());
		bins.push_back(cornerList);
		cornerList.clear();

	}	
	//sort(bins.begin(),bins.end(), myfunction2);
	if(bins.size() >= 4){
		for(int i = 0; i < 4; i++){
			if(bins[i].empty()) return retVec;

			double xtotal = 0;
			double ytotal = 0;
			for(int j = 0; j < bins[i].size(); j++){
				xtotal += bins[i][j].x;
				ytotal += bins[i][j].y;
			}
			Point temp;
			temp.x = xtotal/bins[i].size();
			temp.y = ytotal/bins[i].size();
			retVec.push_back(temp);
		}
	}else{
		return retVec;
	}

	//sort(retVec.begin(),retVec.end(), myfunction3);
	
	sort(retVec.begin(),retVec.end(), myfunction4);


	return retVec;

}


vector<Point> findIntersection(Mat& src, vector<Vec2f>& lines){
	vector<Point> corners;
	for(size_t i = 0; i < lines.size() ; i++){
		double rho1 = lines[i][0];
		double theta1 = lines[i][1];
		double a1 = cos(theta1), b1 = sin(theta1);
		for(size_t j = i+1; j < lines.size(); j++){
			//Find the Cross Product to get the intersection

			double rho2 = lines[j][0];
			double theta2 = lines[j][1];
			if(abs(theta1 - theta2)*180/CV_PI < 10) continue;

			//if(abs(theta1-theta2)*180/CV_PI > 60) continue;
			double a2 = cos(theta2), b2 = sin(theta2);
			double zCross = (a1*b2-a2*b1);
			double xCross = (b1*(-1)*rho2 - b2*(-1)*rho1)/zCross;
			double yCross = (-1)*(a1*(-1)*rho2 - a2*(-1)*rho1)/zCross;
			Point temp;
			if(xCross > 0 && yCross > 0){
				if(xCross < src.cols && yCross < src.rows){
					temp.x = xCross;
					temp.y = yCross;
					//cout << xCross << " " << yCross << endl;
					corners.push_back(temp);
				}
			}
		}	
	}
	return corners;
}

