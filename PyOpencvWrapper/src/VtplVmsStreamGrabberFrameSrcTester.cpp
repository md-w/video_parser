#include "zero_copy_queue.h"
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>

template <typename T> class threadsafe_queue
{
private:
  mutable std::mutex mut;
  std::queue<T> data_queue;
  std::condition_variable data_cond;
  int max_size;

public:
  threadsafe_queue(int max_size) {}
  ~threadsafe_queue() {}
  threadsafe_queue(threadsafe_queue const& other)
  {
    std::lock_guard<std::mutex> lk(other.mut);
    data_queue = other.data_queue;
    max_size = other.max_size;
  }
  bool full()
  {
    std::unique_lock<std::mutex> lk(mut);
    if (data_queue.size() >= max_size)
      return true;
    return false;
  }
  int size()
  {
    std::unique_lock<std::mutex> lk(mut);
    return data_queue.size();
  }
  void push(T&& new_value)
  {
    std::cout << "Here 1" << std::endl;
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.size() < max_size) {
      std::cout << "Here 2" << std::endl;
      data_queue.push(std::move(new_value));
    }
    data_cond.notify_one();
  }
  void wait_and_pop(T& value)
  {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    value = data_queue.front();
    data_queue.pop();
  }
  std::shared_ptr<T> wait_and_pop(int wait_sec)
  {
    std::shared_ptr<T> res;
    std::unique_lock<std::mutex> lk(mut);
    if (data_cond.wait_for(lk, std::chrono::seconds(wait_sec), [this] { return !data_queue.empty(); })) {
      res.reset(std::make_shared<T>(data_queue.front()));
      data_queue.pop();
    } else {
      res.reset(nullptr);
    }
    return res;
  }
  bool try_pop(T& value)
  {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty())
      return false;
    value = data_queue.front();
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop()
  {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty())
      return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }
  bool empty() const
  {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
  }
};
struct probe {
  int value;
  probe()
  {
    static int n;
    value = ++n;
    std::cout << "probe()" << ((void*)this) << std::endl;
  }
  probe(const probe&) = delete;
  // {
  //   std::cout << "probe(c&)" << ((void*)this) << std::endl;
  // }
  probe& operator=(const probe&) = delete;
  probe(probe&&) //= default;
  {
    std::cout << "probe(&&)" << ((void*)this) << std::endl;
  }
  probe& operator=(probe&&) = default;
  ~probe() { std::cout << "~probe()" << ((void*)this) << std::endl; }
};

int main()
{

  // threadsafe_queue<probe> my_queue(10);
  // probe p;
  // my_queue.push(std::move(p));
  // std::cout << "Hello World";

  // zero_copy_queue<probe, 2> my_queue;
  // auto idx = my_queue.getWriteIdx();
  // probe* data;
  // while ((data = my_queue.getWritable(idx)) == nullptr)
  //   ;
  // // new (data) probe();
  // data->value = 110;
  // my_queue.commitWrite(idx);

  // int64_t read_idx = my_queue.getReadIdx();
  // probe* data_read;
  // while ((data_read = my_queue.getReadable(read_idx)) == nullptr)
  //   ;
  // std::cout << (*data_read).value << std::endl;
  // my_queue.commitRead(read_idx);

  // idx = my_queue.getWriteIdx();

  // while ((data = my_queue.getWritable(idx)) == nullptr)
  //   ;
  // // new (data) probe();
  // data->value = 110;
  // my_queue.commitWrite(idx);

  // return 0;

  // zero_copy_queue<probe, 2>* my_queue = new zero_copy_queue<probe, 2>();

  // auto idx = my_queue->getWriteIdx();
  // probe* data;
  // while ((data = my_queue->getWritable(idx)) == nullptr)
  //   ;
  // // new (data) probe();
  // data->value = 110;
  // my_queue->commitWrite(idx);

  // int64_t read_idx = my_queue->getReadIdx();
  // probe* data_read;
  // while ((data_read = my_queue->getReadable(read_idx)) == nullptr)
  //   ;
  // std::cout << (*data_read).value << std::endl;
  // my_queue->commitRead(read_idx);

  // idx = my_queue->getWriteIdx();
  // while ((data = my_queue->getWritable(idx)) == nullptr)
  //   ;
  // // new (data) probe();
  // data->value = 110;
  // my_queue->commitWrite(idx);

  // idx = my_queue->getWriteIdx();
  // while ((data = my_queue->getWritable(idx)) == nullptr)
  //   ;
  // // new (data) probe();
  // data->value = 110;
  // my_queue->commitWrite(idx);

  // std::cout << "now deleting" << std::endl;
  // delete my_queue;
  // return 0;

  zero_copy_queue<probe, 2> my_queue;
  auto idx = my_queue.getWriteIdx();
  probe* data;
  while ((data = my_queue.getWritable(idx)) == nullptr)
    ;
  // new (data) probe();
  data->value = 110;
  my_queue.commitWrite(idx);

  idx = my_queue.getWriteIdx();
  while ((data = my_queue.getWritable(idx)) == nullptr)
    ;
  // new (data) probe();
  data->value = 110;
  my_queue.commitWrite(idx);

  idx = my_queue.getWriteIdx();
  while ((data = my_queue.getWritable(idx)) == nullptr)
    ;
  // new (data) probe();
  data->value = 110;
  my_queue.commitWrite(idx);

  idx = my_queue.getWriteIdx();
  while ((data = my_queue.getWritable(idx)) == nullptr)
    ;
  // new (data) probe();
  data->value = 110;
  my_queue.commitWrite(idx);

  idx = my_queue.getWriteIdx();
  while ((data = my_queue.getWritable(idx)) == nullptr)
    ;
  // new (data) probe();
  data->value = 110;
  my_queue.commitWrite(idx);

  std::cout << "Now deleting" << std::endl;
}