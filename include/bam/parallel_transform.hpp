// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_TRANSFORM_HPP
#define BAM_PARALLEL_TRANSFORM_HPP

#include "parallel_for_each.hpp"

#include <boost/iterator/zip_iterator.hpp>


#include <numeric>

namespace bam {
    template<typename in_iter, typename out_iter, typename Worker>
    void parallel_transform(in_iter input_first, in_iter input_end, out_iter out_first, Worker worker, int grainsize = 0) {
        using in_type = typename std::iterator_traits<in_iter>::value_type;
        using out_type = typename std::iterator_traits<out_iter>::value_type;

        auto helper = [&] (boost::tuple<in_type, out_type&> t) {
            boost::get<1>(t) = worker(boost::get<0>(t));
        };

        auto begin = boost::make_zip_iterator(boost::make_tuple(input_first, out_first));
        auto end = boost::make_zip_iterator(boost::make_tuple(input_end,
            out_first + std::distance(input_first, input_end)));

        bam::parallel_for_each(begin, end, helper, grainsize);

    }
}

#endif
