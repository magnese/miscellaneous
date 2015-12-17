#include <thread>
#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <string>

template<typename T>
void f(T& x)
{
  x=0;
  for(std::size_t i=0;i<1000000000;++i)
    ++x;
}

template<typename T>
void test(T& x0,T& x1,std::string&& name)
{
  // print info
  std::cout<<"Test: "<<name<<std::endl;
  std::cout<<"&x0 =  "<<(long long int)(&x0)<<std::endl;
  std::cout<<"&x1 = "<<(long long int)(&x1)<<std::endl;
  std::cout<<"&x1-&x2 = "<<&x1-&x0<<" bytes"<<std::endl;
  // single thread
  auto start=std::chrono::system_clock::now();
  std::thread t(f<T>,std::ref(x0));
  t.join();
  auto end=std::chrono::system_clock::now();
  std::cout<<"Single thread time: "<<std::chrono::duration<double>(end-start).count()<<" s"<<std::endl;
  // double thread
  start=std::chrono::system_clock::now();
  std::thread t1(f<T>,std::ref(x0));
  std::thread t2(f<T>,std::ref(x1));
  t1.join();
  t2.join();
  end=std::chrono::system_clock::now();
  std::cout<<"Double thread time: "<<std::chrono::duration<double>(end-start).count()<<" s"<<std::endl<<std::endl;
}

int main()
{
  constexpr int offset(16);
  typedef int T;

  std::array<T,2> x1;
  test(x1[0],x1[1],"Adjacent elements stack");

  std::array<T,offset+1> x2;
  test(x2[0],x2[offset],"Padded elements stack");

  std::vector<T> x3(2);
  test(x3[0],x3[1],"Adjacent elements heap");

  std::vector<T> x4(offset+1);
  test(x4[0],x4[offset],"Padded elements heap");

  return 0;
}
