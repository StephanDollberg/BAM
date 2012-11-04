// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASYNC_BAM_TASK_POOL_HPP
#define ASYNC_BAM_TASK_POOL_HPP

#include "../task_pool.hpp"


namespace bam { namespace detail {

class async_task_pool : public bam::task_pool {
public:
  template<typename function, typename ...Args>
  std::tuple<bool, std::future<typename std::result_of<function(Args...)>::type>> try_add(function&& f, Args... args) {
    assert(done == false);
    typedef typename std::result_of<function(Args...)>::type return_type;

    auto bound_task = std::bind(std::forward<function>(f), std::forward<Args>(args)...);
    std::packaged_task<return_type()> task(std::move(bound_task));
    auto ret = task.get_future();


    if(work[0].try_push_back(std::move(task))) {
      sem.post();  
      return std::make_tuple(true, std::move(ret));
    } 
    else {
      return std::make_tuple(false, std::future<return_type>());
    }
  }
};

}}

#endif // ASYNC_BAM_TASK_POOL_HPP
