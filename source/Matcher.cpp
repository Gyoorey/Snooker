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


