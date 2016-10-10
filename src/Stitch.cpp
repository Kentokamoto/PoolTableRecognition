#include "Stitch.h"

/*
 * Function that combines all of the subfunctions into one
 */
Mat Stitch::stitchImages(VideoCapture &cap){
		std::vector<Mat> sharpImages;
		for( int frameNum = 0; frameNum < cap.get(CV_CAP_PROP_FRAME_COUNT); frameNum++){
				Mat frame;
				if(!cap.read(frame)){
					std::cout << "Failed to read in frame" << std::endl;	
					break;
				}
				Mat laplaceImg;
				Laplacian(frame, laplaceImg, CV_64F);
				cv::Scalar mu, sigma;
				cv::meanStdDev(laplaceImg, mu, sigma);

				double focusMeasure = sigma.val[0]*sigma.val[0];	
				if( focusMeasure > 80.0){
						std::cout << "Variance: " << focusMeasure << std::endl;				
						sharpImages.push_back(frame);
						//imshow( "Frame", frame );
						//if(waitKey(0) == 27) {
						//		exit(EXIT_FAILURE);
						//}
				}
				
								/*if(cap.get(CV_CAP_PROP_POS_FRAMES) == 0){
								  cap.read(outputImage);
								  cv::resize(outputImage, outputImage, cv::Size(outputImage.cols/1.5, outputImage.rows/1.5));
								  imshow("image", outputImage);
								  }else{
								  Mat frame;
								  if(!cap.read(frame)){
								  std::cout << "Failed to read frame" << std::endl;
								  break;
								  }else{
								  if((int) cap.get(CV_CAP_PROP_POS_FRAMES)%FRAME_SKIP == 0){
								  std::cout << "Frame: " << cap.get(CV_CAP_PROP_POS_FRAMES) << std::endl;
								  cv::resize(frame, frame, cv::Size(frame.cols/1.5, frame.rows/1.5));

								  if(waitKey(0) == 27) {
								  exit(EXIT_FAILURE);
								  } // hit ESC (ascii code 27) to quit
								  extractSIFT(frame);
								  }
								  }
								  }*/
		}
		sharpImages.erase(sharpImages.begin()+1,sharpImages.end()-1);
		std::cout << sharpImages.size() << std::endl;
		Stitcher stitcher = Stitcher::createDefault(false);
		Stitcher::Status status = stitcher.stitch(sharpImages,outputImage);
		for(int i = 0; i < sharpImages.size(); i++){
			imshow("Frame", sharpImages[i]);
			if(waitKey(0) == 27){
				exit(EXIT_FAILURE);
			}
		}
		if(status == Stitcher::OK){
				imshow("Final Output", outputImage);
		}else{
				std::cout << "Failed" << std::endl;
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
void Stitch::extractSIFT(Mat frame){
		Mat grayFrame, grayOutputImage;
		cvtColor(frame, grayFrame, CV_BGR2GRAY);
		cvtColor(outputImage, grayOutputImage, CV_BGR2GRAY);

		imshow("Gray Frame", grayFrame);
		imshow("Gray Output Image", grayOutputImage);

		// Set keypoints

		Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();
		std::vector<KeyPoint> frameKeypoints, outputImageKeypoints;
		Mat descriptors_1, descriptors_2;
		detector->detectAndCompute(frame, Mat(), frameKeypoints, descriptors_1);
		std::cout << "Detection 1" << std::endl;
		detector->detectAndCompute(outputImage, Mat(), outputImageKeypoints, descriptors_2);
		std::cout << "Detection Complete" << std::endl;


		FlannBasedMatcher flann;
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
		std::vector< Point2f > framePoint;
		std::vector< Point2f > outputImagePoint;

		for( int i = 0; i < good_matches.size(); i++ )
		{
				//-- Get the keypoints from the good matches
				outputImagePoint.push_back( outputImageKeypoints[ good_matches[i].queryIdx ].pt );
				framePoint.push_back( frameKeypoints[ good_matches[i].trainIdx ].pt );
		}

		// Find the Homography Matrix
		if(framePoint.size() >4 && outputImagePoint.size() >4){
				Mat H = findHomography( framePoint, outputImagePoint, CV_RANSAC );
				// Use the Homography Matrix to warp the images
				cv::Mat result;
				warpPerspective(frame,result,H,cv::Size(frame.cols/2+outputImage.cols,frame.rows));

				std::cout << "Warped" << std::endl;
				cv::Mat half(result,cv::Rect(0,0,outputImage.cols,outputImage.rows));
				outputImage.copyTo(half);

				// trim the Black edges
				Mat grayResult;
				cvtColor(result, grayResult, CV_BGR2GRAY);



				imshow( "Result", result );
				outputImage = result;
		}




		//   //-- Draw only "good" matches
		// 	Mat img_matches;
		// 	drawMatches( frame, frameKeypoints, outputImage, outputImageKeypoints,
		// 		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		// 		std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

		// //-- Show detected matches
		// 	imshow( "Good Matches", img_matches );



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
