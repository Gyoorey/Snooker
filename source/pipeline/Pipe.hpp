#ifndef PIPE_HPP
#define PIPE_HPP

#include <thread>
#include "Queue.hpp"

template<class DATA_IN ,class DATA_OUT=DATA_IN >
class Pipe{

	public:
		
		Pipe( Queue<DATA_IN>& input , Queue<DATA_OUT>& output ) : in(input),out(output){
			}

		virtual void operator()(){
			work();
		}
		
		virtual void work()=0;
		
	protected:
		Queue<DATA_IN>& in;
		Queue<DATA_OUT>& out;
};

#endif //PIPE_HPP
