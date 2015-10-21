#ifndef INNER_TYPES_HPP
#define INNER_TYPES_HPP

#include "Table.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>


class SegmentedTableType{
	public:
		cv::Mat* image;
		TableParams* tp;
		
		SegmentedTableType(cv::Mat* image_in, TableParams* tp_in):image(image_in),tp(tp_in){
			}
			
		bool operator!=(long int* a){
			if( this->image != NULL)
				return true;
			else
				return false;
		}

};

std::ostream& operator<<(std::ostream& stream , SegmentedTableType& tp){
	stream << tp.image;
	return stream;
}






#endif //INNER_TYPES_HPP
