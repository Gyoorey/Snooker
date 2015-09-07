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
		cv::Point2f ul,ur,bl,br,ml,mr;
		cv::Point2f ulR,urR,blR,brR,mlR,mrR;
		cv::Mat H;
		cv::Mat mask;
		
		Table(cv::Mat& image);
		bool hasMask();
	private:
		void segmentRGB(cv::Mat& rgbImg);
		std::vector<uint16_t> biggestRegion();
		void segmentField(cv::Mat& rgbImg,std::vector<uint16_t> stats);
};



#endif
