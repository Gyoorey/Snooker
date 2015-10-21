#include <iostream>
#include <thread>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Queue.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "Table.hpp"
#include "BallSegmenter.hpp"
#include "InnerTypes.hpp"

int main( int argc , char** argv ){
	if(argc!=2){
		std::cerr << "usage: ./main [video name] || [picture name] || [camera]\n";
		exit(-4);
	}
	const int bufferLength = 30;
	cv::Mat** buffer = new cv::Mat*[bufferLength];
	for(int i=0;i<bufferLength;i++){
		buffer[i] = new cv::Mat(720,680,CV_8UC3,0.0);
	}
	
	Queue<cv::Mat*> q1;
	Queue<SegmentedTableType> q2;
	Queue<cv::Mat*> q3;
	
	
	Reader<cv::Mat*> reader(q1,buffer,argv[1],bufferLength);
	Table<cv::Mat*,SegmentedTableType> tableSegmenter(q1,q2);
	BallSegmenter<SegmentedTableType,cv::Mat*> ballSegmenter(q2,q3);
	Writer<cv::Mat*> writer(q3);
	
	std::thread tr(reader);
	std::thread tableSegmenterThread(tableSegmenter);
	std::thread ballSegmenterThread(ballSegmenter);
	std::thread tw(writer);

	tr.join();
	tableSegmenterThread.join();
	ballSegmenterThread.join();
	tw.join();
	
	return 0;
}
