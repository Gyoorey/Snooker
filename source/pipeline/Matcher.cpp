#include "Matcher.hpp"

Matcher::Matcher(){
	counter = 0;
	storedPositions = std::vector<std::vector<cv::Point2i>>();
}

void Matcher::store(std::vector<cv::Point2i> temp){
	while(!guard.try_lock()){ //talan, ha zart ki kellenne lepnem
	}
	storedPositions.push_back(temp);
	counter++;
	guard.unlock();
}

void Matcher::reset(){
	while(!guard.try_lock()){
	}
	counter = 0;
	storedPositions.clear();
	guard.unlock();
}

std::vector<std::vector<cv::Point2i>> Matcher::getUnmatchingPoints(){
	std::vector<std::vector<cv::Point2i>> unmatchings;
	for( int i=0;i<counter;i++){ //goes through the storedPositions
		//std::cout << "i:" << i << std::endl;
		for( int j=0;j<storedPositions[i].size();j++ ){ //goes through the actual frame
			cv::Point2i actualPosition = storedPositions[i][j];
			//std::cout << "j:" <<  j << "  actPos: " << actualPosition << std::endl;
			for( int k=i+1;k<counter;k++ ){ //goes through the next frames
				//std::cout << "k:" <<  k << std::endl;
				for( int l=0;l<storedPositions[k].size();l++){ //goes through the next frames positions
					//std::cout << "l:" <<  l << std::endl;
					if(/*actualPosition == storedPositions[k][l]*/ !pointDistLargerThan(actualPosition,storedPositions[k][l],EQUAL_POS_TOL) ){ //if there are matching
						//std::cout << "Match" << std::endl;
						for( int m=0;m<counter;m++){ //delete all instances
							//std::cout << "m:" <<  m << std::endl;
							for( int n=0;n<storedPositions[m].size();n++){
								//std::cout << "n:" <<  n << std::endl;
								if(/*actualPosition == storedPositions[m][n]*/ !pointDistLargerThan(actualPosition,storedPositions[m][n],EQUAL_POS_TOL)){
									storedPositions[m].erase(storedPositions[m].begin()+n);
									break;
								}
							}
						}
						break;
					}
				}
			}
		}
	}
	
	//TODO: skip this and give back storedPositions
	for( int i=0;i<counter;i++){
		if(storedPositions[i].size()>0){
			unmatchings.push_back(std::vector<cv::Point2i>());
		}else{
			continue;
		}
		for( int j=0;j<storedPositions[i].size();j++ ){
			unmatchings.back().push_back(storedPositions[i][j]);
		}
	}
	for( int i=0;i<unmatchings.size();i++){
		for( int j=0;j<unmatchings[i].size();j++ ){
			std::cout << unmatchings[i][j] << " ";
		}
		std::cout << std::endl;
	}
	return unmatchings;
}

std::vector<cv::Point2i> Matcher::getCollinearPointsToInitialPos(std::vector<std::vector<cv::Point2i>> unmatchings, cv::Point2i initial){
	std::vector<cv::Point2i> matchings;
	//TODO: Make it const
	int TOL = 2;
	for( int i=0;i<unmatchings.size();i++ ){
		for( int j=0;j<unmatchings[i].size();j++ ){
			bool wasHit = false;
			for( int k=i+1;k<unmatchings.size();k++ ){
				for( int l=0;l<unmatchings[k].size();l++ ){
					if(pointDistanceFromLine(initial,unmatchings[i][j],unmatchings[k][l]) <= TOL){
						matchings.push_back(unmatchings[k][l]);
						unmatchings[k].erase(unmatchings[k].begin()+l);
						wasHit = true;
						break;
					}
				}
			}
			if(wasHit){
				matchings.push_back(unmatchings[i][j]);
				unmatchings[i].erase(unmatchings[i].begin()+j);
				break;
			}
		}
	}
	/*for( int i=0;i<unmatchings.size();i++ ){
		for( int j=0;j<unmatchings[i].size();j++ ){
			matchings.push_back(unmatchings[i][j]);
			bool wasHit = false;
			for( int k=0;k<matchings.size();k++ ){
				for( int l=i+1;l<unmatchings.size();l++ ){
					for( int m=0;m<unmatchings[k].size();m++ ){
						if(pointDistanceFromLine(initial,unmatchings[i][j],unmatchings[k][l]) <= TOL){
							matchings.push_back(unmatchings[k][l]);
							unmatchings[k].erase(unmatchings[k].begin()+l);
							wasHit = true;
							break;
						}
					}
				}
			}
		}
	}*/
	return matchings;
}

std::vector<cv::Point2i> Matcher::getCollinearPoints(){
	std::vector<cv::Point2i> unmatchings;
	return unmatchings;
}

std::vector<cv::Point2i> Matcher::getMatchingPoints(){
	std::vector<cv::Point2i> unmatchings;
	return unmatchings;
}

int Matcher::pointDistanceFromLine(cv::Point2i x0, cv::Point2i x1, cv::Point2i p){
	if(x0==p || x1==p){
		return 0;
	}
	cv::Point2i temp1(x0 - x1);
	cv::Point2i temp2(x1 - p);
	int denum = (int)std::sqrt(temp1.x*temp1.x + temp1.y*temp1.y);
	if(denum == 0)
		return 0;
	return abs(temp1.x * temp2.y - temp1.y * temp2.x) / denum;
}

bool Matcher::pointDistLargerThan(cv::Point2i p1, cv::Point2i p2, const int tol){
	return ( abs(p1.x-p2.x)>tol || abs(p1.y-p2.y)>tol );
}

std::vector<std::vector<cv::Point2f> > Matcher::iterativePointMatching(std::vector<cv::Point2f> p1 , std::vector<cv::Point2f> p2){
	std::vector<std::vector<cv::Point2f> > matchings;
	for(int i=0;i<MAX_ITERATION;i++){
		if(p1.empty() || p2.empty()){
			return matchings;
		}
		for(int j=0;j<p1.size();j++){
			for(int k=0;k<p2.size();k++){
				if( std::max(abs(p1[j].x-p2[k].x), abs(p1[j].y-p2[k].y))< float(i+1) ){
					std::vector<cv::Point2f> match;
					match.push_back(p1[j]);
					match.push_back(p2[k]);
					matchings.push_back(match);
					p1.erase(p1.begin()+j);
					p2.erase(p2.begin()+k);
					break;
				}
			}
		}
	}
	return matchings;
}

std::vector<std::vector<cv::Point2f> > Matcher::iterativePointMatching2(std::vector<cv::Point2f> p1 , cv::Mat* image, cv::Point2i whitePos, cv::Point2i prev_whitePos, cv::Mat absDiffImage){
	uint8_t kernel[1][5] = { {1,1,1,1,1} };
	cv::Mat kernelMat = cv::Mat(1,5,CV_8U,kernel);
	std::vector<std::vector<cv::Point2f> > matchings;
	cv::Mat gray;
	for( int thres = 150 ; thres>=130 ; thres-=30 ){
		if(image->channels() == 3){
			cv::cvtColor(*image, gray, CV_BGR2GRAY);	
		}else{
			gray = image->clone();
		}
		cv::inRange(gray, cv::Scalar(thres), cv::Scalar(255), gray); //8u C1
		cv::dilate(gray,gray,kernelMat);
		cv::Mat labelImage(gray.size(), CV_32S);
		cv::Mat stats(gray.size(), CV_32S);
		cv::Mat centroids(gray.size(), CV_32S);
		std::vector<cv::Point2f> cenroidPoints;
		int n = cv::connectedComponentsWithStats(gray,labelImage,stats,centroids);
		for(int ii=0;ii<n;ii++){
			if(stats.at<int32_t>(ii,cv::CC_STAT_AREA) < std::pow(8,2)*4 ){
				cenroidPoints.push_back(cv::Point2f((centroids.at<double>(ii,0)),(centroids.at<double>(ii,1))));
				cv::circle(absDiffImage,cv::Point2f((centroids.at<double>(ii,0)),(centroids.at<double>(ii,1))),20,cv::Scalar(0,0,0),-1);
			}
		}
		for(int ii=0;ii<p1.size();ii++){
			cv::circle(absDiffImage,p1[ii],20,cv::Scalar(0,0,0),-1);
		}
		cv::circle(absDiffImage,whitePos,20,cv::Scalar(0,0,0),-1);
		cv::circle(absDiffImage,prev_whitePos,20,cv::Scalar(0,0,0),-1);
		n = cv::connectedComponentsWithStats(absDiffImage,labelImage,stats,centroids);
		for(int ii=0;ii<n;ii++){
				cenroidPoints.push_back(cv::Point2f((centroids.at<double>(ii,0)),(centroids.at<double>(ii,1))));
		}
		for(int i=1;i<MAX_ITERATION;i=i+i){
			if(p1.empty()){
				return matchings;
			}
			for(int j=p1.size()-1;j>=0;j--){
				if(p1.size()==0){
					break;
				}
				cv::Point2f point = p1[j];
				for(int k=0;k<cenroidPoints.size();k++){
					if( !pointDistLargerThan(whitePos,cenroidPoints[k],5)){
						continue;
					}
					if( std::max(abs(point.x-cenroidPoints[k].x), abs(point.y-cenroidPoints[k].y))< float(i+1.0) ){
						std::vector<cv::Point2f> match;
						match.push_back(point);
						match.push_back(cenroidPoints[k]);
						matchings.push_back(match);
						p1.erase(p1.begin()+j);
						//cv::circle(*image,cenroidPoints[k],10,cv::Scalar(0,0,0),-1);
						cenroidPoints.erase(cenroidPoints.begin()+k);
						break;
					}
				}
			}
		}
	}
	return matchings;
}






