#include "Balls.hpp"

uint8_t Balls::kernel[1][5] = { {1,1,1,1,1} };
cv::Mat Balls::kernelMat = cv::Mat(1,5,CV_8U,kernel);


Balls::Balls(){

}

std::vector<std::vector<int>> Balls::colorTable = {
			{ 223 , 250 , 250 }, //white
			{ 40 , 40 , 150 }, //red
			{ 50 , 160 , 160 }, //yellow
			{ 70 , 120 , 20 }, //green
			{ 10 , 80 , 100 }, //brown
			{ 120 , 120 , 10 }, //blue
			{ 120 , 120 , 220 }, //pink
			{ 50 , 50 , 50 }, //black
};


void Balls::setTable(Table t){
	table = t;
}

int Balls::getBallWidth( int y ){
	return round( (y*table.upLowDiffPerHeigth+table.upperWidth)*TABLE_WIDTH_BALL_WIDTH_RATIO );
}

cv::Point2i Balls::getWhitePos(cv::Mat& image){
	cv::Mat maskROI = table.mask(cv::Rect(table.bl.x,table.ul.y,(table.br.x-table.bl.x),(table.br.y-table.ur.y)));
	cv::Mat imageROI = image(cv::Rect(table.bl.x,table.ul.y,(table.br.x-table.bl.x),(table.br.y-table.ur.y)));
	cv::Mat maskedROI(imageROI.size(), imageROI.type());
	cv::multiply(imageROI,maskROI,imageROI);
	cv::inRange(imageROI, cv::Scalar(180, 230, 200), cv::Scalar(255, 255, 255), maskedROI); //8u C1
	cv::Mat labelImage(imageROI.size(), CV_32S);
	cv::Mat stats(imageROI.size(), CV_32S);
	cv::Mat centroids(imageROI.size(), CV_32S);
	//cv::Mat centroidsImage(imageROI.size(), CV_8UC3);
	int n = cv::connectedComponentsWithStats(maskedROI,labelImage,stats,centroids);
	bool wasNewDetection = false;
	int minBallness = 1000;
	int minIndex = -1;
	for(int i=0;i<centroids.rows;i++){
		//cv::circle(centroidsImage,cv::Point2d(centroids.at<double>(i,0),centroids.at<double>(i,1)),5,cv::Scalar(128,128,128),3);
		if( !Matcher::pointDistLargerThan(lastWhitePos,cv::Point2i(centroids.at<double>(i,0),centroids.at<double>(i,1)),searchWindowWidth/2) || lastWhitePos.x == 0){
			int temp = 1000;
			if( (temp = checkBallness(imageROI,centroids.at<double>(i,0),centroids.at<double>(i,1))) < minBallness){
				minBallness = temp;
				minIndex = i;
			}
		}
	}
	if(minIndex != -1){
		lastWhitePos.x = round(centroids.at<double>(minIndex,0));
		lastWhitePos.y = round(centroids.at<double>(minIndex,1));
		wasNewDetection = true;
		unDetectedWhiteCnt = 0;
	}
	
	if(!wasNewDetection){
		minBallness = 1000;
		minIndex = -1;
		++unDetectedWhiteCnt;
		int uX = std::max(lastWhitePos.x-(searchWindowWidth/2)-unDetectedWhiteCnt*20,0);
		int uY = std::max(lastWhitePos.y-(searchWindowWidth/2)-unDetectedWhiteCnt*20,0);
		int w =  std::min(imageROI.cols-uX , searchWindowWidth-1+unDetectedWhiteCnt*40);
		int h =  std::min(imageROI.rows-uY , searchWindowWidth-1+unDetectedWhiteCnt*40);
		cv::Mat searchROI = imageROI(cv::Rect(uX,uY,w,h));
		cv::Mat maskedSeasrchROI(searchROI.size(),searchROI.type()); 
		cv::inRange(searchROI, cv::Scalar(150, 200, 180), cv::Scalar(255, 255, 255), maskedSeasrchROI);
		//cv::imshow("edges", searchROI);
        //cv::waitKey(0);
		int n = cv::connectedComponentsWithStats(maskedSeasrchROI,labelImage,stats,centroids);
		for(int i=0;i<n;i++){
			int temp = 1000;
			if( (temp=checkBallnessSoft( searchROI,centroids.at<double>(i,0),centroids.at<double>(i,1))) < minBallness){
				minBallness = temp;
				minIndex = i;
			}
		}
		if(minIndex != -1){
			lastWhitePos.x = uX + round(centroids.at<double>(minIndex,0));
			lastWhitePos.y = uY + round(centroids.at<double>(minIndex,1));
			wasNewDetection = true;
			unDetectedWhiteCnt = 0;
		}
		
		/*cv::cvtColor(searchROI, searchROI, CV_BGR2GRAY);
		cv::imshow("edges", searchROI);
        cv::waitKey(0);
		cv::medianBlur ( searchROI, searchROI, 21);
		cv::imshow("edges", searchROI);
        cv::waitKey(0);
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
	//std::cout << lastWhitePos << std::endl;
	return lastWhitePos;
}

int Balls::checkBallness(cv::Mat& imageROI, double x, double y){
	int xi = round(x);
	int yi = round(y);
	//std::cout << xi << " , " << yi << std::endl;
	int prevBlueValueRight = imageROI.at<cv::Vec3b>(yi,xi)[0]; //use RED values
	int prevBlueValueLeft = prevBlueValueRight;
	int prevBlueValueTop = prevBlueValueRight;
	int homogenousToRightEnd = -1;
	int homogenousToLeftEnd = -1;
	int homogenousToTopEnd = -1;
	int BALL_WIDTH = getBallWidth(yi);
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
		 	abs(homogenousToRightEnd-BALL_WIDTH/2)<BALL_WIDTH * SIGNIFICANT_CHANGE_TOL && 
		 	abs(homogenousToLeftEnd-BALL_WIDTH/2)<BALL_WIDTH * SIGNIFICANT_CHANGE_TOL && 
		 	abs(homogenousToTopEnd-BALL_WIDTH/2)<BALL_WIDTH * SIGNIFICANT_CHANGE_TOL+1 && 
		 	abs(homogenousToRightEnd-homogenousToLeftEnd)< BALL_WIDTH *SIGNIFICANT_CHANGE_TOL){
			return round(abs(1.5*BALL_WIDTH - (homogenousToRightEnd+homogenousToLeftEnd+homogenousToTopEnd)));
		}
	}
	return 1000;
}


int Balls::checkBallnessSoft(cv::Mat& imageROI, double x, double y){
	int xi = round(x);
	int yi = round(y);
	//std::cout << xi << " , " << yi << std::endl;
	int prevBlueValueRight = imageROI.at<cv::Vec3b>(yi,xi)[0]; //use RED values
	int prevBlueValueLeft = prevBlueValueRight;
	int prevBlueValueTop = prevBlueValueRight;
	int homogenousToRightEnd = -1;
	int homogenousToLeftEnd = -1;
	int homogenousToTopEnd = -1;
	int BALL_WIDTH = getBallWidth(yi);
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
		if( homogenousToRightEnd!=-1 && homogenousToLeftEnd!=-1 && homogenousToTopEnd!=-1 && abs(homogenousToLeftEnd + homogenousToRightEnd - BALL_WIDTH) < BALL_WIDTH && homogenousToTopEnd > BALL_WIDTH/8 && homogenousToTopEnd < BALL_WIDTH){
			//std::cout << "R: " << homogenousToRightEnd << "  L: " << homogenousToLeftEnd << "  T: " << homogenousToTopEnd << std::endl;
			return round(abs(1.5*BALL_WIDTH - (homogenousToRightEnd+homogenousToLeftEnd+homogenousToTopEnd)));
		}
	}
	//std::cout << "R: " << homogenousToRightEnd << "  L: " << homogenousToLeftEnd << "  T: " << homogenousToTopEnd << std::endl;
	return 1000;
}


std::vector<cv::Point2i> Balls::getWhiteAreas(cv::Mat& image){
	std::vector<cv::Point2i> cenroidPoints;
	cv::Mat maskROI = table.mask(cv::Rect(table.bl.x,table.ul.y,(table.br.x-table.bl.x),(table.br.y-table.ur.y)));
	cv::Mat imageROI = image(cv::Rect(table.bl.x,table.ul.y,(table.br.x-table.bl.x),(table.br.y-table.ur.y)));
	cv::Mat maskedROI(imageROI.size(), imageROI.type());
	cv::multiply(imageROI,maskROI,maskedROI);
	cv::cvtColor(maskedROI, maskedROI, CV_BGR2GRAY);
	cv::inRange(maskedROI, cv::Scalar(170), cv::Scalar(255), maskedROI); //8u C1
	cv::dilate(maskedROI,maskedROI,kernelMat);
	cv::Mat labelImage(imageROI.size(), CV_32S);
	cv::Mat stats(imageROI.size(), CV_32S);
	cv::Mat centroids(imageROI.size(), CV_32S);
	int n = cv::connectedComponentsWithStats(maskedROI,labelImage,stats,centroids);
	for(int i=1;i<n;i++){
		if(stats.at<int32_t>(i,cv::CC_STAT_AREA) < std::pow(getBallWidth(centroids.at<double>(i,1)),2)*4 ){
			cenroidPoints.push_back(cv::Point2i(round(centroids.at<double>(i,0)),round(centroids.at<double>(i,1))));
		}
	}
	//for(int i=0;i<centroids.rows;i++){
	//	cenroidPoints.push_back(cv::Point2i(round(centroids.at<double>(i,0)),round(centroids.at<double>(i,1))));
	//}
	return cenroidPoints;
}

int Balls::getBallColor(cv::Mat& image , cv::Point2i p){
	int ballWidth = getBallWidth(p.y);
	if( !Matcher::pointDistLargerThan(lastWhitePos,p,ballWidth/2) ){
		return WHITE;
	}
	cv::Vec3b pixel = image.at<cv::Vec3b>(p.y+ballWidth/4,p.x-ballWidth/4);
	int minDiff=1000;
	int minIndex=-1;
	for( int i=0;i<NUMBER_OF_COLORS;i++ ){
		int temp = abs(pixel[0]-colorTable[i][0])+abs(pixel[1]-colorTable[i][1])+abs(pixel[2]-colorTable[i][2]);
		if(temp < minDiff){
			minDiff = temp;
			minIndex = i;
		}
	}
	return minIndex;
}


void Balls::filterNearPoints(std::vector<cv::Point2i>& input){
	for( int i=0;i<input.size();i++){
		for( int j=0;j<input.size();j++ ){
			if( i==j ){
				continue;
			}
			if( !Matcher::pointDistLargerThan(input[i],input[j],getBallWidth(input[i].y)/2) ){
				input.erase(input.begin()+j);
			}
		}
	}
}

void Balls::findTemplates(cv::Mat& image, cv::Mat& white, cv::Mat& red, cv::Mat& yellow, cv::Mat& green, cv::Mat& brown, cv::Mat& blue, cv::Mat& pink, cv::Mat& black){
	//cv::Mat dst;
	//cv::medianBlur ( image, dst, 7 );
	//cv::cvtColor(image, dst, CV_BGR2GRAY);
	//cv::medianBlur ( image, dst, 7 );
	//image.convertTo(image, -1, 2, 0); -> increase contrast
	//cv::inRange(image, cv::Scalar(0, 0, 100), cv::Scalar(50, 50, 200), image); //red
	//cv::inRange(image, cv::Scalar(40, 230, 230), cv::Scalar(100, 255, 255), image); //yellow
	//cv::inRange(image, cv::Scalar(60, 130, 130), cv::Scalar(255, 255, 255), dst); //green
	//cv::inRange(dst,cv::Scalar(180),cv::Scalar(255),dst);
	//cv::imshow("findTemplate", dst);
	//cv::dilate(dst,dst,cv::Mat());
	//cv::imshow("findTemplateDilate", dst);
	//cv::waitKey(0);
}




