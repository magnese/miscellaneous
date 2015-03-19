#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

class ThreadCollectiveCommunication
{
  public:
  ThreadCollectiveCommunication(const std::size_t& numThreads):
    size_(numThreads),mtx_(),count_(0),seccount_(0),condvar_(),bufferptr_(nullptr)
  {}

  inline const std::size_t& size()
  {
    return size_;
  }

  inline void barrier()
  {
    std::unique_lock<std::mutex> lock(mtx_);
    const std::size_t oldSecCount(seccount_);
    ++count_;
    if(count_!=size_)
      condvar_.wait(lock,[this,oldSecCount](){return seccount_!=oldSecCount;});
    else
    {
      count_=0;
      ++seccount_;
      condvar_.notify_all();
    }
  }

  template<class T>
  inline void alloc(const std::size_t& tid)
  {
    if(tid==0)
      bufferptr_=new std::vector<T>(size_);
  }

  template<class T>
  inline void free(const std::size_t& tid)
  {
    barrier();
    if(tid==0)
    {
      delete static_cast<std::vector<T>*>(bufferptr_);
      bufferptr_=nullptr;
    }
    barrier();
  }

  template<class T>
  inline void set(const T& value,const std::size_t& tid)
  {
    barrier();
    (*(static_cast<std::vector<T>*>(bufferptr_)))[tid]=value;
    barrier();
  }

  template<class T>
  inline std::vector<T>& get() const
  {
    return *(static_cast<std::vector<T>*>(bufferptr_));
  }

  private:
  const std::size_t size_;
  std::mutex mtx_;
  std::size_t count_;
  std::size_t seccount_;
  std::condition_variable condvar_;
  void* bufferptr_;
};

void exec(ThreadCollectiveCommunication& comm, std::size_t tid)
{
  comm.template alloc<std::size_t>(tid);
  comm.template set<std::size_t>(tid,tid);
  comm.template get<std::size_t>();
  comm.template free<std::size_t>(tid);
}

int main()
{
  const std::size_t numThreads(10);
  ThreadCollectiveCommunication comm(numThreads);

  std::vector<std::thread> t(numThreads);
  for(std::size_t tid=0;tid!=numThreads;++tid)
    t[tid]=std::thread(exec,std::ref(comm),tid);
  for(std::size_t tid=0;tid!=numThreads;++tid)
    t[tid].join();

  return 0;
}
