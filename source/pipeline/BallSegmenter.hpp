#ifndef BALL_SEGMENTER_HPP
#define BALL_SEGMENTER_HPP

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
#include <map>
#include "Matcher.hpp"
#include "Pipe.hpp"
#include "Queue.hpp"
#include "Table.hpp"


class Balls{
	public:		
		static constexpr float SIGNIFICANT_CHANGE_TOL = 0.25;
		static const unsigned short BLUE_LOW = 130;
		static constexpr float TABLE_WIDTH_BALL_WIDTH_RATIO = 0.035;
		static const int searchWindowWidth = 100;
		static constexpr float SIGNIFICANT_CHANGE_RATIO = 0.8;
		static std::vector<std::vector<int>> colorTable;
		static const int NUMBER_OF_COLORS = 8;
		static uint8_t kernel[1][5];
		static cv::Mat kernelMat; 
		enum colorIndex{
			WHITE,
			RED,
			YELLOW,
			GREEN,
			BROWN,
			BLUE,
			PINK,
			BLACK
		};
		
		TableParams* table;
		Balls();
		cv::Mat prev_image;
		
		int getBallColor(cv::Mat& image , cv::Point2i p);
		std::vector<cv::Point2f> getWhiteAreas(cv::Mat* image);
		void filterNearPoints(std::vector<cv::Point2i>& input);
		int getBallWidth( int y );
		void setTable(TableParams* t);
		void findTemplates(cv::Mat& image, cv::Mat& white, cv::Mat& red, cv::Mat& yellow, cv::Mat& green, cv::Mat& brown, cv::Mat& blue, cv::Mat& pink, cv::Mat& black);
		void findTemplates(cv::Mat* image);
		cv::Point2i getWhitePos(cv::Mat* image);
		void subtractImages(cv::Mat* image);
		void setPrevImage(cv::Mat* image,int mode);
		void drawMatchings(cv::Mat* image,std::vector<std::vector<cv::Point2f> > matchings);
		void drawTrajectories(cv::Mat* image,std::map<int,std::vector<cv::Point2f>> trajectories);
		std::vector<std::pair<cv::Point2f,int>> getWhiteRegions(cv::Mat* image);
	private:
		int checkBallnessSoft(cv::Mat& imageROI, double x, double y);
		int checkBallness(cv::Mat& imageROI, double x, double y);
		cv::Point2i lastWhitePos;
		int unDetectedWhiteCnt;
		
		
};

class ComparePoints { // simple comparison function
   	public:
      	bool operator()(cv::Point2f a,cv::Point2f b) { 
      		return (a.x!=b.x)?(a.x<b.x):(a.y<b.y);
     	}
};

template<class DATA_IN,class DATA_OUT>
class BallSegmenter : public Pipe<DATA_IN,DATA_OUT>{
	public:
		
		BallSegmenter(Queue<DATA_IN>& input , Queue<DATA_OUT>& output):Pipe<DATA_IN,DATA_OUT>(input,output),ballsInit(false),balls(){
			}
		BallSegmenter(){
			}
		
		void work(){
			std::vector<cv::Point2f> prevCentroids;
			std::vector<cv::Point2f> currentCentroids;
			std::map<cv::Point2f,int,ComparePoints> pointIndeces;
			std::map<int,std::vector<cv::Point2f>> trajectories;
			cv::Point2f prev_whitePos;
			while(1){
				DATA_IN input = Pipe<DATA_IN,DATA_OUT>::in.pop();
				Pipe<DATA_IN,DATA_OUT>::out.push(input.image);
				continue;
				if(!ballsInit){
					balls.setTable(input.tp);
					balls.setPrevImage(input.image,3);
					prevCentroids = balls.getWhiteAreas(input.image);
					cv::Point2i whitePos = balls.getWhitePos(input.image);
					prev_whitePos = whitePos;
					for(int i=0;i<prevCentroids.size();i++){
						if(!Matcher::pointDistLargerThan(prevCentroids[i],whitePos,5)){
							continue;
						}
						pointIndeces.insert(std::pair<cv::Point2f,int>(prevCentroids[i],i+2));
						trajectories.insert(std::pair<int,std::vector<cv::Point2f>>(i+2,std::vector<cv::Point2f>()));
						trajectories[i+2].push_back(prevCentroids[i]);
					}
					trajectories.insert(std::pair<int,std::vector<cv::Point2f>>(1,std::vector<cv::Point2f>()));
					trajectories[1].push_back(whitePos);
					//balls.subtractImages(input.image);
					Pipe<DATA_IN,DATA_OUT>::out.push(input.image);
					ballsInit = true;
				}else{
					/*std::vector<std::pair<cv::Point2f,int>> centers = balls.getWhiteRegions(input.image);
					cv::cvtColor(*(input.image),*(input.image),CV_GRAY2BGR);
					for(int i=0;i<centers.size();i++){
						cv::circle(*(input.image),centers[i].first,3,cv::Scalar(0,0,255),-1);
						cv::putText(*(input.image),std::to_string(centers[i].second),centers[i].first,1,1,cv::Scalar(0,0,255));
					}*/
					//cv::cvtColor(*(input.image),*(input.image),CV_BGR2GRAY);
					//cv::adaptiveThreshold(*(input.image),*(input.image),255,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY,3,0);
					//std::vector<cv::Mat> rgb;
					//cv::split(*(input.image), rgb);
					//*(input.image) = rgb[2].clone();
					//cv::inRange(*(input.image), cv::Scalar(150), cv::Scalar(255), *(input.image));
					cv::Mat absDiffImage;
					cv::absdiff(*(input.image),balls.prev_image,absDiffImage);
					balls.prev_image = input.image->clone();
					cv::inRange(absDiffImage, cv::Scalar(8,8,8), cv::Scalar(255,255,255), absDiffImage);
					//Pipe<DATA_IN,DATA_OUT>::out.push(input.image);
					//continue;
					//currentCentroids = balls.getWhiteAreas(input.image);
					cv::Point2i whitePos = balls.getWhitePos(input.image);
					trajectories[1].push_back(whitePos);
					std::vector<std::vector<cv::Point2f> > matchings = Matcher::iterativePointMatching2(prevCentroids,input.image,whitePos,prev_whitePos,absDiffImage);
					//for( std::map<cv::Point2f,int,ComparePoints>::iterator it = pointIndeces.begin() ; it!=pointIndeces.end() ; it++ ){
					//	std::cout << it->first << " : " << it->second  << std::endl;
					//}
					prevCentroids.clear();
					for(int i=0;i<matchings.size();i++){
						prevCentroids.push_back(matchings[i][1]);
					}
					for(int i=0;i<matchings.size();i++){
						if(matchings[i][1] == matchings[i][0]){
							continue;
						}
						int oldIndex = pointIndeces[matchings[i][0]];
						if(oldIndex == 0){
							continue;
						}
						trajectories[oldIndex].push_back(matchings[i][1]);
						std::map<cv::Point2f,int,ComparePoints>::iterator it;
						it = pointIndeces.find(matchings[i][0]);
						pointIndeces.erase(it);
						pointIndeces.insert(std::pair<cv::Point2f,int>(matchings[i][1],oldIndex));
					}
					//balls.drawMatchings(input.image,matchings);
					balls.drawTrajectories(input.image,trajectories);
					prev_whitePos = whitePos;
					//prevCentroids = currentCentroids;
					//balls.subtractImages(input.image);
					//cv::inRange(*(input.image), cv::Scalar(10, 10, 10), cv::Scalar(255, 255, 255), *(input.image));
					Pipe<DATA_IN,DATA_OUT>::out.push(input.image);
				}
			}
		}
	private:
		Balls balls;
		bool ballsInit;
};





#endif
