#ifndef FOOP_H
#define FOOP_H
#include <opencv2/opencv.hpp>
#include <vector>
std::vector<cv::Point> findTable(cv::Mat src);
std::vector<cv::Point> findIntersection(cv::Mat&,std::vector<cv::Vec2f>&);
std::vector<cv::Point> averagePoints(std::vector<cv::Point> corner);
#endif

