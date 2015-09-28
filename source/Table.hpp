#ifndef TABLE_HPP
#define TABLE_HPP

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>

class Table{
	public:	
		static const int R_TABLE_WIDTH = 354;
		static const int TABLE_WIDTH_IN_CM = 177;
		static constexpr float PIXEL_TO_CM = TABLE_WIDTH_IN_CM / R_TABLE_WIDTH;
		
		cv::Point2f ul,ur,bl,br,ml,mr;
		cv::Point2f ulR,urR,blR,brR,mlR,mrR;
		cv::Mat H;
		cv::Mat H_inv;
		cv::Mat mask;
		//for Ball width calculations
		int height, upperWidth, lowerWidht, upLowDiff , upLowDiffPerHeigth;
		
		Table(cv::Mat& image);
		Table();
		bool hasMask();
		cv::Point2i rectify(cv::Point2i p);
		cv::Point2i backProject(cv::Point2i p);
	private:
		static const int GREEN_AND_OTHER_DIFF = 40;
	
		void segmentRGB(cv::Mat& rgbImg);
		std::vector<uint16_t> biggestRegion();
		void segmentField(cv::Mat& rgbImg,std::vector<uint16_t> stats);
};



#endif
