// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// work_range, underlaying "scheduler" for all parallel_ constructs

#ifndef BAM_WORK_RANGE_H
#define BAM_WORK_RANGE_H

#include <utility>
#include <mutex>

namespace bam { namespace detail {

template<typename ra_iter>
class work_range {
public:
  work_range(ra_iter begin_, ra_iter end_, unsigned grainsize_) : iter(begin_), end(end_), grainsize(grainsize_)  {}

  void get_chunk(std::pair<ra_iter, ra_iter>& ret) { // take chunk by reference for excep safety
    std::lock_guard<std::mutex> lock(m, std::adopt_lock); // get_chunck() always requires a call to work_available() first
    if(iter < end - grainsize) {
      ret.first = iter;
      ret.second = iter + grainsize;
      iter += grainsize;
    }
    else {      
      ret.first = iter;
      ret.second = end;
      iter = end;
    }
  }

  bool work_available(const std::vector<std::unique_ptr<work_range<ra_iter>>>& steal_pool) {
    if(!work_done() || work_stealable(steal_pool)) {
      return true;
    }
    else {
      return false;
    }
  }

private:
  mutable std::mutex m;
  ra_iter iter;
  ra_iter end;
  const int grainsize;


  bool work_done() const {
    std::unique_lock<std::mutex> lock(m);
    if(iter == end) {
      return true;
    }
    else {
      lock.release(); // keep lock locked for the following call to get_chunk
      return false;
    }
  }

  // steal work from work_ranges in steal_pool
  bool work_stealable(const std::vector<std::unique_ptr<work_range<ra_iter>>>& steal_pool) {
    for(auto it = std::begin(steal_pool); it != std::end(steal_pool); ++it) {
      if(this != it->get()) {
        std::unique_lock<std::mutex> lk1(m, std::defer_lock);
        std::unique_lock<std::mutex> lk2((*it)->m, std::defer_lock);
        std::lock(lk1, lk2);

        auto remaining_work = (*it)->end - (*it)->iter;
        if(remaining_work > grainsize) {
          iter = (*it)->iter + remaining_work / 2;
          end = (*it)->end;

          (*it)->end = (*it)->iter + remaining_work / 2;
          lk1.release(); // keep this->m locked for work_range::get_chunk
          return true;
        }
      }
    }

    return false;
  }
};

} }

#endif // BAM_WORK_RANGE_H
