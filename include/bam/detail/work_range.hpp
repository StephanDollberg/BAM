// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// work_range, underlaying "scheduler" for all parallel_ constructs

#ifndef BAM_WORK_RANGE_H
#define BAM_WORK_RANGE_H

#include <utility>
#include <mutex>
#include <list>
#include <memory>

namespace bam { namespace detail {

    template<typename ra_iter>
    class work_range {
    public:
        work_range(ra_iter begin_, ra_iter end_, unsigned grainsize_) : iter(begin_), end(end_), grainsize(grainsize_)  {}

        /**
         * @brief try_fetch_work tries to fetch work
         * @param chunk work chunk to fill with work
         * @param steal_pool list of other work_ranges from which can be stolen if all work is done
         * @return true if work was aquired, false otherwise
         */
        bool try_fetch_work(std::pair<ra_iter, ra_iter>& chunk, std::list<work_range<ra_iter>>& steal_pool) {
            std::unique_lock<std::mutex> lock(m);
            if (try_get_chunk(chunk)) {
                return true;
            }
            else if(work_stealable(steal_pool, lock)) {
                return try_get_chunk(chunk);
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

        /**
         * @brief try_get_chunk trys to get work
         * @param ret pair to fill with work
         * @return true if work was aquired
         */
        bool try_get_chunk(std::pair<ra_iter, ra_iter>& ret) { // take chunk by reference for excep safety
            if(iter < end - grainsize) {
                ret.first = iter;
                ret.second = iter + grainsize;
                iter += grainsize;
                return true;
            }
            else if (iter < end) {
                ret.first = iter;
                ret.second = end;
                iter = end;
                return true;
            }
            return false;
        }


        /**
         * @brief work_stealable checks if work can be stolen and does if available
         * @param steal_pool other work_ranges from work can be stolen
         * @param lk1 std::unique_lock which is currently locked from this->try_fetch_work call
         * @return true if work was stolen, false otherwise
         */
        bool work_stealable(std::list<work_range<ra_iter>>& steal_pool, std::unique_lock<std::mutex>& lk1) {
            for(auto&& i : steal_pool) {
                if(this != &i) {
                    lk1.unlock(); // unlock this->m to make it ready for deadlock safe double lock -> std::lock
                    std::unique_lock<std::mutex> lk2(i.m, std::defer_lock);
                    std::lock(lk1, lk2);

                    auto remaining_work = i.end - i.iter;
                    if(remaining_work > grainsize) {
                        iter = i.iter + remaining_work / 2;
                        end = i.end;

                        i.end = i.iter + remaining_work / 2;
                        return true;
                    }
                }
            }

            return false;
        }
    };
} }

#endif // BAM_WORK_RANGE_H
