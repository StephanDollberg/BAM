// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_UTILITY_HPP
#define BAM_PARALLEL_UTILITY_HPP

#include "work_range.hpp"
#include <thread>
#include <vector>
#include <list>
#include <future>
#include <iostream>

namespace bam { namespace detail {

    /**
     * \brief return grainsize taking number threads and range size into account
     * \param range_size size of range to worked on by parallel_
     * \param threadcount number threads to be spawned
     */
    template<typename distance>
    int get_grainsize(distance range_size, int threadcount) {
        auto work_per_thread = threadcount ? range_size / threadcount : 0;
  
        if(work_per_thread == 0) {
            return 0;
        }
  
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

    /**
     * @brief returns grainsize, work_piece_per_thread
     */
    template<typename distance>
    std::tuple<int, int> get_scheduler_params(distance dist, int grainsize) {
        // get all the parameters like threadcount, grainsize and work per thread
        auto threadcount = detail::get_threadcount(dist);

        if(grainsize == 0) {
            grainsize = detail::get_grainsize(dist, threadcount);
        }
        auto work_piece_per_thread = threadcount ? dist / threadcount : 0;

        return std::make_tuple(grainsize, work_piece_per_thread);
    }

    /**
     * @brief builds work with given range and work per thread
     */
    template<typename range_iter>
    std::list<work_range<range_iter>> make_work(range_iter begin, range_iter end, int initial_work_per_thread, int grainsize) {
        std::list<work_range<range_iter>> work;
        
        auto counter = begin;
        for(; counter < end - initial_work_per_thread; counter += initial_work_per_thread) {
            work.emplace_back(counter, counter + initial_work_per_thread, grainsize);
        }
        work.emplace_back(counter, end, grainsize);

        return work;
    }

    template<typename work_iter, typename worker_foo>
    auto spawn_tasks(work_iter begin, work_iter end, worker_foo&& foo) 
      -> std::vector<std::future<typename std::result_of<worker_foo(work_iter)>::type>>
    {
        std::vector<std::future<typename std::result_of<worker_foo(work_iter)>::type>> threads;
        for(auto it = begin; it != end; ++it) {
            threads.emplace_back(std::async(std::launch::async, foo, it));
        }

        return threads;
    }

    template<typename iter>
    void get_tasks(iter begin, iter end) {
        for(auto it = begin; it != end; ++it) {
            it->get();
        }
    }

} }

#endif // UTILITY_HPP
