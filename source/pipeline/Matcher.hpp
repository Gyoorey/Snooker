#ifndef MATCHER_HPP
#define MATCHER_HPP

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
#include <mutex>
#include "Table.hpp"

class Matcher{
	public:	
		static const int EQUAL_POS_TOL = 2;
		static const int MAX_ITERATION = 100;
		Matcher();
		void store(std::vector<cv::Point2i> temp);
		std::vector<cv::Point2i> getCollinearPointsToInitialPos(std::vector<std::vector<cv::Point2i>> unmatchings, cv::Point2i initial);
		std::vector<cv::Point2i> getCollinearPoints();
		std::vector<cv::Point2i> getMatchingPoints();
		std::vector<std::vector<cv::Point2i>> getUnmatchingPoints();
		static bool pointDistLargerThan(cv::Point2i p1, cv::Point2i p2, const int tol);
		static int pointDistanceFromLine(cv::Point2i x0, cv::Point2i x1, cv::Point2i p);
		void reset();
		static std::vector<std::vector<cv::Point2f> > iterativePointMatching(std::vector<cv::Point2f> p1 , std::vector<cv::Point2f> p2);
		static std::vector<std::vector<cv::Point2f> > iterativePointMatching2(std::vector<cv::Point2f> p1 , cv::Mat* image, cv::Point2i whitePos , cv::Point2i prev_whitePos, cv::Mat absDiffImage);
	private:
		std::vector<std::vector<cv::Point2i>> storedPositions;
		int counter;
		std::mutex guard;

};



#endif
