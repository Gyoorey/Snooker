#ifndef READER_HPP
#define READER_HPP

#include <thread>
#include "Queue.hpp"
#include "Pipe.hpp"
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

template<class DATA>
class Reader{
	public:
		Reader( Queue<DATA>& output , DATA* b , std::string path_in , int maxLength_in) : out(output),buffer(b),cnt(0),path(path_in),maxLength(maxLength_in){
			}
			
		void operator()(){
				work();
		}
		
		void work(){
			if( path == "camera" ){
				readCamera();
			}else if( path.substr( path.size()-3 , 3 ) == "mp4" ){
				readVideo();
			}else if( path.substr( path.size()-3 , 3 ) == "png" ||  path.substr( path.size()-3 , 3 ) == "jpg" ){
				readPicture();
			}else{
				std::cerr << "Supported formats: [camera] [jpg,png] [mp4]" << std::endl;
				exit(-3);
			}
		}
	
	private:
		Queue<DATA>& out;
		DATA* buffer;
		const int maxLength;
		int cnt;
		std::string path;
		
		void readCamera(){
			cv::VideoCapture cap(0);
			std::cout << "Camera opened." << std::endl;
			while(1){
				cap >> *(buffer[cnt]);
				out.push(buffer[cnt]);
				cnt++;
				if(cnt == maxLength){
					cnt = 0;
				}
			}
		}

		void readVideo(){
			cv::VideoCapture cap(path);
			if(cap.isOpened()){
				std::cout << "Video opened: " << path << std::endl;
			}else{
				std::cerr << "Could not open video: " << path << std::endl;
				exit(-1);
			}
			unsigned long long int videoLength = cap.get(CV_CAP_PROP_FRAME_COUNT);
			unsigned long long int frameCnt = 0;
			int wait = round(1000/cap.get(CV_CAP_PROP_FPS));
			/////backward playing
			/*frameCnt = videoLength;
			while( frameCnt-- > 0){
				cap.set(CV_CAP_PROP_POS_FRAMES,frameCnt);
				cap >> *(buffer[cnt]);
				out.push(buffer[cnt]);
				cnt++;
				if(cnt == maxLength){
					cnt = 0;
				}
				std::this_thread::sleep_for (std::chrono::milliseconds(wait+100));
			}*/
			/////
			while( frameCnt++ < videoLength){
				cap >> *(buffer[cnt]);
				std::cout << "Image read: " << frameCnt << std::endl;
				out.push(buffer[cnt]);
				cnt++;
				if(cnt == maxLength){
					cnt = 0;
				}
				std::this_thread::sleep_for (std::chrono::milliseconds(wait));
			}
			out.push(NULL);
			std::cout << "Video end.\n";
		}

		void readPicture(){
			*(buffer[cnt]) = cv::imread(path, CV_LOAD_IMAGE_COLOR);
			if(buffer[cnt]->data){
				std::cout << "Picture opened: " << path << std::endl;
				out.push(buffer[cnt]);
			}else{
				std::cerr << "Could not open picture: " << path << std::endl;
				exit(-2);
			}
		}

};

#endif //READER_HPP
