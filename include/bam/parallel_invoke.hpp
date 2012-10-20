// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_INVOKE_HPP
#define BAM_PARALLEL_INVOKE_HPP

#include "task_pool.hpp"

#include <vector>

namespace bam {

/**
 * @brief invokes n functions in parallel; simple wrapper around a bam::task_pool
 * @tparam Fs variadic template param
 * @param fs variadic function param, each representing a function
 */
template<typename ... Fs>
void parallel_invoke(Fs ...fs) {
  task_pool pool;
  std::vector<std::function<void()>> v_foos { fs ... };

  for(auto&& i : v_foos) {
    pool.add(i);
  }
}

}


#endif // BAM_PARALLEL_INVOKE_HPP
