// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// work_pool, underlaying "scheduler" for task_pool

#ifndef BAM_WORK_POOL_HPP
#define BAM_WORK_POOL_HPP

#include "function_wrapper.hpp"

#include <mutex>
#include <queue>
#include <future>
#include <vector>
#include <utility>

namespace bam { namespace detail {

class work_pool {
public:

  /**
   * \brief add new task to pool
   * \param task callable function object which will be added to queue
   */
  template<typename callable>
  void push_back(callable&& task) {
    std::lock_guard<std::mutex> lock(m);
    queue.push(std::move(task));
  }

  /**
   * \brief try to add new task to pool
   * \param task callable function object which will be added to queue
   * \return false if queue is not empty, no task will be added in this case
   */
  template<typename callable>
  bool try_push_back(callable&& task) {
    std::lock_guard<std::mutex> lock(m);

    if(queue.empty()) {
      queue.push(std::move(task));
      return true;
    }
    else {
      return false;
    }
  }

  /**
   * @brief try_fetch_work
   * @param ret function_wrapper which will be filled with new work if available
   * @param steal_pool std::vector of other work_pools from work can be stolen
   * @return true if work was fetched
   */
  bool try_fetch_work(function_wrapper& ret, std::vector<work_pool>& steal_pool) {
    std::unique_lock<std::mutex> lock(m);
    if(try_get_work(ret)) {
      return true;
    }
    else if(work_stealable(steal_pool, lock)) {
      return try_get_work(ret);
    }
    else {
      return false;
    }
  }



private:
  mutable std::mutex m;
  std::queue<bam::detail::function_wrapper> queue;

  /**
   * @brief try_get_work try to get new work
   * @param ret function_wrapper to be filled with next work
   * @return true if work was aquired
   */
  bool try_get_work(function_wrapper& ret) { // take function_wrapper by ref for excep safety
    if(!queue.empty()) {
      ret = std::move(queue.front());
      queue.pop();
      return true;
    }
    else {
      return false;
    }
  }

  /**
   * @brief work_stealable checks whether work can be stolen and steals if available
   * @param steal_pool std::vector of other work_pools from which work can be stolen
   * @param lock1 unqiue_lock which locks this->m
   * @return true if work was stolen
   */
  bool work_stealable(std::vector<work_pool>& steal_pool, std::unique_lock<std::mutex>& lock1) {
    for(auto& it : steal_pool) {
      if(&it != this) {
        lock1.unlock();
        std::unique_lock<std::mutex> lock2(it.m, std::defer_lock);
        std::lock(lock1, lock2);

        const int queue_size = it.queue.size();
        auto remaining_work = 0;

        // we need to steal at least one function. This is needed for the semaphore signalling meachanism 
        if(queue_size == 1) {
          remaining_work = 1;
        }
        else {
          remaining_work = queue_size / 2;
        }

        if(remaining_work > 0) {
          while(remaining_work-- > 0) {
            queue.push(std::move(it.queue.front()));
            it.queue.pop();
          }

          return true;
        }
      }
    }
    return false;
  }
};

}}

#endif // WORK_POOL_HPP
