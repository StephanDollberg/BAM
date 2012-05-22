// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// work_pool, underlaying "scheduler" for task_pool

#ifndef BAM_WORK_POOL_HPP
#define BAM_WORK_POOL_HPP

#include <mutex>
#include <queue>
#include <future>

#include "function_wrapper.hpp"


namespace bam { namespace detail {

class work_pool {
public:
  template<typename callable>
  void push_back(callable&& task) {
    std::lock_guard<std::mutex> lock(m);
    queue.push(std::move(task));
  }

  function_wrapper get_work() {
    std::lock_guard<std::mutex> lock(m, std::adopt_lock); // get_chunck() always requires a call to work_available() first
    auto ret = std::move(queue.front());
    queue.pop();
    return ret;
  }

  bool work_available(std::vector<work_pool>& steal_pool) {
    if(!work_done() || work_stealable(steal_pool)) {
      return true;
    }
    else {
      return false;
    }
  }

private:
  mutable std::mutex m;
  std::queue<bam::detail::function_wrapper> queue;

  bool work_done() const {
    std::unique_lock<std::mutex> lock(m);
    if(!queue.empty()) {
      lock.release();
      return false;
    }
    else {
      return true;
    }
  }

  // steal work from other work_pools in steal_pool
  bool work_stealable(std::vector<work_pool>& steal_pool) {
    for(auto& it : steal_pool) {
      if(&it != this) {
        std::unique_lock<std::mutex> lock1(m, std::defer_lock);
        std::unique_lock<std::mutex> lock2(it.m, std::defer_lock);
        std::lock(lock1, lock2);

        auto remaining_work = it.queue.size() / 2;

        if(remaining_work > 0) {
          while(remaining_work-- > 0) {
            queue.push(std::move(it.queue.front()));
            it.queue.pop();
          }

          lock1.release(); // keep this->m locked for work_pool::get_chunk
          return true;
        }
      }
    }
    return false;
  }
};

}}

#endif // WORK_POOL_HPP
