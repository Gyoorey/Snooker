#ifndef WRITER_HPP
#define WRITER_HPP

#include <thread>
#include "Queue.hpp"
#include "Pipe.hpp"
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


template<class DATA>
class Writer{
	public:
		Writer( Queue<DATA>& input ) : in(input){
			}
			
		void operator()(){
			work();
		}
		
		void work(){
			int counter = 1;
			while(1){
				DATA out = in.pop();
				if(out != NULL){
					std::cout << "\t" << out << ": " << counter++ << std::endl;
				}else{
					break;
				}
			}
		}
	
	private:
		Queue<DATA>& in;
};

std::ostream& operator<<(std::ostream& stream , cv::Mat* mat){
	cv::imshow("writer",*mat);
	cv::waitKey(1);
	stream << "Image write";
	return stream;
}

#endif //WRITER_HPP
