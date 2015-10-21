#include "Reader.hpp"

template<typename DATA>
void Reader<DATA>::readCamera(){
	cv::VideoCapture cap(0);
	std::cout << "Camera opened" << std::endl;
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	start = std::chrono::system_clock::now();
	while(1){
		end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << 1.0/elapsed.count() << std::endl;
		start = std::chrono::system_clock::now();
		cap >> *(buffer[cnt]);
		out.push(buffer[cnt]);
		cnt++;
		if(cnt == maxLength){
			cnt = 0;
		}
	}
}

template<class DATA>
void Reader<DATA>::readVideo(){

}

template<class DATA>
void Reader<DATA>::readPicture(){

}
