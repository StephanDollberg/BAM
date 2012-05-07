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

  std::pair<ra_iter, ra_iter> get_chunk() {
    std::lock_guard<std::mutex> lock(m, std::adopt_lock); // get_chunck() always requires a call to work_available() first
    if(iter < end - grainsize) {
      auto ret = std::pair<ra_iter, ra_iter>(iter, iter + grainsize);
      iter += grainsize;
      return ret;
    }
    else {      
      auto ret = std::pair<ra_iter, ra_iter>(iter, end);
      iter = end;
      return ret;
    }
  }

  bool work_available(const std::vector<std::unique_ptr<bam::detail::work_range<ra_iter>>>& steal_pool) {
    if(!work_done() || work_stealable(steal_pool)) {
      return true;
    }
    else {
      return false;
    }
  }

private:
  ra_iter iter;
  ra_iter end;
  const int grainsize;
  mutable std::mutex m;

  bool work_done() const {
    m.lock();
    if(iter == end) {
      m.unlock();
      return true;
    }
    else {
      return false;
    }
  }

  bool work_stealable(const std::vector<std::unique_ptr<bam::detail::work_range<ra_iter>>>& steal_pool) {
    for(auto it = std::begin(steal_pool); it != std::end(steal_pool); ++it) {
      if(this != it->get()) {
        std::lock(m, (*it)->m);
        std::unique_lock<std::mutex> lk1(m, std::adopt_lock);
        std::lock_guard<std::mutex> lk2((*it)->m, std::adopt_lock);

        auto remaining_work = (*it)->end - (*it)->iter;
        if(remaining_work > grainsize) {
          iter = (*it)->iter + remaining_work / 2;
          end = (*it)->end;

          (*it)->end = (*it)->iter + remaining_work / 2;
          lk1.release();
          return true;
        }
      }
    }

    return false;
  }
};

} }

#endif // BAM_WORK_RANGE_H
