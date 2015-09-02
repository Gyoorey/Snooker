#ifndef TABLE_HPP
#define TABLE_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class Table{
	public:
		cv::Point2f ul,ur,bl,br,ml,mr;
		cv::Point3f ulR,urR,blR,brR,mlR,mrR;
		cv::Mat H;
		cv::Mat mask;
		
		Table(Mat& image);
		bool hasMask();
	private:
		Mat* segmentRGB(Mat& rgbImg);
		Mat* biggestRegion(Mat& segmImg);
		Mat* segmentField(Mat& rgbImg, Mat& segmImg);
};



#endif
