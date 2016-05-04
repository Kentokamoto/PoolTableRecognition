#ifndef FOOP_H
#define FOOP_H
#include <opencv2/opencv.hpp>
#include <vector>
void findTable(cv::Mat src);
std::vector<cv::Point> findIntersection(std::vector<cv::Vec2f>&);
std::vector<cv::Point> averagePoints(std::vector<cv::Point>& corner);
//void findOrthogonal(std::vector<cv::Vec2f> inputLine, std::vector<cv::Vec2f>& line1, std::vector<cv::Vec2f>& line2);
#endif

