// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_REDUCE_HPP
#define BAM_PARALLEL_REDUCE_HPP

#include <vector>
#include <memory>
#include <future>

#include "detail/work_range.hpp"
#include "detail/parallel_utility.hpp"
#include "utility.hpp"

namespace bam {

template<typename return_type, typename ra_iter, typename worker_predicate, typename join_predicate>
return_type parallel_reduce(ra_iter begin, ra_iter end, worker_predicate worker, join_predicate joiner, int grainsize = 0) {
  // get all the parameters like threadcount, grainsize and work per thread
  auto threadcount = bam::detail::get_threadcount(end - begin);
  if(threadcount == 0)
    return worker(begin, end);
  if(grainsize == 0) {
    grainsize = bam::detail::get_grainsize(end - begin, threadcount);
  }
  auto work_piece_per_thread = (end - begin) / threadcount;

  // vectors to store work, results and the threads
  std::vector<std::unique_ptr<bam::detail::work_range<ra_iter>>> work(threadcount); // using unique_ptr to solve uncopyable stuff
  std::vector<return_type> results(threadcount);
  std::vector<std::thread> threads(threadcount);

  // initialize work
  auto counter = begin;
  for(auto it = std::begin(work); it != std::end(work) - 1; ++it, counter += work_piece_per_thread) {
    *it = bam::make_unique<bam::detail::work_range<ra_iter>>(counter, counter + work_piece_per_thread, grainsize);
  }
  work.back() = bam::make_unique<bam::detail::work_range<ra_iter>>(counter, end, grainsize);

  // helper function
  auto work_helper = [&] (int thread_id) {

    if(work[thread_id]->work_available(work)) { // first run initializes results vector
      auto first_work_chunk = work[thread_id]->get_chunk();
      results[thread_id] = worker(first_work_chunk.first, first_work_chunk.second);
    }

    while(work[thread_id]->work_available(work)) {
      auto work_chunk = work[thread_id]->get_chunk();
      auto result = worker(work_chunk.first, work_chunk.second);
      results[thread_id] = joiner(results[thread_id], result);
    }
  };

  // spawn threads
  auto thread_id_counter = 0;
  for(auto& i : threads) {
    i = std::thread(work_helper, thread_id_counter++);
  }

  // join threads
  for(auto& i : threads) {
    i.join();
  }

  // join results
  auto result = *std::begin(results);
  for(auto it = std::begin(results) + 1; it != std::end(results); ++it) {
    result = joiner(result, *it);
  }
  return result;
}

}

#endif // bam_PARALLEL_REDUCE_HPP
