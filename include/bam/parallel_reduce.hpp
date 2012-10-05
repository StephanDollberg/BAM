// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_REDUCE_HPP
#define BAM_PARALLEL_REDUCE_HPP

#include "detail/work_range.hpp"
#include "detail/parallel_utility.hpp"
#include "utility.hpp"

#include <vector>
#include <list>
#include <future>
#include <iterator>

namespace bam {

//! parallel_reduce algorithm, which enables parallelism on reduce opeartions
/**
 * \param begin begin iterator of the range to be worked on
 * \param end iterator of the range to be worked on
 * \param worker function object predicate which the threads will run to opearte on the given range
 * \param joiner function object predicate which will be used to fullfil the reduce operations and combine the results from different threads and determine the result
 * \param grainsize defines the grainsize, default argument of 0 means that grainsize will be determined on runtime
 */
template<typename return_type, typename ra_iter, typename worker_predicate, typename join_predicate>
return_type parallel_reduce(ra_iter begin, ra_iter end, worker_predicate worker, join_predicate joiner, int grainsize = 0) {
  // get all the parameters like threadcount, grainsize and work per thread
  auto threadcount = detail::get_threadcount(end - begin);

  if(threadcount == 0)
    return worker(begin, end);

  if(grainsize == 0) {
    grainsize = detail::get_grainsize(end - begin, threadcount);
  }
  auto work_piece_per_thread = (end - begin) / threadcount;

  // vectors to store work, results and the threads
  std::list<detail::work_range<ra_iter>> work;
  std::vector<std::future<return_type>> threads;

  // build the work for each thread
  auto counter = begin;
  for(; counter < end - work_piece_per_thread; counter += work_piece_per_thread) {
    work.emplace_back(counter, counter + work_piece_per_thread, grainsize);
  }
  work.emplace_back(counter, end, grainsize);

  // helper function
  auto work_helper = [&] (typename std::list<detail::work_range<ra_iter>>::iterator thread_iter) -> return_type {
    return_type ret = return_type();
    std::pair<ra_iter, ra_iter> work_chunk;

    if(thread_iter->try_fetch_work(work_chunk, work)) { // first run initializes ret
      ret = worker(work_chunk.first, work_chunk.second);
    }

    while(thread_iter->try_fetch_work(work_chunk, work)) {
      auto result = worker(work_chunk.first, work_chunk.second);
      ret = joiner(ret, result);
    }

    return ret;
  };

  // spawn threads
  for(auto it = std::begin(work); it != std::end(work); ++it) {
    threads.emplace_back(std::async(std::launch::async, work_helper, it));
  }

  // join results
  auto result = std::begin(threads)->get();
  for(auto it = std::begin(threads) + 1; it != std::end(threads); ++it) {
    result = joiner(result, it->get());
  }
  return result;
}

}

#endif // bam_PARALLEL_REDUCE_HPP
