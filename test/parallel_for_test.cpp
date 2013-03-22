#include "../include/bam/parallel_for.hpp"
#include "catch.hpp"

#include <boost/range/irange.hpp>

#include <numeric>
#include <vector>

TEST_CASE("parallel_for/1", "parallel_for on small range ") {
  std::vector<int> v(6, 1);
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_for/2", "parallel_for on large range ") {
  std::vector<int> v(25000, 1);
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_for/3", "parallel_for on 0 range ") {
  std::vector<int> v;
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_for/4", "parallel_for on a 1 element range") {
  std::vector<int> v(1, 1);
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

// lame
TEST_CASE("parallel_for/5", "parallel_for on negative elements range") {
  std::vector<int> v(25000, 1);
  auto worker = [&] (int begin, int end) { for(auto i = begin; i < end; ++i) {
    v[-1 * i] *= 2;
    }
  };
  int start = -1 * v.size() + 1;
  int end = 0;
  bam::parallel_for(start, end, worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()) - 1);
}

TEST_CASE("parallel_for/6", "parallel_for excep from worker thread") {
  int size = 10;
  typedef std::vector<int>::iterator iter;
  std::vector<int> v(size, 1);
  auto worker = [&] (iter begin, iter end) { for(auto it = begin; it != end; ++it) {
    if(*it == 1)
      throw std::runtime_error("testing exception");
  }
  };
  CHECK_THROWS_AS(bam::parallel_for(std::begin(v), std::end(v), worker), std::exception&);
}

TEST_CASE("parallel_for/7", "testing parallel_for range overloads") {
    std::vector<int> v{0, 0, 0, 0, 0, 0};
    auto worker = [&] (int b, int e) {
        for(int i = b; i != e; ++i) {
            v[i] += 1;
        }
    };
    bam::parallel_for(boost::irange(0,6), worker);
    CHECK(std::accumulate(v.begin(), v.end(), 0) == 1 * static_cast<int>(v.size()));
    bam::parallel_for(boost::irange(0,6), worker, 1);
    CHECK(std::accumulate(v.begin(), v.end(), 0) == 2 * static_cast<int>(v.size()));
    bam::parallel_for(0, 6, worker);
    CHECK(std::accumulate(v.begin(), v.end(), 0) == 3 * static_cast<int>(v.size()));
    bam::parallel_for(0, 6, worker, 1);
    CHECK(std::accumulate(v.begin(), v.end(), 0) == 4 * static_cast<int>(v.size()));
}

