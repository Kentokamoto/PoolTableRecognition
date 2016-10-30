#include "Stitch.h"

/*
 * Function that combines all of the subfunctions into one
 */
 Mat Stitch::stitchImages(std::vector<cv::Mat> input){ 
 	outputImage = input[0];

 	for(int i = 1; i < input.size(); i++){
 		extractSIFT(outputImage,input[i]);
 	}


 	return outputImage;
 }

 struct point_sorter
 {
 	bool operator ()( const Point2f a, Point2f  b )
 	{
 		return ( (a.x + 500*a.y) < (b.x + 500*b.y) );
 	}
 };


/*
 * Extract the SIFT features
 */
 void Stitch::extractSIFT(Mat frame1, Mat frame2){
 	//cv::resize(frame1, frame1, cv::Size(frame1.cols/2, frame1.rows/2));
 	//cv::resize(frame2, frame2, cv::Size(frame2.cols/2, frame2.rows/2));
 	Mat grayFrame1, grayFrame2;
 	cvtColor(frame1, grayFrame1, CV_BGRA2GRAY);
 	cvtColor(frame2, grayFrame2, CV_BGRA2GRAY);
 	blur( grayFrame1, grayFrame1, Size(3,3) );
 	blur( grayFrame2, grayFrame2, Size(3,3) );



		//imshow("Gray Frame", grayFrame);
		//imshow("Gray Output Image", grayOutputImage);
		//waitKey(0);
		// Set keypoints

 	Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();
 	std::vector<KeyPoint> frame1Keypoints, frame2Keypoints;
 	Mat descriptors_1, descriptors_2;
 	detector->detectAndCompute(grayFrame1, Mat(), frame1Keypoints, descriptors_1);
 	std::cout << "Detection 1" << std::endl;
 	detector->detectAndCompute(grayFrame2, Mat(), frame2Keypoints, descriptors_2);
 	std::cout << "Detection Complete" << std::endl;


 	BFMatcher flann;
 	std::vector< DMatch > matches;
 	flann.match( descriptors_2, descriptors_1, matches );


 	double max_dist = 0; double min_dist = 100;
		//-- Quick calculation of max and min distances between keypoints
 	for( int i = 0; i < descriptors_2.rows; i++ ){
 		double dist = matches[i].distance;
 		if( dist < min_dist ) min_dist = dist;
 		if( dist > max_dist ) max_dist = dist;
 	}
 	printf("-- Max dist : %f \n", max_dist );
 	printf("-- Min dist : %f \n", min_dist );
		//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
		//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
		//-- small)
		//-- PS.- radiusMatch can also be used here.
 	std::vector< DMatch > good_matches;
 	for( int i = 0; i < descriptors_2.rows; i++ ){ 
 		if( matches[i].distance <= max(2*min_dist, 0.02) ){
 			good_matches.push_back( matches[i]); 
 		}
 	}

 	for( int i = 0; i < (int)good_matches.size(); i++ )
 		{ printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }



 	//waitKey(0);

 	std::vector< Point2f > frame1Point;
 	std::vector< Point2f > frame2Point;
 	std::cout << good_matches.size() << std::endl;
 	for( int i = 0; i < good_matches.size(); i++ )
 	{
		//-- Get the keypoints from the good matches
 		frame2Point.push_back( frame2Keypoints[ good_matches[i].queryIdx ].pt );
 		frame1Point.push_back( frame1Keypoints[ good_matches[i].trainIdx ].pt );
 	}

		//   //-- Draw only "good" matches


		// Find the Homography Matrix
 	if(frame1Point.size() >4 && frame2Point.size() >4){
 		Mat H = findHomography( frame2Point, frame1Point, CV_RANSAC );
				// Use the Homography Matrix to warp the images
 		cv::Mat result;
 		warpPerspective(frame2,result,H,cv::Size(frame1.cols+frame2.cols,frame1.rows+frame2.rows));
 		removeBlack(result);
 		removeBlack(frame1);
 		std::cout << "Warped" << std::endl;
 		cv::Mat half(result,cv::Rect(0,0,frame1.cols,frame1.rows));
 		frame1.copyTo(half);

				// trim the Black edges
 		Mat grayResult;
 		cvtColor(result,result,CV_BGRA2BGR);
 		cvtColor(result, grayResult, CV_BGR2GRAY);
 		Mat _img;
 		double threshVal = cv::threshold(grayResult, _img, 
 			1, 
 			255, 
 			CV_THRESH_BINARY);

 		Point2f offset;
 		std::vector<std::vector<Point> > contours;
 		findContours(_img,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

 		Rect box = boundingRect(contours[0]);

 		Mat crop = result(box);
 		//imshow( "Result", result );
 		outputImage = crop;
 	}



// Mat img_matches;
// 			drawMatches( frame1, frame1Keypoints, frame2, frame2Keypoints,
// 				good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
// 				std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
//  			imshow( "Good Matches", img_matches );
// 			waitKey(0);



		//-- Show detected matches


		//     Mat frameDrawKeypoints, outputImageDrawKeypoints;
		//     drawKeypoints(frame, frameKeypoints, frameDrawKeypoints);
		//     drawKeypoints(outputImage, outputImageKeypoints, outputImageDrawKeypoints);
		//     std::cout << "Keypoints drawn " << std::endl;
		// 	imshow("Gray Frame", frameDrawKeypoints);	
		// 	imshow("Gray Output Image", outputImageDrawKeypoints); 

 	if(waitKey(0) == 27) {
 		exit(EXIT_FAILURE);
		} // hit ESC (ascii code 27) to quit


	} 
void Stitch::removeBlack(Mat& input){
	cv::Mat input_bgra;
    cv::cvtColor(input, input_bgra, CV_BGR2BGRA);

    // find all white pixel and set alpha value to zero:
    for (int y = 0; y < input_bgra.rows; ++y)
    for (int x = 0; x < input_bgra.cols; ++x)
    {
        cv::Vec4b & pixel = input_bgra.at<cv::Vec4b>(y, x);
        // if pixel is white
        if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
        {
            // set alpha to zero:
            pixel[3] = 0;
        }
    }
    input = input_bgra;
}
/*
 * Find Closes matching features from image output
 */
 void Stitch::findFeatures(){

 }

/*
 * Fit Homography
 */
 void Stitch::fitHomography(){

 }

/*
 * stich frame to outputImage
 */
 void Stitch::stichFrame(){

 }
