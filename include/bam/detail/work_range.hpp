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


  bool try_fetch_work(std::pair<ra_iter, ra_iter>& chunk, const std::vector<std::unique_ptr<work_range<ra_iter> > >& steal_pool) {
    std::unique_lock<std::mutex> lock(m);
    if (get_chunk(chunk)) {
      return true;
    }
    else if(work_stealable(steal_pool, lock)) {
      return get_chunk(chunk);
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

  bool get_chunk(std::pair<ra_iter, ra_iter>& ret) { // take chunk by reference for excep safety
    if(iter < end - grainsize) {
      ret.first = iter;
      ret.second = iter + grainsize;
      iter += grainsize;
      return true;
    }
    else if (iter < end){
      ret.first = iter;
      ret.second = end;
      iter = end;
      return true;
    }
    return false;
  }

  // steal work from work_ranges in steal_pool
  bool work_stealable(const std::vector<std::unique_ptr<work_range<ra_iter> > >& steal_pool, std::unique_lock<std::mutex>& lk1) {
    for(auto it = std::begin(steal_pool); it != std::end(steal_pool); ++it) {
      if(this != it->get()) {
        //std::unique_lock<std::mutex> lk1(m, std::defer_lock);
        lk1.unlock();
        std::unique_lock<std::mutex> lk2((*it)->m, std::defer_lock);
        std::lock(lk1, lk2);

        auto remaining_work = (*it)->end - (*it)->iter;
        if(remaining_work > grainsize) {
          iter = (*it)->iter + remaining_work / 2;
          end = (*it)->end;

          (*it)->end = (*it)->iter + remaining_work / 2;
          //lk1.release(); // keep this->m locked for work_range::get_chunk
          return true;
        }
      }
    }

    return false;
  }
};

} }

#endif // BAM_WORK_RANGE_H
