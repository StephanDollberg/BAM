// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_UTILITY_HPP
#define BAM_PARALLEL_UTILITY_HPP

#include <thread>

namespace bam { namespace detail {

/**
 * \brief return grainsize taking number threads and range size into account
 * \param range_size size of range to worked on by parallel_
 * \param threadcount number threads to be spawned
 */
template<typename distance>
int get_grainsize(distance range_size, int threadcount) {
  auto work_per_thread = range_size / threadcount;

  if(work_per_thread / 100 > 0)
    return work_per_thread / 100;
  else if(work_per_thread / 10 > 0)
    return work_per_thread / 10;
  else
    return 1;
}

/**
 * @brief get_threadcount checks hardware concurrency
 * @return returns number threads to be used
 */
inline int get_threadcount() {
  int physicalthreads = std::thread::hardware_concurrency();
  int threadcount =  physicalthreads ? physicalthreads * 2 : 16; // small oversubscription

  return threadcount;
}

/**
 * @brief get_threadcount checks hardware concurrency
 * \param rangesize range size to be worked on, to check whether there aren't too many threads
 */
template<typename distance>
int get_threadcount(distance rangesize) {
  int physicalthreads = std::thread::hardware_concurrency();
  int threadcount =  physicalthreads ? physicalthreads * 2 : 16; // small oversubscription

  if(threadcount > rangesize) {
    return rangesize;
  }

  return threadcount;
}

} }

#endif // UTILITY_HPP
