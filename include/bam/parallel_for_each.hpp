// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_FOR_EACH_HPP
#define BAM_PARALLEL_FOR_EACH_HPP

#include "detail/parallel_utility.hpp"
#include <iterator>
#include <algorithm>

namespace bam {

//! parallel_for algorithm, replacing serial std::for_each loops
/**
 * \param begin begin iterator of the range to be worked on
 * \param end end iterator of the range to be worked on
 * \param worker function object predicate which the threads will run to operate on the given range
 * \param grainsize defines the grainsize, default argument of 0 means that grainsize will be determined on runtime
 */
template<typename ra_iter, typename worker_predicate>
void parallel_for_each(ra_iter begin, ra_iter end, worker_predicate worker, int grainsize = 0) {

  // get params work_piece_per_thread and grainsize
  auto work_piece_per_thread = 0;
  std::tie(grainsize, work_piece_per_thread) = detail::get_scheduler_params(end - begin, grainsize);

  if(work_piece_per_thread == 0) {
    return;
  }

  // build work
  auto work = detail::make_work(begin, end, work_piece_per_thread, grainsize);

  // helper function which the threads will run
  auto work_helper = [&] (typename decltype(work)::iterator thread_iter) {
    std::pair<ra_iter, ra_iter> work_chunk;
    while(thread_iter->try_fetch_work(work_chunk, work)) {
      std::for_each(work_chunk.first, work_chunk.second, worker);
    }
  };

  // spawn tasks
  auto tasks = detail::spawn_tasks(std::begin(work), std::end(work), work_helper);

  // get tasks & rethrow exceptions
  detail::get_tasks(std::begin(tasks), std::end(tasks));
}

}


#endif // BAM_PARALLEL_FOR_EACH_HPP
