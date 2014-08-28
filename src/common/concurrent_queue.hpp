#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_
/**
   Based on a post by Anthony Williams.  See
   http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
*/

#include <queue>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

template <typename Data> class concurrent_queue {
private:
  std::queue<Data> queue;
  mutable boost::mutex mutex;
  boost::condition_variable cv;

public:
  void push(Data const &data) {
    boost::mutex::scoped_lock lock(mutex);
    queue.push(data);
    lock.unlock();
    cv.notify_one();
  }

  bool empty() const {
    boost::mutex::scoped_lock lock(mutex);
    return queue.empty();
  }

  bool try_pop(Data &popped_value) {
    boost::mutex::scoped_lock lock(mutex);
    if (queue.empty()) {
      return false;
    }

    popped_value = queue.front();
    queue.pop();
    return true;
  }

  void wait_and_pop(Data &popped_value) {
    boost::mutex::scoped_lock lock(mutex);
    while (queue.empty()) {
      cv.wait(lock);
    }
    popped_value = queue.front();
    queue.pop();
  }

  int size() {
    boost::mutex::scoped_lock lock(mutex);
    return queue.size();
  }

};
#endif /* _CONCURRENT_QUEUE_H_ */
