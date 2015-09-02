#include "Balls.hpp"

cv::Point2i Balls::getWhitePos(cv::Mat& image, Table& t){
	cv::Point2i whitePos(0,0);
	cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
	cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
	cv::Mat maskedROI(imageROI.size(), imageROI.type());
	cv::multiply(imageROI,maskROI,maskedROI);
	cv::inRange(maskedROI, cv::Scalar(200, 240, 200), cv::Scalar(255, 255, 255), maskedROI); //8u C1
	cv::Mat labelImage(imageROI.size(), CV_32S);
	cv::Mat stats(imageROI.size(), CV_32S);
	cv::Mat centroids(imageROI.size(), CV_32S);
	//cv::Mat centroidsImage(imageROI.size(), CV_8UC3);
	int n = cv::connectedComponentsWithStats(maskedROI,labelImage,stats,centroids);
	for(int i=0;i<centroids.rows;i++){
		//cv::circle(centroidsImage,cv::Point2d(centroids.at<double>(i,0),centroids.at<double>(i,1)),5,cv::Scalar(128,128,128),3);
		if(checkBallness(imageROI,centroids.at<double>(i,0),centroids.at<double>(i,1))){
			whitePos.x = round(centroids.at<double>(i,0));
			whitePos.y = round(centroids.at<double>(i,1));
			break;
		}
	}
	//cv::imshow("edges", centroidsImage);
	//cv::waitKey(0);
	std::cout << whitePos << std::endl;
	return whitePos;
}

bool Balls::checkBallness(cv::Mat& imageROI, double x, double y){
	int xi = round(x);
	int yi = round(y);
	std::cout << xi << " , " << yi << std::endl;
	int prevRedValue = imageROI.at<cv::Vec3b>(yi,xi)[2]; //use RED values
	std::cout << imageROI.at<cv::Vec3b>(yi,xi) << std::endl;
	int homogenousToRightEnd = 0;
	for(int i=xi+1;i<xi+2*BALL_WIDTH;i++){
		if(i > imageROI.cols){
			break;
		}
		std::cout << "prev: " << int(prevRedValue) << "   " << "act: " << int(imageROI.at<cv::Vec3b>(yi,i)[2]) << std::endl;
		if(imageROI.at<cv::Vec3b>(yi,i)[2] < round(prevRedValue*SIGNIFICANT_CHANGE_RATIO)){
			homogenousToRightEnd = i-xi;
			std::cout << homogenousToRightEnd << std::endl;
			break;
		}else{
			prevRedValue = imageROI.at<cv::Vec3b>(yi,i)[2];
		}
	}
	return false;
}
