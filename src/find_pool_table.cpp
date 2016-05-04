
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

void findTable(Mat src){
	Mat src_gray, detected_edges, cdst, color_dst;	
	char* window_name = "Edge Map";
	double lowThreshold = 50;
	double highThreshold = 150;
	int kernel_size = 3;

/// Convert the image to grayscale
	cvtColor( src, src_gray, CV_BGR2GRAY );
	cvtColor( src_gray, color_dst, CV_GRAY2BGR );
/// Create a window
	namedWindow( window_name, CV_WINDOW_AUTOSIZE );

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
 highThreshold = otsu_thresh_val * 0.5;

/// Canny detector
 Canny( detected_edges, detected_edges, lowThreshold, highThreshold, kernel_size );

 imshow( window_name, detected_edges );	
 waitKey(0);

 cvtColor(detected_edges, cdst, CV_GRAY2BGR);

 vector<Vec2f> lines;
 HoughLines(detected_edges, lines, 1, CV_PI/180	, 130,0,0 );

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
 	line( cdst, pt1, pt2, Scalar(0,255,255), 1, CV_AA);
 }
 vector<Vec2f> line1, line2;
//findOrthogonal(lines,line1,line2);

//cout << line1.size() << " " << line2.size() << endl;;
 vector<Point> corners = findIntersection(lines);
 vector<Point> cornerAVG = averagePoints(corners);
 for(size_t i = 0; i < corners.size(); i++){
	//cout << corners.at(i).x << " " << corners.at(i).y << endl;
 	circle(cdst, corners.at(i),4,Scalar(255,255,0),1,8,0);
 }
 imshow("detected lines",cdst);

}


bool myfunction (vector<Vec2f> a, vector<Vec2f> b) { return (a.size()>b.size()); }

//void findOrthogonal(vector<Vec2f> inputLine, vector<Vec2f>& line1, vector<Vec2f>& line2){
//	vector< vector<Vec2f> > bins;
//
//	// To set values
//	for(int i = 0; i < 18; i++)
//	{
//		vector<Vec2f> temp; // create an array, don't work directly on buff yet.
//		bins.push_back(temp); // Store the array in the buffer
//	}
//
//	for(size_t i = 0; i < inputLine.size(); i++){
//		bins[(int)floor((inputLine[i][1]*18/CV_PI))].push_back(inputLine[i]);
// 	}
// 	vector<int> index(bins.size());
// 	for (size_t i = 0; i != index.size(); ++i) index[i] = i;

// 	sort(index.begin(),index.end(), [&bins](size_t a, size_t b){return bins[a].size() > bins[b].size(); });
// 	int index2 = 0;
// 	for(int i = 1; i < index.size(); i++){
// 		if(abs(index[0] - index[i]) <= 2){ 
// 			continue;
// 		}else{ 
// 			index2 = index[i];
// 			break;
// 		} 
// 	}

// 	for(size_t i = 0; i < inputLine.size(); i++){
// 		double D = 25;
// 		if(abs( inputLine[i][1]*180/CV_PI - (index[0]-90)*180  ) < D ||
// 				(abs(inputLine[i][1]*180/CV_PI-180-(index[0]-90)*180) < D ||
// 				 abs(inputLine[i][1]*180/CV_PI-180+(index[0]-90)*180) < D)){
// 			line1.push_back(inputLine[i]);
// 		}else if(abs( inputLine[i][1]*180/CV_PI - (index[index2]-90)*180  ) < D || 
// 				(abs(inputLine[i][1]*180/CV_PI-180-(index[index2]-90)*180) < D ||
// 				 abs(inputLine[i][1]*180/CV_PI-180+(index[index2]-90)*180) < D)){
// 			line2.push_back(inputLine[i]);
// 		}
// 	}


// }

bool myfunction2 (vector<Point> a, vector<Point> b) { return (a.size()>b.size()); }

vector<Point> averagePoints(vector<Point>& corner){

	vector< vector<Point> > bins;
	for(size_t i = 0; i < corner.size(); i++){
		double x = corner[i].x;
		double y = corner[i].y;
		vector<Point> temp;
		temp.push_back(corner[i]);
		for(size_t j = i+1; j < corner.size(); j++){
			if(abs(x - corner[j].x) < 20 && abs(y - corner[j].y) < 20){
				temp.push_back(corner[j]);
			}

		}
		bins.push_back(temp);
		temp.clear();
	}

	vector<Point> retVec;
	if(bins.size() >= 4){
		sort(bins.begin(),bins.end(), myfunction2);
		for(size_t i = 0; i < 4; i++){
			double xSum = 0;
			double ySum = 0;
			for(size_t j = 0; j < bins[i].size(); j++){
			//cout << bins[i][j].x << " " << bins[i][j].y << endl;
				xSum += bins[i][j].x;
				ySum += bins[i][j].y;
			}
			Point temp(xSum/bins.size(), ySum/bins.size());
			retVec.push_back(temp);
		}
	}else{
		return retVec;
	}
	return retVec;

}




vector<Point> findIntersection(vector<Vec2f>& lines){
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
			double a2 = cos(theta2), b2 = sin(theta2);
			double zCross = (a1*b2-a2*b1);
			double xCross = (b1*(-1)*rho2 - b2*(-1)*rho1)/zCross;
			double yCross = (-1)*(a1*(-1)*rho2 - a2*(-1)*rho1)/zCross;
			Point temp;
			if(xCross > 0 && yCross > 0){
				temp.x = xCross;
				temp.y = yCross;
				cout << xCross << " " << yCross << endl;
				corners.push_back(temp);
			}
		}	
	}
	return corners;
}

