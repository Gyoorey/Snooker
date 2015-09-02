#include "Balls.hpp"

cv::Point2i Balls::getWhitePos(cv::Mat& image, Table& t){
	cv::Point2i whitePos(0,0);
	cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
	cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
	cv::multiply(imageROI,maskROI,imageROI);
	cv::inRange(imageROI, cv::Scalar(200, 200, 200), cv::Scalar(255, 255, 255), imageROI); //8u C1
	cv::Mat labelImage(imageROI.size(), CV_32S);
	cv::Mat stats(imageROI.size(), CV_32S);
	cv::Mat centroids(imageROI.size(), CV_32S);
	int n = cv::connectedComponentsWithStats(imageROI,labelImage,stats,centroids);
	for(int i=0;i<centroids.rows;i++){
		if(checkBallness(imageROI,centroids.at<float>(i,0),centroids.at<float>(i,1))){
			whitePos.x = round(centroids.at<float>(i,0));
			whitePos.y = round(centroids.at<float>(i,1));
			break;
		}
	}
	std::cout << whitePos << std::endl;
	return whitePos;
}

bool Balls::checkBallness(cv::Mat& imageROI, float x, float y){
	int xi = round(x);
	int yi = round(y);
	return true;
}
