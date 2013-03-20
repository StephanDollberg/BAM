// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_PARALLEL_REDUCE_HPP
#define BAM_PARALLEL_REDUCE_HPP

#include "detail/parallel_utility.hpp"
#include <iterator>

#include <boost/range.hpp>

namespace bam {

    //! parallel_reduce algorithm, which enables parallelism on reduce opeartions
    /**
     * \param begin begin iterator of the range to be worked on
     * \param end iterator of the range to be worked on
     * \param worker function object predicate which the threads will run to opearte on the given range
     * \param joiner function object predicate which will be used to fullfil the reduce operations and combine the results from different threads and determine the result
     * \param grainsize defines the grainsize, default argument of 0 means that grainsize will be determined on runtime
     */
    template<typename ra_iter, typename worker_predicate, typename join_predicate>
    auto parallel_reduce(ra_iter begin, ra_iter end, worker_predicate worker, join_predicate joiner, int grainsize = 0) ->
      typename std::result_of<
          join_predicate(typename std::result_of<worker_predicate(ra_iter, ra_iter)>::type, typename std::result_of<worker_predicate(ra_iter, ra_iter)>::type)
        >::type 
    {
        typedef typename std::result_of<worker_predicate(ra_iter, ra_iter)>::type worker_return_type;
        typedef typename std::result_of<join_predicate(worker_return_type, worker_return_type)>::type return_type;
  
        // get params work_piece_per_thread and grainsize
        auto work_piece_per_thread = 0;
        std::tie(grainsize, work_piece_per_thread) = detail::get_scheduler_params(end - begin, grainsize);
  
        if(work_piece_per_thread == 0) {
            return worker(begin, end);
        }
  
        // create work
        auto work = detail::make_work(begin, end, work_piece_per_thread, grainsize);
  
        // helper function
        auto work_helper = [&] (typename decltype(work)::iterator thread_iter) -> return_type {
            return_type ret = return_type();
            std::pair<ra_iter, ra_iter> work_chunk;
    
            if(thread_iter->try_fetch_work(work_chunk, work)) { // first run initializes ret
              ret = worker(work_chunk.first, work_chunk.second);
            }
    
            while(thread_iter->try_fetch_work(work_chunk, work)) {
              auto result = worker(work_chunk.first, work_chunk.second);
              ret = joiner(ret, result);
            }
    
            return ret;
        };
  
        // start runner tasks
        auto threads = detail::spawn_tasks(std::begin(work), std::end(work), work_helper);
  
        // join results
        auto result = std::begin(threads)->get();
        for(auto it = std::begin(threads) + 1; it != std::end(threads); ++it) {
            result = joiner(result, it->get());
        }
  
        return result;
    }

    template<typename range, typename worker_predicate, typename joiner_predicate>
    auto parallel_reduce(range& rng, worker_predicate worker, joiner_predicate joiner, int grainsize = 0) -> 
        typename std::result_of<joiner_predicate(
                    typename std::result_of<worker_predicate(typename range::iterator, typename range::iterator)>::type, 
                    typename std::result_of<worker_predicate(typename range::iterator, typename range::iterator)>::type
                )>::type
    {
        return parallel_reduce(boost::begin(rng), boost::end(rng), std::move(worker), std::move(joiner), grainsize);
    }
}
#endif // bam_PARALLEL_REDUCE_HPP
