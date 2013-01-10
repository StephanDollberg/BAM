// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_SEMAPHORE
#define BAM_SEMAPHORE

#include <mutex>
#include <condition_variable>
#include <cassert>

namespace bam { namespace detail {

/**
 * @brief basic semaphore, used as a non-busy signalling system in task_pool
 */
class semaphore {
public:

  /**
   * @brief creates semaphore with a default counter of 0
   */
  semaphore() : counter(0) {}

  /**
   * @brief creates semaphore with a given start counter
   * @param start_counter initial counter value
   */
  semaphore(int start_counter) : counter(start_counter) {
    assert(start_counter >= 0);
  }

  /**
   * @brief raises counter and notifies one waiting thread
   */
  void post() {
    std::unique_lock<std::mutex> lock(mutex);
    ++counter;
    lock.unlock();
    cond_var.notify_one();
  }

  /**
   * @brief decreases counter, blocks if counter is zero
   */
  void wait() {
    std::unique_lock<std::mutex> lock(mutex);

    auto predicate = [&] () { return counter; };

    cond_var.wait(lock, predicate);

    --counter;
  }

private:
  std::condition_variable cond_var;
  std::mutex mutex;
  int counter;
};


} }


#endif // BAM_SEMAPHORE
