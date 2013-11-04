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
    namespace detail {
        // looks for a non-end iterator and then returns it
        template<typename Container, typename Iter>
        Iter join_iter(Container& c, Iter default_iter) {
            for(auto&& task : c) {
                auto iter = task.get();
                if(iter != default_iter) {
                    return iter;
                }
            }
            
            return default_iter;
        }
    }

    /**
     * @brief searches val in range [begin, end)
     * @return returns an iterator to one occurange of val, if the value was not found returns end
     */
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
        auto work_helper = [&] (detail::work_range<Iter>& work_rng) {
            std::pair<Iter, Iter> work_chunk;
            while(!done && work_rng.try_fetch_work(work_chunk, work)) {
                auto found_iter = boost::find(work_chunk, val);
                if(found_iter != work_chunk.second) {
                    done = true;
                    return found_iter;
                }
            }

            return end;
        };

        // spawn tasks
        auto tasks = detail::spawn_tasks(work, work_helper);

        // get tasks & rethrow
        return detail::join_iter(tasks, end);
    }

    /**
     * @brief searches for val in rng 
     * @return returns an iterator to one occurance of the searched value, if value was not found returns an end iterator of rng
     */
    template<typename Range, typename T>
    auto parallel_find(Range&& rng, const T& val, int grainsize = 0) -> decltype(boost::begin(rng)) {
        return parallel_find(boost::begin(rng), boost::end(rng), val, grainsize);
    }
}

#endif // BAM_PARALLEL_FIND_HPP
