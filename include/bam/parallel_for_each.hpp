// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_FOR_EACH_HPP
#define BAM_PARALLEL_FOR_EACH_HPP

#include<future>
#include<vector>
#include<memory>
#include<iterator>

#include "detail/work_range.hpp"
#include "detail/parallel_utility.hpp"
#include "utility.hpp"

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
  //get all parameters like threadcount, grainsize and work per thread
  auto threadcount = bam::detail::get_threadcount(end - begin);
  if(threadcount == 0)
    return;
  if(grainsize == 0) {
    grainsize = bam::detail::get_grainsize(end - begin, threadcount);
  }
  auto work_piece_per_thread = (end - begin) / threadcount;

  // vectors to store all threads and work for each thread
  std::vector<std::unique_ptr<bam::detail::work_range<ra_iter>>> work(threadcount); // using unique_ptr to solve uncopyable stuff
  std::vector<std::future<void> > threads(threadcount);

  // build work for each thread
  auto counter = begin;
  for(auto it = std::begin(work); it != std::end(work) - 1; ++it, counter += work_piece_per_thread) {
    *it = bam::make_unique<bam::detail::work_range<ra_iter>>(counter, counter + work_piece_per_thread, grainsize);
  }
  work.back() = bam::make_unique<bam::detail::work_range<ra_iter>>(counter, end, grainsize);

  // helper function which the threads will run
  auto work_helper = [&] (int thread_id) {
    while(work[thread_id]->work_available(work)) {
      std::pair<ra_iter, ra_iter> work_chunk;
      work[thread_id]->get_chunk(work_chunk);
      std::for_each(work_chunk.first, work_chunk.second, worker);
    }
  };

  // spawn threads
  auto thread_id_counter = 0;
  for(auto& i : threads) {
    i = std::async(std::launch::async, work_helper, thread_id_counter++);
  }

  // rethrow exceptions
  for(auto& i : threads) {
    i.get();
  }
}

}


#endif // BAM_PARALLEL_FOR_EACH_HPP
