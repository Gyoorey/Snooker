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
		static const unsigned short BALL_WIDTH = 14;
		static const unsigned short SIGNIFICANT_CHANGE_TOL = 5;
		static const float SIGNIFICANT_CHANGE_RATIO = 0.6;
		cv::Point2i getWhitePos(cv::Mat& image, Table& t);
	private:
		bool checkBallness(cv::Mat& imageROI, double x, double y);

};



#endif
