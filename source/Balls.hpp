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
		cv::Point2i getWhitePos(cv::Mat& image, Table& t);
	private:
		bool checkBallness(cv::Mat& imageROI, float x, float y);

};



#endif
