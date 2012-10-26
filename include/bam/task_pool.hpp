// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_TASK_POOL_HPP
#define BAM_TASK_POOL_HPP

#include "detail/work_pool.hpp"
#include "detail/parallel_utility.hpp"
#include "detail/function_wrapper.hpp"
#include "detail/semaphore.hpp"


#include <vector>
#include <future>
#include <atomic>
#include <functional>
#include <cassert>

namespace bam {

class task_pool {
public:
  task_pool() : sem(0), done(false), work(detail::get_threadcount()), threads(detail::get_threadcount()) {
    init_impl();
  }

  ~task_pool() {
    done = true;

    for(auto i = 0u; i != threads.size(); ++i) {
      sem.post();
    }
  }

  /**
   * \brief add tasks with arguments
   * \param f argument taking the function object to be added to the task pool
   * \param args variadic argument to take the parameters for the function being added
   */
  template<typename function, typename ...Args>
  std::future<typename std::result_of<function(Args...)>::type> add(function&& f, Args&& ...args) {
    assert(done == false);
    typedef typename std::result_of<function(Args...)>::type return_type;

    auto bound_task = std::bind(std::forward<function>(f), std::forward<Args>(args)...);
    std::packaged_task<return_type()> task(std::move(bound_task));
    auto ret = task.get_future();

    work[0].push_back(std::move(task)); // profiling needed
    sem.post();
    return ret;
  }

  //! finish tasks and get ready for new ones
  void wait() {
    assert(done == false);
    done = true;

    for(auto& i : threads)
      i.get();

    // make task pool ready to work again after all work has been finished
    done = false;

    try {
      int thread_id_counter = 0;
      for(auto&& i : threads)
        i = std::async(std::launch::async, &task_pool::worker, this, thread_id_counter++);
    } catch (const std::system_error& err) {
      done = true;
      throw;
    }
  }

  //! finish tasks and don't restart threading
  void wait_and_finish() {
    assert(done == false);
    done = true;

    for(auto&& i : threads) {
      i.get();
    }
  }

private:
  detail::semaphore sem;
  std::atomic<bool> done;
  std::vector<detail::work_pool> work;
  std::vector<std::future<void> > threads;

  /**
   * @brief worker helper function the threads will run
   * @param thread_id thread_id which is used to map to the right work_pool
   */
  void worker(int thread_id) {
    detail::function_wrapper task;
    while(!done) {
      if(work[thread_id].try_fetch_work(task, work)) {
        task();
      }
      else {
        std::this_thread::yield();
        sem.wait();
      }
    }

    // finish work
    while(work[thread_id].try_fetch_work(task, work)) {
      task();
    }
  }

  /**
   * @brief waits till all threads have finished
   */
  void wait_impl() {
    done = true;

    for(auto i = 0u; i != threads.size(); ++i) {
      sem.post();
    }

    for(auto& i : threads) {
      i.get();
    }
  }

  /**
   * @brief starts threads with worker func
   */
  void init_impl() {
    try {
      int thread_id = 0;
      for(auto&& i: threads) {
        i = std::async(std::launch::async, &task_pool::worker, this, thread_id++);
      }
    } catch (const std::system_error& err) {
      done = true;
      throw;
    }
  }
};

}

#endif // BAM_TASK_POOL_HPP
