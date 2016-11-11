#include "TableStitch.h"

const double M_PI2DEG = 180/3.14159;
using namespace cv;
TableStitch::TableStitch(){


}

/*
 * Getters
 */

 Mat TableStitch::getStitchedTable(){
 	return stitchedTable;
 }


 /*
  * Setters
  */
  void TableStitch::setStitchedTable(Mat input){
  	stitchedTable = input;
  }

/*
 * Other Methods 
 */
bool sortByRho(Vec2f a, Vec2f b){
    return a[0] > b[0];
}

bool sortByTheta(Vec2f a, Vec2f b){
    return a[1] > b[1];
}
void filterLines(std::vector<Vec2f>& lines, float ThetaTol, float RhoTol){
  for(int i = lines.size()-2; i >= 0; i--){
    if((abs(lines[i+1][0] - lines[i][0]) <= RhoTol) && (abs(lines[i+1][1] - lines[i][1]) <= ThetaTol)){
      lines.erase(lines.begin() + i+1);
    }
  }

}
 void TableStitch::compute(Mat left, Mat right){
 	std::cout << "GrayImages" << std::endl;
 	Mat leftGray, leftHSV, rightGray, rightHSV;
 	Mat leftDetectedEdges, rightDetectedEdges;
 	double lowThreshold=50;
 	double highThreshold=150;
 	int kernelSize = 3;

 	// Convert to HSV Gray Images
 	cvtColor(left, leftHSV,CV_BGR2HSV);
 	cvtColor(leftHSV,leftGray,CV_BGR2GRAY);

 	// Detect Edges
 	blur(leftGray, leftDetectedEdges, Size(5,5));
 	Mat _img;
 	double otsu_thresh_val = cv::threshold(leftDetectedEdges, _img,
 		0,
 		255,
 		CV_THRESH_BINARY | CV_THRESH_OTSU);

 	lowThreshold = otsu_thresh_val*0.5;
 	highThreshold = otsu_thresh_val;
 	Canny(leftDetectedEdges, leftDetectedEdges, lowThreshold, highThreshold, kernelSize);
 	Mat cdst = leftGray;
  #if 0
 	std::vector<Vec2f> lines;
 	HoughLines(leftDetectedEdges, lines,1,CV_PI/180, 200,0,0);
  //sort(lines.begin(), lines.end(), sortByTheta);
 	sort(lines.begin(), lines.end(), sortByRho);

  filterLines(lines, 4, 20);
 	cvtColor(cdst, cdst, CV_GRAY2BGR);
 	for( size_t i = 0; i < lines.size(); i++ ){
 		float rho = lines[i][0], theta = lines[i][1];
 		Point pt1, pt2;
 		double a = cos(theta), b = sin(theta);
 		double x0 = a*rho, y0 = b*rho;

    std::cout << "Theta: " << theta * M_PI2DEG << " Rho: " << rho << std::endl;
 		
    pt1.x = cvRound(x0 + 10000*(-b));
 		pt1.y = cvRound(y0 + 10000*(a));
 		pt2.x = cvRound(x0 - 10000*(-b));
 		pt2.y = cvRound(y0 - 10000*(a));
 		line( cdst, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
 	}
  #else
  std::vector<Vec4i> linesP;
  cvtColor(leftDetectedEdges, cdst, CV_GRAY2BGR);
  HoughLinesP( leftDetectedEdges, linesP, 1, CV_PI/180, 60, 150, 50 );
  std::cout << linesP.size() << std::endl;
  for( size_t i = 0; i < linesP.size(); i++ ){
        line( cdst, Point(linesP[i][0], linesP[i][1]),
            Point(linesP[i][2], linesP[i][3]), Scalar(0,0,255), 2, 8 );
  }
#endif

	// Detect Pockets
  std::vector<std::vector<Point> > contours;
  std::vector<Vec4i> hierarchy;

  findContours( leftDetectedEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  RNG rng(12345);
  /// Draw contours
  Mat drawing = Mat::zeros( leftDetectedEdges.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
     }

  // Ptr<FeatureDetector> blobsDetector = FeatureDetector::create("SimpleBlob");
  // std::vector<KeyPoint> keypoints;
  // blobsDetector->detect(leftGray, keypoints);
  // Mat drawImage = leftGray.clone();
  // for (size_t i = 0; i < keypoints.size(); ++i){
  //   circle(drawImage, keypoints[i].pt, 4, Scalar(255, 0, 255), -1); 
  // }

 		std::vector<int> compression_params;
 		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
 		compression_params.push_back(9);
 		imwrite("Output.png", cdst,compression_params );
    //imshow("Output" ,cdst);
    //waitKey(0);
 	}

