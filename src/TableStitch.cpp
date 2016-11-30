#include "TableStitch.h"

const double M_PI2DEG = 180/3.14159;
using namespace cv;
using namespace std;
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
 * Sorting Methods
 */
bool sortByRho(Vec2f a, Vec2f b){
    return a[0] > b[0];
}

bool sortByTheta(Vec2f a, Vec2f b){
    return a[1] > b[1];
}

bool sortByDistance(Point2i a, Point2i b){
  float aSqr = sqrt(a.x*a.x + a.y*a.y);
  float bSqr = sqrt(b.x*b.x + b.y*b.y);
  return aSqr > bSqr;
}


/*
 * Other Methods 
 */
void filterLines(std::vector<Vec2f>& lines, float ThetaTol, float RhoTol){
  for(int i = lines.size()-2; i >= 0; i--){
    if((abs(lines[i+1][0] - lines[i][0]) <= RhoTol) && (abs(lines[i+1][1] - lines[i][1]) <= ThetaTol)){
      lines.erase(lines.begin() + i+1);
    }
  }
}
vector<Vec2f> TableStitch::getEdgeCoords(Mat& input){
  vector<Vec2f> output;
  for(int i = 0; i < input.rows; i++){
    for(int j = 0; j < input.cols; j++){
      if( input.at<uchar>(i,j) > 0){
        output.push_back(Vec2f(i,j));
      }
    }
  }
  return output;
}


void TableStitch::organizeLinesAndEdges(vector<Point2i> edges, vector<Vec2f> lines, float accTol){
   for(int i = 0; i < edges.size(); i++){
     for(auto line : lines){
       float rho = line[0], theta = line[1];
       float output = (float)edges[i].x*cos(theta) + (float)edges[i].y*sin(theta);
       if(abs(output-rho) < accTol){
         if(matchingEdges.find(make_pair(line[0], line[1])) != matchingEdges.end()){ // In Map
          matchingEdges[make_pair(line[0], line[1])].push_back(edges[i]);
         }else{
          std::vector<Point2i> v;
          v.push_back(edges[i]);
          matchingEdges[make_pair(line[0], line[1])] = v;
         }
       }
     }
   }
}


 void TableStitch::compute(Mat left){
 	//std::cout << "GrayImages" << std::endl;
 	Mat leftGray, leftHSV, rightGray, rightHSV;
 	Mat leftDetectedEdges, rightDetectedEdges;
 	double lowThreshold=50;
 	double highThreshold=150;
 	int kernelSize = 3;

  // Detect Edges
  //blur(left, left, Size(5,5));
 	// Convert to HSV Gray Images 
 	cvtColor(left, leftHSV,CV_BGR2HSV);
 	cvtColor(leftHSV,leftGray,CV_BGR2GRAY);
  //leftDetectedEdges = leftGray;
 	// Detect Edges
 	blur(leftGray, leftDetectedEdges, Size(7,7));
 	Mat _img;
 	double otsu_thresh_val = cv::threshold(leftDetectedEdges, _img,
 		0,
 		255,
 		CV_THRESH_BINARY | CV_THRESH_OTSU);

 	lowThreshold = otsu_thresh_val*0.5;
 	highThreshold = otsu_thresh_val;
 	Canny(leftDetectedEdges, leftDetectedEdges, lowThreshold, highThreshold, kernelSize);
  Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*1 + 1, 2*1 ),
                                       Point( 1, 1 ) );
  dilate(leftDetectedEdges, leftDetectedEdges, element);
  //erode(leftDetectedEdges, leftDetectedEdges, element);
 	Mat cdst = leftDetectedEdges;
  #if 1
 	std::vector<Vec2f> lines;
 	HoughLines(leftDetectedEdges, lines,1,CV_PI/180, 400,0,0);
  sort(lines.begin(), lines.end(), sortByTheta);
 	//sort(lines.begin(), lines.end(), sortByRho);

  filterLines(lines, 4, 20);
  //vector<Vec2f> edges = getEdgeCoords(leftDetectedEdges);
  std::vector<cv::Point2i>  locations;   // output, locations of non-zero pixels 
  cv::findNonZero(leftDetectedEdges, locations);
  std::cout << locations.size() << std::endl;
  std::cout << lines.size() << std::endl;

  organizeLinesAndEdges(locations, lines, 10);
  cout << matchingEdges.size() << endl;
  cout << "Sizes" << endl;
  cvtColor(cdst, cdst, CV_GRAY2BGR);
  for(auto& point: matchingEdges){
    sort(point.second.begin(), point.second.end(), sortByDistance);
    cout << point.second.size() << endl;
    for(auto& linePoints : point.second){
      //cout << linePoints[1] << " " << linePoints[0] << endl;
      circle(cdst, linePoints, 1, Scalar(0,255,0));  
    }
    //line( cdst, point.second[0],point.second[point.second.size()-1], Scalar(0,0,255), 1, 8 );
  }
  
 	
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
 		line( cdst, pt1, pt2, Scalar(255,0,255), 1, CV_AA);
 	}
  #else
  // Dilate the imaage if possible.
  dilate(leftDetectedEdges, leftDetectedEdges, element);
  std::vector<Vec4i> linesP;
  cvtColor(leftDetectedEdges, cdst, CV_GRAY2BGR);
  //cdst = left;
  HoughLinesP( leftDetectedEdges, linesP, 1,CV_PI/180, 80, 150, 50 );
  //std::cout << linesP.size() << std::endl;
  for( size_t i = 0; i < linesP.size(); i++ ){
        line( cdst, Point(linesP[i][0], linesP[i][1]),
            Point(linesP[i][2], linesP[i][3]), Scalar(0,0,255), 1, 8 );
        circle(cdst, Point(linesP[i][0],linesP[i][1]), 5, Scalar(0,255,0));     
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
    namedWindow("Output", CV_WINDOW_NORMAL);
    stitchedTable = cdst;
    //imshow("Output" ,cdst);
    //1waitKey(0);
 	}

