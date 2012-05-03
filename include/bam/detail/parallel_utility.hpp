// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_UTILITY_HPP
#define BAM_PARALLEL_UTILITY_HPP

namespace bam { namespace detail {

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

int get_threadcount() {
  int physicalthreads = std::thread::hardware_concurrency();
  int threadcount =  physicalthreads ? physicalthreads * 2 : 16; // small oversubscription

  return threadcount;
}

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
