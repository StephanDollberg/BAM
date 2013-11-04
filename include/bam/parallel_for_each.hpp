// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_FOR_EACH_HPP
#define BAM_PARALLEL_FOR_EACH_HPP

#include "detail/parallel_utility.hpp"
#include "parallel_for.hpp"

#include <boost/range.hpp>
#include <iterator>
#include <algorithm>

#ifdef BAM_USE_TBB
#include <tbb/parallel_for_each.h>
#endif

namespace bam {

    /**
     * \brief parallel_for algorithm, replacing serial std::for_each loops
     * \param begin begin iterator of the range to be worked on
     * \param end end iterator of the range to be worked on
     * \param worker function object predicate which the threads will run to operate on the given range
     * \param grainsize defines the grainsize, default argument of 0 means that grainsize will be determined on runtime
     */
    template<typename ra_iter, typename worker_predicate>
    void parallel_for_each(ra_iter begin, ra_iter end, worker_predicate worker, int grainsize = 0) {
        auto worker_helper = [=] (ra_iter b, ra_iter e) {
            std::for_each(b, e, worker);
        };

        parallel_for(begin, end, worker_helper, grainsize);
    }

    /**
     * @brief range wrapper for bam::parallel_for_each
     */
    template<typename range, typename worker_predicate>
    void parallel_for_each(range&& rng, worker_predicate worker, int grainsize = 0) {
        parallel_for_each(boost::begin(rng), boost::end(rng), std::move(worker), grainsize);
    }
}


#endif // BAM_PARALLEL_FOR_EACH_HPP
