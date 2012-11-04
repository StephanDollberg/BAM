// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_ASYNC_IMPL 
#define BAM_ASYNC_IMPL

#include "async_task_pool.hpp"

#include <future>
#include <type_traits>
#include <utility>

namespace bam { namespace detail {

/**
 * @brief get static thread pool in template
 */
inline bam::detail::async_task_pool& get_pool() {
	static bam::detail::async_task_pool pool;
	return pool;
}

/**
 * @brief implementation of async
 * @param policy std::launch policy, behaviour equal to std::async
 * @param foo function to run with
 * @param args arguments
 */
template<typename Foo, typename ...Args>
std::future<typename std::result_of<Foo(Args...)>::type> async_impl(std::launch policy, Foo&& foo, Args&& ...args) {
	bam::detail::async_task_pool& pool = get_pool();
	
	if(policy == (std::launch::async | std::launch::deferred)) {
		
		// we need to do this to avoid recursive deadlock
		auto tuple = pool.try_add(std::forward<Foo>(foo), std::forward<Args>(args)...);
		if(std::get<0>(tuple)) {
			return std::move(std::get<1>(tuple));
		}
		else {
			return std::async(std::launch::deferred, std::forward<Foo>(foo), std::forward<Args>(args)...);
		}
	}
	else if (policy == std::launch::deferred) {
		return std::async(std::launch::deferred, std::forward<Foo>(foo), std::forward<Args>(args)...);
	}
	else {
		return pool.add(std::forward<Foo>(foo), std::forward<Args>(args)...);
	}
}


}}

#endif // BAM_ASYNC_IMPL