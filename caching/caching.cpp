#include <thread>
#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <string>

void rf(int &n)
{
	for(int i=0;i<1000000000;++i)
		++n;
}

template<typename T>
void test(T& x0,T& x1,std::string&& name)
{
  // print info
  std::cout<<"Test: "<<name<<std::endl;
  std::cout<<"&x0 =  "<<(long long int)(&x0)<<std::endl;
  std::cout<<"&x1 = "<<(long long int)(&x1)<<std::endl;
  std::cout<<"(&x1 - &x2) = "<<&x1-&x0<<" bytes"<<std::endl;
	// single execution
  x0=0;
	auto start=std::chrono::system_clock::now();
	std::thread t(rf,std::ref(x0));
	t.join();
	auto end=std::chrono::system_clock::now();
	std::cout<<"Single thread time: "<< std::chrono::duration<double>(end-start).count()<<std::endl;
	// double execution
  x0=0;
  x1=0;
	start=std::chrono::system_clock::now();
	std::thread t1(rf,std::ref(x0));
	std::thread t2(rf,std::ref(x1));
	t1.join();
	t2.join();
	end=std::chrono::system_clock::now();
	std::cout << "Double thread time: " <<std::chrono::duration<double>(end-start).count()<<std::endl<<std::endl;
}

int main()
{
  constexpr int offset(16);

  std::array<int,2> x1;
  test(x1[0],x1[1],"Adjacent elements stack");

  std::array<int,offset+1> x2;
  test(x2[0],x2[offset],"Padded elements stack");

  std::vector<int> x3(2);
  test(x3[0],x3[1],"Adjacent elements heap");

  std::vector<int> x4(offset+1);
  test(x4[0],x4[offset],"Padded elements heap");

  return 0;
}
