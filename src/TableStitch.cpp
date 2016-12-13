#include "TableStitch.h"

const double M_PI2DEG = 180/3.14159;
using namespace cv;
using namespace std;

Point2i tableColorLoc;
Point2i bumperColorLoc;
Point2i edgeColorLoc;

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
 * Structures
 */
struct lineInfo {
  Vec2f houghLine;
  int hit;
  int miss;

  lineInfo(Vec2f line, int hit, int miss){
    this->houghLine = line;
    this->hit = hit;
    this->miss = miss;
  }
};


/*
 * Sorting Methods
 */
bool sortByRho(Vec2f a, Vec2f b){
  return a[0] > b[0];
}

bool sortByTheta(Vec2f a, Vec2f b){
  return a[1] > b[1];
}

bool sortByHitMiss(lineInfo a, lineInfo b){
  return ((float)a.hit/(float)a.miss) > ((float)b.hit/(float)b.miss);
}
bool sortByX(Point2i a, Point2i b){
  return a.x > b.x;
}
bool sortByY(Point2i a, Point2i b){
  return a.y > b.y;
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
/*
 * The method takes in a set of non-zero points in a binary image
 * Compares those to the detected hough lines to see if any lie on that line.
 */
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

Point2i getIntersection(pair<float, float> firstPair, pair<float, float> secondPair){
  float theta1 = get<1>(firstPair);
  float rho1 = get<0>(firstPair);
  float theta2 = get<1>(secondPair);
  float rho2 = get<0>(secondPair);

  float m1 = -cos(theta1)/sin(theta1);
  float b1 = rho1/sin(theta1);

  float m2 = -cos(theta2)/sin(theta2);
  float b2 = rho2/sin(theta2); 

  int x = cvRound((b2-b1)/(m1-m2));
  int y = cvRound(m1*x + b1);
  return Point2i(x,y);
}

vector<Point2i> TableStitch::getPockets(Mat image, float gapTol){
  vector<Point2i> pockets;
  pair<float, float> line1, line2;
  vector<Point2i> points1, points2;
  // First find the parallel lines
  map<pair<float, float> , std::vector<Point2i> >::iterator prevIt = matchingEdges.begin();
  map<pair<float, float> , std::vector<Point2i> >::iterator it = prevIt;
  for(it ; it!= matchingEdges.end(); it++){
    if(it != matchingEdges.begin()){
      cout <<"Points: " <<  get<1>(prevIt->first)*M_PI2DEG <<" " << get<1>(it->first)*M_PI2DEG << endl;
      if(abs(get<1>(prevIt->first)*M_PI2DEG - get<1>(it->first)*M_PI2DEG) < 5.0){
        cout << "Found" << endl;
        line1 = prevIt->first;
        line2 = it->first;

        points1 = prevIt->second;
        points2 = it->second;
        break;
      }
      prevIt = it;
    }
  }

  // Find intersections
  cout << get<0>(line1) << " line1 " << get<1>(line1) << endl;
  cout << get<0>(line2) << " line2 " << get<1>(line2) << endl;
  if(get<0>(line2) == 0 && get<1>(line2)==0){
    return pockets;
  }
  for(auto& point : matchingEdges){
    if(point.first != line1 && point.first != line2){
      cout << get<0>(point.first) << " " << get<1>(point.first) << endl;
      Point2i intersection = getIntersection(point.first, line1);
      pockets.push_back(intersection);
      pockets.push_back(getIntersection(point.first, line2));
    }
  }

  for(int i = 0; i < points1.size()-1; i++){
    if(points1[i].x > image.cols/2){
      if((points1[i-1].x - points1[i].x) > gapTol){
        float theta = get<1>(line1);
        float rho = get<0>(line1);
        float m1 = -cos(theta)/sin(theta);
        float b1 = rho/sin(theta);
        int x = cvRound((points1[i-1].x + points1[i].x)/2);
        int y = cvRound(m1*x + b1);
        cout << "middle for line 1: "<< x << " " << y << endl;
        pockets.push_back(Point2i(x,y));
        break;
      }
    }
  }
  for(int i = 0; i < points2.size()-1; i++){
    if(points2[i].x > image.cols/2){
      if((points2[i-1].x - points2[i].x) > gapTol){
        float theta = get<1>(line2);
        float rho = get<0>(line2);
        float m1 = -cos(theta)/sin(theta);
        float b1 = rho/sin(theta);
        int x = cvRound((points2[i-1].x + points2[i].x)/2);
        int y = cvRound(m1*x + b1);
        pockets.push_back(Point2i(x,y));
        cout << "middle for line 2: "<< x << " " << y << endl;
        break;
      }
    }
  }

  return pockets;


}
/*
 * Callback Function used for selecting points on an image
 */
void CallBackFunc(int event, int x, int y, int flags, void* userdata){
  if  (event == EVENT_MBUTTONDOWN){
    cout << "Setting Edge color at clicked - position (" << x << ", " << y << ")" << endl;
    edgeColorLoc = Point2i(x,y);
  }
  else if  ( event == EVENT_RBUTTONDOWN ){
    cout << "Setting Bumper color at clicked - position (" << x << ", " << y << ")" << endl;
    bumperColorLoc = Point2i(x,y);
  }
  else if  ( event == EVENT_LBUTTONDOWN ){
    cout << "Setting Table color at clicked - position (" << x << ", " << y << ")" << endl;
    tableColorLoc = Point2i(x,y);
  }
}

bool colorIsSimilar(Vec3b color1, Vec3b color2, float tolerance){
  if(abs(color1[0]-color2[0]) < tolerance && 
    abs(color1[1]-color2[1]) < tolerance &&
    abs(color1[2]-color2[2]) < tolerance){
    return true;
}
return false;
}


vector<Vec2f> TableStitch::findCorrectLines(Mat image, vector<Vec2f> houghLines,float rhoOffset, float colorTol){
  vector<Vec2f> correctLines;
  vector<lineInfo> foundLinesV, foundLinesH;

  // Retrieve the RGB colors for the manually selected bumper and wood rail
  Vec3b bumperColor = image.at<Vec3b>(bumperColorLoc.y, bumperColorLoc.x);
  Vec3b edgeColor = image.at<Vec3b>(edgeColorLoc.y, edgeColorLoc.x);

  // Check each hough line found
  for (auto& line : houghLines){
    float rho = line[0], theta = line[1];
    float rho1, rho2;
    cout << "Rho: " << rho << " Theta: " << theta*M_PI2DEG << " rho Offset: " << rhoOffset << endl;
    rho1 = rho + rhoOffset;
    rho2 = rho - rhoOffset;
    cout << "Rho1: " << rho1 << " Rho2: " << rho2 << endl;

    int hit = 0;
    int miss = 0;
    for (int x = 0; x < image.cols; x+= 40){

      int y1 = cvRound(rho1/sin(theta) - x*cos(theta)/sin(theta));
      int y2 = cvRound(rho2/sin(theta) - x*cos(theta)/sin(theta));
      if(y1 < 0 || y1 > image.rows || y2 < 0 || y2 > image.rows ){
        continue;
      }else{
        Vec3b rhoColor1 = image.at<Vec3b>(y1, x);
        Vec3b rhoColor2 = image.at<Vec3b>(y2, x);

        // Check if the outer point color agrees with the edge color specified

        if(colorIsSimilar(rhoColor1, bumperColor,colorTol) || colorIsSimilar(rhoColor2, bumperColor, colorTol)){
          // Check if the inner point color agrees with the bumper color specified.
          if(colorIsSimilar(rhoColor1, edgeColor,colorTol) || colorIsSimilar(rhoColor2, edgeColor, colorTol) ){
            hit++;
          }else{
            miss++;
          }
        }else{
          miss++;
        }
      }
    }
    cout << " Hit: " << hit << " Miss: " << miss << endl;
    if((float)hit/(float)(miss)> 0.1){
      cout << "Found" << endl;
      if(theta*M_PI2DEG > 45 && theta*M_PI2DEG < 135){
        foundLinesH.push_back(lineInfo(line,hit,miss));
      }else{
        foundLinesV.push_back(lineInfo(line,hit,miss));
      }
    }
  }
  sort(foundLinesV.begin(), foundLinesV.end(), sortByHitMiss);
  sort(foundLinesH.begin(), foundLinesH.end(), sortByHitMiss);
  if(foundLinesH.size() >= 2){
    correctLines.push_back(foundLinesH[0].houghLine);
    correctLines.push_back(foundLinesH[1].houghLine);
  }else if(foundLinesH.size() == 1){
    correctLines.push_back(foundLinesH[0].houghLine);
  }

  if(foundLinesV.empty()== false){
    correctLines.push_back(foundLinesV[0].houghLine);
  }

  return correctLines;
}

/*
 * Main Program
 */
void TableStitch::compute(Mat image){
  //Create a window for User interaction
  namedWindow("My Window", CV_WINDOW_AUTOSIZE);
  //set the callback function for any mouse event
  setMouseCallback("My Window", CallBackFunc, NULL);
  cout << "Press left mouse button when selecting center table color: " << endl;
  cout << "Press right mouse button when selecting bumper color: " << endl;
  cout << "Press middle mouse button when selecting the table edge color: " << endl;
  //show the image
  while(true){
    imshow("My Window", image);
    // Wait until user press some key
    char key = waitKey(0) ;
    if(key == 'q'){
      break;
    }
  }

  Mat imgGray, imgHSV;
  Mat detectedEdges;
  double lowThreshold=50;
  double highThreshold=150;
  int kernelSize = 3;

  // Detect Edges
  // Convert RGB to HSV Gray Images 
  cvtColor(image, imgHSV,CV_BGR2HSV);
  cvtColor(imgHSV,imgGray,CV_BGR2GRAY);
  blur(imgGray, detectedEdges, Size(5,5));
  // Set thresholds using the Otsu Threshold Values
  Mat _img;
  double otsu_thresh_val = cv::threshold(detectedEdges, _img,
   0,
   255,
   CV_THRESH_BINARY | CV_THRESH_OTSU);
  lowThreshold = otsu_thresh_val*0.5;
  highThreshold = otsu_thresh_val;

  // Get Edge Detection using the Otsu Edge Detections
  Canny(detectedEdges, detectedEdges, lowThreshold, highThreshold, kernelSize);

  // Dilate the image (Make the nonzero values appear wider on screen)
  int dilationValue = 1;
  Mat element = getStructuringElement( MORPH_ELLIPSE,
   Size( 2*dilationValue + 1, 2*dilationValue ),
   Point( dilationValue, dilationValue ) );
  dilate(detectedEdges, detectedEdges, element);

  Mat cdst = detectedEdges;

  // Use Hough Lines Algorithm to get Rho and Theta Values
  #if 1
  std::vector<Vec2f> lines;
  HoughLines(detectedEdges, lines,1,CV_PI/180, 300,0,0);
  sort(lines.begin(), lines.end(), sortByTheta);
  // Run some tests to see which lines are true to the rail and the cushion of the image.
  vector<Vec2f> correctLines = findCorrectLines(image,lines,5.0, 20.0);
  // Filter extra noise from the lines so hough lines that appear to be very similar with 
  // slightly different thetas or rhos are ignored. 
  
  sort(correctLines.begin(), correctLines.end(), sortByRho);
  //filterLines(correctLines, 4, 20);
  //sort(correctLines.begin(), correctLines.end(), sortByTheta);
  //filterLines(correctLines, 4, 20);

  // Organize the lines so only we know which edges line up with the hough lines
  std::vector<cv::Point2i>  locations;   // output, locations of non-zero pixels 
  cv::findNonZero(detectedEdges, locations);
  std::cout << locations.size() << std::endl;
  std::cout << lines.size() << std::endl;
  organizeLinesAndEdges(locations, correctLines, 4);

  // Draw the lines that line up
  cvtColor(cdst, cdst, CV_GRAY2BGR);
  for(auto& point: matchingEdges){
    sort(point.second.begin(), point.second.end(), sortByX);
    cout << point.second.size() << endl;
    for(auto& linePoints : point.second){
      circle(cdst, linePoints, 1, Scalar(0,255,0));  
    }
    //line( cdst, point.second[0],point.second[point.second.size()-1], Scalar(0,0,255), 1, 8 );
  }
  
  // Draw the Hough Lines
  for( size_t i = 0; i < correctLines.size(); i++ ){
   float rho = correctLines[i][0], theta = correctLines[i][1];
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

 //   for( size_t i = 0; i < lines.size(); i++ ){
 //   float rho = lines[i][0], theta = lines[i][1];
 //   Point pt1, pt2;
 //   double a = cos(theta), b = sin(theta);
 //   double x0 = a*rho, y0 = b*rho;


 //   pt1.x = cvRound(x0 + 10000*(-b));
 //   pt1.y = cvRound(y0 + 10000*(a));
 //   pt2.x = cvRound(x0 - 10000*(-b));
 //   pt2.y = cvRound(y0 - 10000*(a));
 //   line( cdst, pt1, pt2, Scalar(255,0,0), 1, CV_AA);
 // }
 // Use Probabalistic Hough Lines
  #else
  // Dilate the imaage if possible.
  dilate(detectedEdges, detectedEdges, element);
  std::vector<Vec4i> linesP;
  cvtColor(detectedEdges, cdst, CV_GRAY2BGR);
  //cdst = left;
  HoughLinesP( detectedEdges, linesP, 1,CV_PI/180, 80, 150, 50 );
  //std::cout << linesP.size() << std::endl;
  for( size_t i = 0; i < linesP.size(); i++ ){
        line( cdst, Point(linesP[i][0], linesP[i][1]),
            Point(linesP[i][2], linesP[i][3]), Scalar(0,0,255), 1, 8 );
        circle(cdst, Point(linesP[i][0],linesP[i][1]), 5, Scalar(0,255,0));     
  }
#endif

	// Detect Pockets
 vector<Point2i> pockets = getPockets(image, 40.0);
 cout << pockets.size() << endl;
 for(auto& pocket : pockets){
  circle(cdst, pocket, 10, Scalar(0,0,255));  
}

std::vector<int> compression_params;
compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
compression_params.push_back(9);
imwrite("Output.png", cdst,compression_params );
namedWindow("Output", CV_WINDOW_NORMAL);
stitchedTable = cdst;
    //imshow("Output" ,cdst);
    //1waitKey(0);
}

