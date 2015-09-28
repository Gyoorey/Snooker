#ifndef BALLS_HPP
#define BALLS_HPP

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
#include "Table.hpp"
#include "Matcher.hpp"

class Balls{
	public:		
		static constexpr float SIGNIFICANT_CHANGE_TOL = 0.25;
		static const unsigned short BLUE_LOW = 130;
		static constexpr float TABLE_WIDTH_BALL_WIDTH_RATIO = 0.035;
		static const int searchWindowWidth = 100;
		static constexpr float SIGNIFICANT_CHANGE_RATIO = 0.8;
		static std::vector<std::vector<int>> colorTable;
		static const int NUMBER_OF_COLORS = 8;
		static uint8_t kernel[1][5];
		static cv::Mat kernelMat; 
		enum colorIndex{
			WHITE,
			RED,
			YELLOW,
			GREEN,
			BROWN,
			BLUE,
			PINK,
			BLACK
		};
		
		Table table;
		Balls();
		
		int getBallColor(cv::Mat& image , cv::Point2i p);
		std::vector<cv::Point2i> getWhiteAreas(cv::Mat& image);
		void filterNearPoints(std::vector<cv::Point2i>& input);
		int getBallWidth( int y );
		void setTable(Table t);
		void findTemplates(cv::Mat& image, cv::Mat& white, cv::Mat& red, cv::Mat& yellow, cv::Mat& green, cv::Mat& brown, cv::Mat& blue, cv::Mat& pink, cv::Mat& black);
		
		cv::Point2i getWhitePos(cv::Mat& image);
	private:
		int checkBallnessSoft(cv::Mat& imageROI, double x, double y);
		int checkBallness(cv::Mat& imageROI, double x, double y);
		cv::Point2i lastWhitePos;
		int unDetectedWhiteCnt;
		
		
};



#endif
