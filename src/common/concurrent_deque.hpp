#ifndef _CONCURRENT_DEQUE_H_
#define _CONCURRENT_DEQUE_H_
/**
   Based on a post by Anthony Williams.  See
   http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
*/

#include <deque>
#include <iterator>
#include <algorithm>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <Logging.hpp>

template <typename Data> class concurrent_deque {
private:
  std::deque<Data> deq;
  mutable boost::mutex mutex;
  boost::condition_variable cv;

public:

  void lock() {
    mutex.lock();
  }

  void unlock() {
    mutex.unlock();
  }

  void push_front(Data data) {
    boost::mutex::scoped_lock lock(mutex);
    LOG(debug);
    deq.push_front(data);
    lock.unlock();
    cv.notify_one();
  }

  void push_back(Data data) {
    boost::mutex::scoped_lock lock(mutex);
    LOG(debug);
    deq.push_back(data);
    lock.unlock();
    cv.notify_one();
  }

  bool empty() const {
    boost::mutex::scoped_lock lock(mutex);
    LOG(debug);
    return deq.empty();
  }

  bool try_pop_back(Data &popped_value) {
    boost::mutex::scoped_lock lock(mutex);
    LOG(debug);

    if (deq.empty()) {
      return false;
    }
    popped_value = deq.back();
    deq.pop_back();
    return true;
  }

  bool try_pop_front(Data &popped_value) {
    boost::mutex::scoped_lock lock(mutex);
    LOG(debug);

    if (deq.empty()) {
      return false;
    }
    popped_value = deq.front();
    deq.pop_front();
    return true;
  }

  Data wait_pop_back() {
    boost::mutex::scoped_lock lock(mutex);
    while (deq.empty()) {
      cv.wait(lock);
    }
    LOG(debug);

    Data popped_value = deq.back();
    deq.pop_back();
    return popped_value;
  }

  Data wait_pop_front() {
    boost::mutex::scoped_lock lock(mutex);
    while (deq.empty()) {
      cv.wait(lock);
    }
    LOG(debug);
    Data popped_value = deq.front();
    deq.pop_front();
    return popped_value;
  }

  void wait_not_empty() {
    boost::mutex::scoped_lock lock(mutex);
    while (deq.empty()) {
      cv.wait(lock);
    }
    LOG(debug);

  }

  Data front() {
    return deq.front();
  }

  Data back() {
    return deq.back();
  }

  int size() {
    return deq.size();
  }

  typename std::deque<Data>::iterator begin() {
    return deq.begin();
  }

  typename std::deque<Data>::iterator end() {
    return deq.end();
  }

  void erase(typename std::deque<Data>::iterator it) {
    boost::mutex::scoped_lock lock(mutex);
    LOG(debug);
    deq.erase(it);
  }

  Data at(size_t n) {
    return deq.at(n);
  }
};
#endif /* _CONCURRENT_DEQUE_H_ */
