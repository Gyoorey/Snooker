#ifndef BALLS_HPP
#define BALLS_HPP

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
#include "Table.hpp"

class Balls{
	public:		
		static const unsigned short BALL_WIDTH = 16;
		static const unsigned short SIGNIFICANT_CHANGE_TOL = 4;
		static const unsigned short BLUE_LOW = 100;
		
		static const int searchWindowWidth = 150;
		
		static constexpr float SIGNIFICANT_CHANGE_RATIO = 0.8;
		std::vector<cv::Point2i> getWhiteAreas(cv::Mat& image, Table& t);
		cv::Point2i getWhitePos(cv::Mat& image, Table& t);
	private:
		bool checkBallness(cv::Mat& imageROI, double x, double y);
		cv::Point2i lastWhitePos;
		int unDetectedWhiteCnt;
		
		
};



#endif
