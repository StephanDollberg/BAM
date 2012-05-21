// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_TASK_POOL_HPP
#define BAM_TASK_POOL_HPP

#include <vector>
#include <future>
#include <atomic>
#include <functional>

#include "detail/work_pool.hpp"
#include "detail/parallel_utility.hpp"

namespace bam {

class task_pool {
public:
  task_pool() : done(false), work(bam::detail::get_threadcount()), threads(bam::detail::get_threadcount()) {
    try {
      int thread_id = 0;
      for(auto& i: threads) {
        i = std::async(std::launch::async, &task_pool::worker, this, thread_id++);
      }
    } catch (const std::system_error& err) {
      done = true;
      throw;
    }
  }

  ~task_pool() {
    done = true;
  }

  // add task that takes no args
  template<typename function>
  std::future<typename std::result_of<function()>::type> add(function&& f) {
    typedef typename std::result_of<function()>::type return_type;

    std::packaged_task<return_type()> task(std::move(f));
    auto ret = task.get_future();
    work[0].push_back(std::move(task));  // profiling needed
    return ret;
  }

  // add tasks with arguments
  template<typename function, typename ...Args>
  std::future<typename std::result_of<function(Args...)>::type> add(function&& f, Args&& ...args) {
    typedef typename std::result_of<function(Args...)>::type return_type;

    auto bound_task = std::bind(std::forward<function>(f), std::forward<Args>(args)...);
    std::packaged_task<return_type()> task(std::move(bound_task));
    auto ret = task.get_future();

    work[0].push_back(std::move(task)); // profiling needed
    return ret;
  }

  // finish tasks
  void wait() {
    done = true;

    for(auto& i : threads)
      i.get();

    // make task pool ready to work again after all work has been finished
    done = false;

    try {
      int thread_id_counter = 0;
      for(auto& i : threads)
        i = std::async(std::launch::async, &task_pool::worker, this, thread_id_counter++);
    } catch (const std::system_error& err) {
      done = true;
      throw;
    }

  }

private:
  std::atomic<bool> done;
  std::vector<bam::detail::work_pool> work;
  std::vector<std::future<void> > threads;

  // worker function, threads will run
  void worker(int thread_id) {
    while(!done) {
      if(work[thread_id].work_available(work)) {
        work[thread_id].get_work()();
      }
      else {
        std::this_thread::yield();
      }
    }

    // finish work
    while(work[thread_id].work_available(work)) {
      work[thread_id].get_work()();
    }
  }
};

}

#endif // BAM_TASK_POOL_HPP
