// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_INVOKE_HPP
#define BAM_PARALLEL_INVOKE_HPP

#include "task_pool.hpp"
#include "timer.hpp"

#include <vector>
#include <thread>

namespace bam {

/**
 * @brief invokes n functions in parallel; simple wrapper around some std::asyncs
 * @tparam Fs variadic template param
 * @param fs variadic function param, each representing a function
 */
template<typename ... Fs>
void parallel_invoke(Fs ...fs) {

  std::vector<std::function<void()>> v_foos { fs ... };

  std::vector<std::future<void>> temp_futs;

  for(auto&& i : v_foos) {
    temp_futs.push_back(std::async(std::launch::async, i));
  }
}

}


#endif // BAM_PARALLEL_INVOKE_HPP
