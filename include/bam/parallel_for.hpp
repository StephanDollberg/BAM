// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_FOR_HPP
#define BAM_PARALLEL_FOR_HPP

#include "detail/parallel_utility.hpp"
#include <iterator>

#include <boost/range.hpp>

#ifdef BAM_USE_TBB
#include <tbb/parallel_for.h>
#endif

namespace bam {

    template<typename ra_iter, typename worker_predicate>
    void parallel_for_impl(ra_iter begin, ra_iter end, worker_predicate worker, int grainsize = 0) {
        // get params work_piece_per_thread and grainsize
        auto work_piece_per_thread = 0;
        std::tie(grainsize, work_piece_per_thread) = detail::get_scheduler_params(end - begin, grainsize);

        if(work_piece_per_thread == 0) {
            return;
        }

        // build work
        auto work = detail::make_work(begin, end, work_piece_per_thread, grainsize);

        // helper function which the threads will run
        auto work_helper = [&work, worker] (detail::work_range<ra_iter>& work_rng) {
            std::pair<ra_iter, ra_iter> work_chunk;
            while(work_rng.try_fetch_work(work_chunk, work)) {
                worker(work_chunk.first, work_chunk.second);
            }
        };

        // spawn tasks
        auto tasks = detail::spawn_tasks(work, work_helper);

        // get tasks & rethrow
        detail::get_tasks(tasks);
    }

    /**
     * \brief parallel_for algorithm, replacing serial for loops
     * \param begin begin iterator of the range to be worked on
     * \param end end iterator of the range to be worked on
     * \param worker function object predicate which the threads will run to operate on the given range
     * \param grainsize defines the grainsize, default argument of 0 means that grainsize will be determined on runtime
     */
    template<typename ra_iter, typename worker_predicate>
    void parallel_for(ra_iter begin, ra_iter end, worker_predicate worker, int grainsize = 0) {
        parallel_for_impl(begin, end, std::move(worker), grainsize);
    }

    /**
     * \brief range wrapper for bam::parallel_for
     */
    template<typename range, typename worker_predicate>
    void parallel_for(range&& rng, worker_predicate worker, int grainsize = 0) {
        parallel_for(boost::begin(rng), boost::end(rng), std::move(worker), grainsize);
    }
}

#endif // bam_PARALLEL_FOR_HPP
