#include <iostream>
#include <thread>
#include "Queue.hpp"

class IntPlusFloat{
	public:
		int* i;
		float f;
		
		IntPlusFloat( int* i_in , float f_in ) : i(i_in),f(f_in){
			}
};

class IntPlusFloatPlusDouble{
	public:
		IntPlusFloat ipf;
		double d;
		
		
		IntPlusFloatPlusDouble( IntPlusFloat i_in , double d_in ) : ipf(i_in),d(d_in){
			}
};

std::ostream& operator<<(std::ostream& stream , IntPlusFloatPlusDouble& in){
	std::cout << "[" << *(in.ipf.i) << " , " << in.ipf.f << " , " << in.d << " ]";
	return stream;
}


template<class DATA_IN ,class DATA_OUT=DATA_IN >
class Pipe{

	public:
		
		Pipe( Queue<DATA_IN>& input , Queue<DATA_OUT>& output ) : in(input),out(output){
			}

		void operator()(){
			work();
		}
		
		virtual void work()=0;
		
	protected:
		Queue<DATA_IN>& in;
		Queue<DATA_OUT>& out;
};

class PipeInt : public Pipe<int*>{
	public:
		PipeInt( Queue<int*>& input , Queue<int*>& output ) : Pipe<int*>(input,output){
			}

	void work(){
		int* a = in.pop();
		*a *= 5;
		out.push(a);
		//std::cout << *a << std::endl;
	}

};

template<class DATA_IN ,class DATA_OUT=DATA_IN >
class PipeIntPLusFloat : public Pipe<DATA_IN,DATA_OUT>{
	public:
	
		PipeIntPLusFloat( Queue<DATA_IN>& input , Queue<DATA_OUT>& output ) : Pipe<DATA_IN,DATA_OUT>(input,output){
			}

		void work(){
			DATA_IN a = Pipe<DATA_IN,DATA_OUT>::in.pop();
			*a *= 2;
			IntPlusFloat f(a,1.0);
			Pipe<DATA_IN,DATA_OUT>::out.push(f);
			std::cout << "egesz: " << *a << "  float: " << f.f << std::endl;
		}

};

template<class DATA_IN ,class DATA_OUT=DATA_IN >
class PipeIntPLusFloatPlusDouble : public Pipe<DATA_IN,DATA_OUT>{
	public:
	
		PipeIntPLusFloatPlusDouble( Queue<DATA_IN>& input , Queue<DATA_OUT>& output ) : Pipe<DATA_IN,DATA_OUT>(input,output){
			}

		void work(){
			DATA_IN a = Pipe<DATA_IN,DATA_OUT>::in.pop();
			*(a.i) *= 2;
			a.f *= 3;
			IntPlusFloatPlusDouble f( IntPlusFloat(a.i,a.f) , 5.0 );
			Pipe<DATA_IN,DATA_OUT>::out.push(f);
			std::cout << "PipeIntPLusFloatPlusDouble" << std::endl;
		}

};

template<class DATA>
class Reader{
	public:
		Reader( Queue<DATA>& output , DATA* b) : out(output),buffer(b),cnt(0){
			}
			
		void operator()(){
			while(1)
				work();
		}
		
		void work(){
			scanf("%d",buffer[cnt]);
			out.push(buffer[cnt]);
			cnt++;
			if(cnt == maxLength){
				cnt = 0;
			}
		}
	
	private:
		Queue<DATA>& out;
		DATA* buffer;
		const int maxLength = 10;
		int cnt;
};

template<class DATA>
class Writer{
	public:
		Writer( Queue<DATA>& input ) : in(input){
			}
			
		void operator()(){
			while(1)
				work();
		}
		
		void work(){
			DATA out = in.pop();
			std::cout << "End: " << out << std::endl;
		}
	
	private:
		Queue<DATA>& in;
};

int main(){
	int** buffer = new int*[10];
	for(int i=0;i<10;i++){
		buffer[i] = new int;
	}
	Queue<int*> q1,q2;
	Queue<IntPlusFloat> q3;
	Queue<IntPlusFloatPlusDouble> q4;
	PipeInt p1(q1,q2);
	PipeIntPLusFloat<int*,IntPlusFloat> p2(q2,q3);
	PipeIntPLusFloatPlusDouble<IntPlusFloat,IntPlusFloatPlusDouble> p3(q3,q4);
	Reader<int*> r(q1,buffer);
	Writer<IntPlusFloatPlusDouble> w(q4);

	std::thread tr(r);
	std::thread tp1(p1);
	std::thread tp2(p2);
	std::thread tp3(p3);
	std::thread tw(w);

	tp1.join();
	tp2.join();
	tp3.join();
	tr.join();
	tw.join();
	delete[] buffer;
	
	
	return 0;
}
