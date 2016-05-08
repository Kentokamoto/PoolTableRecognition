#ifndef WARP_H
#define WARP_H
#include <opencv2/opencv.hpp>
#include <vector>
cv::Mat warpImage(std::vector<cv::Point>& corners, cv::Mat src);
void findBalls(cv::Mat);
double ptDistance(cv::Point& a, cv::Point& b);
#endif