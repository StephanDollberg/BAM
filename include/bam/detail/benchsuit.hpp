// (C) Copyright Stephan Dollberg 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BAM_BENCHSUIT_HPP
#define BAM_BENCHSUIT_HPP

#include<vector>
#include<functional>
#include<utility>

#include "../timer.hpp"

namespace bam { namespace detail {

template<typename resolution>
class benchsuit {
public:

  template<typename function, typename... Args>
  void add(std::string desc, function&& foo, Args&&... args) {
    auto new_task = std::bind(std::forward<function>(foo), std::forward<Args>(args)...);
    functions.push_back(std::make_pair(desc, std::move(new_task)));
  }

  void run() {
    for(auto& i : functions) {
      bam::timer<resolution> t;
      i.second();
      std::cout << i.first << " took " << t.elapsed() << " time" << std::endl;
    }

    functions.clear();
  }

private:
  std::vector<std::pair<std::string, std::function<void()> > > functions;
};

}}

#endif // BAM_BENCHSUIT_HPP
