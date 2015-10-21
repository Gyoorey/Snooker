#include "BallSegmenter.hpp"

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

void Balls::setPrevImage(cv::Mat* image,int mode){
	if(mode==1){
		prev_image = cv::Mat::zeros(image->rows,image->cols,CV_8UC1);
		return;
	}
	prev_image = cv::Mat::zeros(image->rows,image->cols,CV_8UC3);
}


void Balls::setTable(TableParams* t){
	table = t;
}

int Balls::getBallWidth( int y ){
	return round( (y*table->upLowDiffPerHeigth+table->upperWidth)*TABLE_WIDTH_BALL_WIDTH_RATIO );
}

cv::Point2i Balls::getWhitePos(cv::Mat* image){
	cv::Mat maskedROI(image->size(), image->type());
	cv::inRange(*image, cv::Scalar(180, 230, 200), cv::Scalar(255, 255, 255), maskedROI); //8u C1
	cv::Mat labelImage(image->size(), CV_32S);
	cv::Mat stats(image->size(), CV_32S);
	cv::Mat centroids(image->size(), CV_32S);
	//cv::Mat centroidsImage(imageROI.size(), CV_8UC3);
	int n = cv::connectedComponentsWithStats(maskedROI,labelImage,stats,centroids);
	bool wasNewDetection = false;
	int minBallness = 1000;
	int minIndex = -1;
	for(int i=0;i<centroids.rows;i++){
		//cv::circle(centroidsImage,cv::Point2d(centroids.at<double>(i,0),centroids.at<double>(i,1)),5,cv::Scalar(128,128,128),3);
		if( !Matcher::pointDistLargerThan(lastWhitePos,cv::Point2i(centroids.at<double>(i,0),centroids.at<double>(i,1)),searchWindowWidth/2) || lastWhitePos.x == 0){
			int temp = 1000;
			if( (temp = checkBallness(*image,centroids.at<double>(i,0),centroids.at<double>(i,1))) < minBallness){
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
		int w =  std::min(image->cols-uX , searchWindowWidth-1+unDetectedWhiteCnt*40);
		int h =  std::min(image->rows-uY , searchWindowWidth-1+unDetectedWhiteCnt*40);
		cv::Mat searchROI = (*image)(cv::Rect(uX,uY,w,h));
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


std::vector<cv::Point2f> Balls::getWhiteAreas(cv::Mat* image){
	std::vector<cv::Point2f> cenroidPoints;
	cv::Mat gray;
	if(image->channels() == 3){
		cv::cvtColor(*image, gray, CV_BGR2GRAY);	
	}else{
		gray = image->clone();
	}
	
	cv::inRange(gray, cv::Scalar(150), cv::Scalar(255), gray); //8u C1
	cv::dilate(gray,gray,kernelMat);
	cv::Mat labelImage(gray.size(), CV_32S);
	cv::Mat stats(gray.size(), CV_32S);
	cv::Mat centroids(gray.size(), CV_32S);
	int n = cv::connectedComponentsWithStats(gray,labelImage,stats,centroids);
	for(int i=1;i<n;i++){
		if(stats.at<int32_t>(i,cv::CC_STAT_AREA) < std::pow(getBallWidth(centroids.at<double>(i,1)),2)*4 ){
			cenroidPoints.push_back(cv::Point2f((centroids.at<double>(i,0)),(centroids.at<double>(i,1))));
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
	
}

void Balls::findTemplates(cv::Mat* image){
	//generate red template
	int ballWidth = getBallWidth(image->rows/2);
	cv::Mat redTemplate = cv::Mat::zeros(ballWidth/2,ballWidth,CV_8UC3);
	cv::circle(redTemplate,cv::Point(redTemplate.cols/2,redTemplate.rows),ballWidth/2,cv::Scalar(255,0,0),-1);
	cv::Mat result;
	cv::matchTemplate(*image,redTemplate,result, CV_TM_CCOEFF_NORMED);
    normalize( result, result, 0, 255, cv::NORM_MINMAX, -1, cv::Mat() );
    double minVal, maxVal;
  	cv::Point2i  minLoc, maxLoc, matchLoc;
	cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
	cv::inRange(result, cv::Scalar(0.99*maxVal), cv::Scalar(maxVal), result);
	*image = result.clone();
    //cv::imshow("edges", result);
    //cv::waitKey(0);
}

void Balls::subtractImages(cv::Mat* image){
	cv::Mat temp = image->clone();
	cv::subtract(*image,prev_image,*image); 
	prev_image = temp.clone();
}

void Balls::drawMatchings(cv::Mat* image,std::vector<std::vector<cv::Point2f> > matchings){
	//cv::cvtColor(*image,*image,CV_GRAY2BGR);
	for(int i=0;i<matchings.size();i++){
		cv::circle(*image,matchings[i][0],5,cv::Scalar(255-i*10,50+i*10,128),-1);
		cv::circle(*image,matchings[i][1],5,cv::Scalar(255-i*10,50+i*10,128),-1);
	}
}

void Balls::drawTrajectories(cv::Mat* image,std::map<int,std::vector<cv::Point2f>> trajectories){
	//cv::cvtColor(*image,*image,CV_GRAY2BGR);
	for( std::map<int,std::vector<cv::Point2f>>::iterator it=trajectories.begin() ;  it!=trajectories.end() ; it++ ){
		std::vector<cv::Point2f> points = it->second;
		int color = it->first;
		for( int i=0 ; i<points.size(); i++ ){
			cv::circle(*image,points[i],3,cv::Scalar(255-color*10,color*10,0),-1);
		}
	}
}

std::vector<std::pair<cv::Point2f,int>> Balls::getWhiteRegions(cv::Mat* image){
	std::vector<std::pair<cv::Point2f,int>> centers;
	cv::Mat labelImage(image->size(), CV_32S);
	cv::Mat stats(image->size(), CV_32S);
	cv::Mat centroids(image->size(), CV_32S);
	int n = cv::connectedComponentsWithStats(*image,labelImage,stats,centroids);
	for(int i=1;i<n;i++){
		centers.push_back(std::pair<cv::Point2f,int>(cv::Point2f((centroids.at<double>(i,0)),(centroids.at<double>(i,1))),(int)(stats.at<int32_t>(i,cv::CC_STAT_AREA)/250)) );
		//std::pow(0.5*getBallWidth(centroids.at<double>(i,1)),2)*3.14
	}
	return centers;
}



