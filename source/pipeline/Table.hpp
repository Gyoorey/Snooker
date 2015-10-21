#ifndef TABLE_HPP
#define TABLE_HPP

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>
#include "Pipe.hpp"
#include "Queue.hpp"

class TableParams{
	public:	
		static const int R_TABLE_WIDTH = 354;
		static const int TABLE_WIDTH_IN_CM = 177;
		static constexpr float PIXEL_TO_CM = TABLE_WIDTH_IN_CM / R_TABLE_WIDTH;
		
		cv::Point2f ul,ur,bl,br,ml,mr;
		cv::Point2f ulR,urR,blR,brR,mlR,mrR;
		cv::Mat H;
		cv::Mat H_inv;
		cv::Mat H_cropped;
		cv::Mat H_inv_cropped;
		cv::Mat mask;
		cv::Mat maskROI;
		//for Ball width calculations
		int height, upperWidth, lowerWidht, upLowDiff , upLowDiffPerHeigth;
		cv::Vec3b clothColor;
		cv::Point2i seedPoint;
		cv::Mat prev_image;
		cv::Mat correctionImage;
		
		TableParams();
		void process(cv::Mat* image);
		bool hasMask();
		void cropTable(cv::Mat* image);
		cv::Point2i rectify(cv::Point2i p);
		cv::Point2i backProject(cv::Point2i p);
		void getClothColor(cv::Mat* image);
		void removeCloth(cv::Mat* image);
		void proba(cv::Mat* image);
		void correctColor(cv::Mat* image,bool rectified);
	private:
		static const int GREEN_AND_OTHER_DIFF = 40;
	
		void segmentRGB(cv::Mat& rgbImg);
		std::vector<uint16_t> biggestRegion();
		void segmentField(cv::Mat& rgbImg,std::vector<uint16_t> stats);
};


template<class DATA_IN,class DATA_OUT>
class Table : public Pipe<DATA_IN,DATA_OUT>{
	public:
		
		Table(Queue<DATA_IN>& input , Queue<DATA_OUT>& output):Pipe<DATA_IN,DATA_OUT>(input,output),tableProcessed(false),tp(){
			}
		Table(){
			}
		
		void work(){
			while(1){
				DATA_IN input = Pipe<DATA_IN,DATA_OUT>::in.pop();
				if(input == NULL){
					Pipe<DATA_IN,DATA_OUT>::out.push(DATA_OUT(NULL,NULL));
					break;
				}
				if(tableProcessed){
					tp.cropTable(input);
					tp.correctColor(input,false);
					//tp.removeCloth(input);
					//cv::warpPerspective(*input,*input,tp.H_cropped,cv::Size(354,708));
					DATA_OUT stt(input,&tp);
					Pipe<DATA_IN,DATA_OUT>::out.push(stt);
				}else{
					tp.process(input);
					tableProcessed = true;
					tp.cropTable(input);
					tp.correctColor(input,false);
					//tp.removeCloth(input);
					//cv::warpPerspective(*input,*input,tp.H_cropped,cv::Size(354,708));
					DATA_OUT stt(input,&tp);
					Pipe<DATA_IN,DATA_OUT>::out.push(stt);
				}
			}
		}
	private:
		TableParams tp;
		bool tableProcessed;
};



#endif
