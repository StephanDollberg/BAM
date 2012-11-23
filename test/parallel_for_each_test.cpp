#include "../include/bam/parallel_for_each.hpp"
#include "catch.hpp"

#include <numeric>
#include <vector>

TEST_CASE("parallel_for_each/1", "compute and accumulate") {
  std::vector<int> v(6, 1);
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}
  
TEST_CASE("parallel_for_each/2", "compute and accumulate on large range") {
  std::vector<int> v(25000, 1);
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_for_each/3", "compute and accumulate on empty range") {
  std::vector<int> v;
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 0);
}

TEST_CASE("parallel_for_each/4", "compute and accumulate on 1 element range") {
  std::vector<int> v(1, 1);
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_for_each/5", "test exception in worker function") {
  std::vector<int> v(10, 1);
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter::value_type x) { if(x == 1) throw std::runtime_error("testing exception"); };
  CHECK_THROWS_AS(bam::parallel_for_each(std::begin(v), std::end(v), worker), std::runtime_error);
}
