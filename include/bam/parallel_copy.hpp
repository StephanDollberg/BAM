// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_COPY_HPP
#define BAM_PARALLEL_COPY_HPP

#include <boost/range/begin.hpp>
#include "parallel_for.hpp"
#include <iterator>

namespace bam {
    template<typename InputIterator, typename OutputIterator>
    void parallel_copy(InputIterator begin, InputIterator end, OutputIterator dest) {
        auto helper = [=] (InputIterator b, InputIterator e) {
            std::copy(b, e, std::next(dest, std::distance(begin, b)));
        };

        parallel_for(begin, end, helper);
    }

    template<typename Range, typename Oiter>
    void parallel_copy(const Range& rng, Oiter target) {
        parallel_copy(boost::begin(rng), boost::end(rng), target);
    }

}

#endif // BAM_PARALLEL_COPY_HPP
