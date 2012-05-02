// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_FOR_H
#define BAM_PARALLEL_FOR_H

#include<vector>
#include<thread>
#include<memory>
#include<iterator>

#include "detail/work_range.hpp"
#include "detail/parallel_utility.hpp"
#include "utility.hpp"

namespace bam {

template<typename ra_iter, typename worker_predicate>
void parallel_for(ra_iter begin, ra_iter end, worker_predicate worker, int grainsize = 0) {
  // sets all parameters like threadcount, grainsize and work per thread
  auto threadcount = bam::detail::get_threadcount(end - begin);
  if(threadcount == 0)
    return;
  if(grainsize == 0) {
    grainsize = bam::detail::get_grainsize(end - begin, threadcount);
  }
  auto work_piece_per_thread = (end - begin) / threadcount;

  // vectors to store the threads and work per thread
  std::vector<std::unique_ptr<bam::detail::work_range<ra_iter>>> work(threadcount); // using unique_ptr to solve uncopyable stuff
  std::vector<std::thread> threads(threadcount);

  // build the work for each thread
  auto counter = begin;
  for(auto it = std::begin(work); it != std::end(work) - 1; ++it, counter += work_piece_per_thread) {
    *it = bam::make_unique<bam::detail::work_range<ra_iter>>(counter, counter + work_piece_per_thread, grainsize);
  }
  work.back() = bam::make_unique<bam::detail::work_range<ra_iter>>(counter, end, grainsize);

  // helper function which the threads will run
  auto work_helper = [&] (int thread_id) {
    while(work[thread_id]->work_available(work)) {
      auto work_chunk = work[thread_id]->get_chunk();
      worker(work_chunk.first, work_chunk.second);
    }
  };

  // spawn threads
  auto thread_id_counter = 0;
  for(auto& i : threads) {
    i = std::thread(work_helper, thread_id_counter++);
  }

  // join all threads
  for(auto& i : threads) {
    i.join();
  }
}
}

#endif // bam_PARALLEL_FOR_H
