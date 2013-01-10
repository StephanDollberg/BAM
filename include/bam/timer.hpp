// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace bam {

//! Timer class which enables you to measure elapsed time intervals
template<class resolution>
class timer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_point;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_epoch;

public:
  typedef resolution resolution_type;

  const static bool is_steady = std::chrono::high_resolution_clock::is_steady;

  /**
   * @brief timer starts timer
   */
  timer() : start_point(std::chrono::high_resolution_clock::now()), last_epoch(start_point) {}

  /**
   * \brief returns the passed time since the creation of the timer
   */
  typename resolution::rep elapsed() {
    last_epoch = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<resolution>(last_epoch - start_point).count();
  }

  /**
   * \brief returns time since last call to elapsed or since_last_epoch, if no call happend before this call, returns the time since creation
   */
  typename resolution::rep since_last_epoch() {
    auto ret = last_epoch;
    last_epoch = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<resolution>(std::chrono::high_resolution_clock::now() - ret).count();
  }
};

//! typedef as default timer
typedef timer<std::chrono::milliseconds> basic_timer;

}


#endif // TIMER_H
