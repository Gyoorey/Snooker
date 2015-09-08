#include "Balls.hpp"


cv::Point2i Balls::getWhitePos(cv::Mat& image, Table& t){
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
	bool wasNewDetection = false;
	for(int i=0;i<centroids.rows;i++){
		//cv::circle(centroidsImage,cv::Point2d(centroids.at<double>(i,0),centroids.at<double>(i,1)),5,cv::Scalar(128,128,128),3);
		if(checkBallness(imageROI,centroids.at<double>(i,0),centroids.at<double>(i,1))){
			lastWhitePos.x = round(centroids.at<double>(i,0));
			lastWhitePos.y = round(centroids.at<double>(i,1));
			wasNewDetection = true;
			unDetectedWhiteCnt = 0;
			break;
		}
	}
	if(!wasNewDetection){
		int uX = std::max(lastWhitePos.x-searchWindowWidth/2,0);
		int uY = std::max(lastWhitePos.y-searchWindowWidth/2,0);
		int w =  std::min(imageROI.cols-uX , searchWindowWidth-1);
		int h =  std::min(imageROI.rows-uY , searchWindowWidth-1);
		cv::Mat searchROI = imageROI(cv::Rect(uX,uY,w,h));
		cv::cvtColor(searchROI, searchROI, CV_BGR2GRAY);
		double minVal; 
		double maxVal; 
		cv::Point2i minLoc; 
		cv::Point2i maxLoc;
		cv::minMaxLoc( searchROI, &minVal, &maxVal, &minLoc, &maxLoc );
		cv::circle(searchROI,maxLoc,5,cv::Scalar(255),3);
		lastWhitePos.x = uX+maxLoc.x;
		lastWhitePos.y = uY+maxLoc.y;	
		
		//TODO: Need some filtering
		wasNewDetection = true;
		unDetectedWhiteCnt = 0;
		//cv::imshow("edges", searchROI);
        //cv::waitKey(0);
        return lastWhitePos;
		//cv::inRange(searchROI, cv::Scalar(220, 245, 220), cv::Scalar(255, 255, 255), searchROI);
		//int n = cv::connectedComponentsWithStats(searchROI,labelImage,stats,centroids);
		/*for(int i=0;i<n;i++){
			cv::circle(searchROI,cv::Point2i(centroids.at<double>(i,0),centroids.at<double>(i,1)),5,cv::Scalar(200,200,200),3);	
			std::cout << i << ".: " << cv::Point2i(centroids.at<double>(i,0),centroids.at<double>(i,1)) << std::endl;
		}
		if(n==2){
			if(searchROI.at<uint8_t>(round(centroids.at<double>(1,0)),round(centroids.at<double>(1,1)) ) == 255){
				lastWhitePos.x = uX+round(centroids.at<double>(1,0));
				lastWhitePos.y = uY+round(centroids.at<double>(1,1));
			}else{
				lastWhitePos.x = uX+round(centroids.at<double>(0,0));
				lastWhitePos.y = uY+round(centroids.at<double>(0,1));
			}
			wasNewDetection = true;
			unDetectedWhiteCnt = 0;
			std::cout << "New pos: " << cv::Point2i(centroids.at<double>(1,0),centroids.at<double>(1,1)) << std::endl;
		}else{
			for(int i=0;i<n;i++){
				cv::circle(searchROI,cv::Point2i(centroids.at<double>(i,0),centroids.at<double>(i,1)),5,cv::Scalar(200,200,200),3);	
				std::cout << cv::Point2i(centroids.at<double>(i,0),centroids.at<double>(i,1)) << std::endl;
			}
		}*/
		//cv::imshow("edges", searchROI);
        //cv::waitKey(0);
	}
	//cv::imshow("edges", centroidsImage);
	//cv::waitKey(0);
	if(!wasNewDetection){
		++unDetectedWhiteCnt;
		return cv::Point2i(0,0);
	}
	std::cout << lastWhitePos << std::endl;
	return lastWhitePos;
}

bool Balls::checkBallness(cv::Mat& imageROI, double x, double y){
	int xi = round(x);
	int yi = round(y);
	//std::cout << xi << " , " << yi << std::endl;
	int prevBlueValueRight = imageROI.at<cv::Vec3b>(yi,xi)[0]; //use RED values
	int prevBlueValueLeft = prevBlueValueRight;
	int prevBlueValueTop = prevBlueValueRight;
	int homogenousToRightEnd = -1;
	int homogenousToLeftEnd = -1;
	int homogenousToTopEnd = -1;
	for(int i=1;i<2*BALL_WIDTH;i++){ //TODO: Catch out of bound exception
		//std::cout << "prevR: " << int(prevBlueValueRight) << "   " << "actR: " << int(imageROI.at<cv::Vec3b>(yi,i)[2]) << "     prevL: " << int(prevBlueValueLeft) << "    actL: " << int(imageROI.at<cv::Vec3b>(yi,xi-i)[2]) << std::endl;
		if( (imageROI.at<cv::Vec3b>(yi,xi+i)[0] < round(prevBlueValueRight*SIGNIFICANT_CHANGE_RATIO) || imageROI.at<cv::Vec3b>(yi,xi+i)[0]<BLUE_LOW) && homogenousToRightEnd==-1 && imageROI.at<cv::Vec3b>(yi,xi+i)[1]!=0){
			homogenousToRightEnd = i;
			//std::cout << homogenousToRightEnd << std::endl;
		}else{
			prevBlueValueRight = imageROI.at<cv::Vec3b>(yi,xi+i)[0];
		}
		if( (imageROI.at<cv::Vec3b>(yi,xi-i)[0] < round(prevBlueValueLeft*SIGNIFICANT_CHANGE_RATIO) || imageROI.at<cv::Vec3b>(yi,xi-i)[0]<BLUE_LOW) && homogenousToLeftEnd==-1 && imageROI.at<cv::Vec3b>(yi,xi-i)[1]!=0){
			homogenousToLeftEnd = i;
			//std::cout << homogenousToRightEnd << std::endl;
		}else{
			prevBlueValueLeft = imageROI.at<cv::Vec3b>(yi,xi-i)[0];
		}
		if( (imageROI.at<cv::Vec3b>(yi-i,xi)[0] < round(prevBlueValueTop*SIGNIFICANT_CHANGE_RATIO) || imageROI.at<cv::Vec3b>(yi-i,xi)[0]<BLUE_LOW) && homogenousToTopEnd==-1 && imageROI.at<cv::Vec3b>(yi-i,xi)[1]!=0){
			homogenousToTopEnd = i;
			//std::cout << homogenousToRightEnd << std::endl;
		}else{
			prevBlueValueTop = imageROI.at<cv::Vec3b>(yi-i,xi)[0];
		}
		if(homogenousToRightEnd!=-1 && homogenousToLeftEnd!=-1 && homogenousToTopEnd!=-1 &&
		 	abs(homogenousToRightEnd-BALL_WIDTH/2)<SIGNIFICANT_CHANGE_TOL && 
		 	abs(homogenousToLeftEnd-BALL_WIDTH/2)<SIGNIFICANT_CHANGE_TOL && 
		 	abs(homogenousToTopEnd-BALL_WIDTH/2)<SIGNIFICANT_CHANGE_TOL+1 && 
		 	abs(homogenousToRightEnd-homogenousToLeftEnd)<SIGNIFICANT_CHANGE_TOL){
			return true;
		}
	}
	return false;
}


std::vector<cv::Point2i> Balls::getWhiteAreas(cv::Mat& image, Table& t){
	std::vector<cv::Point2i> cenroidPoints;
	cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
	cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
	cv::Mat maskedROI(imageROI.size(), imageROI.type());
	cv::multiply(imageROI,maskROI,maskedROI);
	cv::inRange(maskedROI, cv::Scalar(140, 140, 140), cv::Scalar(255, 255, 255), maskedROI); //8u C1
	cv::Mat labelImage(imageROI.size(), CV_32S);
	cv::Mat stats(imageROI.size(), CV_32S);
	cv::Mat centroids(imageROI.size(), CV_32S);
	int n = cv::connectedComponentsWithStats(maskedROI,labelImage,stats,centroids);
	for(int i=0;i<centroids.rows;i++){
		cenroidPoints.push_back(cv::Point2i(round(centroids.at<double>(i,0)),round(centroids.at<double>(i,1))));
	}
	return cenroidPoints;
}

