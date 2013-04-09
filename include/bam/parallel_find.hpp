// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_FIND_HPP
#define BAM_PARALLEL_FIND_HPP

#include "detail/work_range.hpp"
#include "detail/parallel_utility.hpp"
#include <boost/range/algorithm.hpp>
#include <tuple>
#include <atomic>

namespace bam {
    template<typename Iter, typename T>
    Iter parallel_find(Iter begin, Iter end, const T& val, int grainsize = 0) {
        // get params work_piece_per_thread and grainsize
        auto work_piece_per_thread = 0;
        std::tie(grainsize, work_piece_per_thread) = detail::get_scheduler_params(end - begin, grainsize);

        if(work_piece_per_thread == 0) {
            return end;
        }

        // build work
        std::atomic<bool> done(false);
        auto work = detail::make_work(begin, end, work_piece_per_thread, grainsize);

        // helper function which the threads will run
        auto work_helper = [&] (typename decltype(work)::iterator thread_iter) {
            std::pair<Iter, Iter> work_chunk;
            while(!done && thread_iter->try_fetch_work(work_chunk, work)) {
                auto found_iter = boost::find(work_chunk, val);
                if(found_iter != work_chunk.second) {
                    done = true;
                    return found_iter;
                }
            }

            return end;
        };

        // spawn tasks
        auto tasks = detail::spawn_tasks(std::begin(work), std::end(work), work_helper);

        // get tasks & rethrow
        for(auto&& task : tasks) {
            auto iter = task.get();
            if(iter != end) {
               return iter;
            }
        } 
        return end;
    }
}

#endif // BAM_PARALLEL_FIND_HPP
