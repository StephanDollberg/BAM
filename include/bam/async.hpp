// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_ASYNC
#define BAM_ASYNC

#include "detail/async_impl.hpp"

#include <future>
#include <type_traits>
#include <utility>

namespace bam {

/**
 * @brief bam::async with std::launch::async | std::launch::deferred implicitly given 
 */
template<typename Foo, typename ...Args, typename std::enable_if<!std::is_enum<Foo>::value, int>::type = 0    > 
std::future<typename std::result_of<Foo(Args...)>::type> async(Foo&& foo, Args&& ...args) {
	return detail::async_impl(std::launch::async | std::launch::deferred, std::forward<Foo>(foo), std::forward<Args>(args)... );
}

/**
 * @brief replacement for std::async; uses internal scheduler in any case
 * @param policy std::launch policiy
 * @param foo function to run
 * @param args arguments to pass to foo
 * @return non-blocking std::future
 */
template<typename Foo, typename ...Args>
std::future<typename std::result_of<Foo(Args...)>::type> async(std::launch policy, Foo&& foo, Args&& ...args) {
	return detail::async_impl(policy, std::forward<Foo>(foo), std::forward<Args>(args)... );
}

}

#endif // BAM_ASYNC