#include "../include/bam/parallel_reduce.hpp"
#include "catch.hpp"
#include <numeric>

TEST_CASE("parallel_reduce/1", "parallel_reduce on small range") {
  std::vector<int> v {1, 2, 3, 4, 5, 6};
  typedef std::vector<int>::const_iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto worker = [] (iter a, iter b) { return std::max_element(a,b); };
  auto ret = bam::parallel_reduce(std::begin(v), std::end(v),worker, joiner);
  CHECK(*ret == v.back());
}
  
TEST_CASE("parallel_reduce/2", "parallel_reduce on large range") {
  std::vector<int> v(25000, 1);
  v.back() = 2;
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto worker = [] (iter a, iter b) { return std::max_element(a,b); };
  auto ret = bam::parallel_reduce(std::begin(v), std::end(v),worker, joiner);
  CHECK(*ret == v.back());
}

TEST_CASE("parallel_reduce/3", "parallel_reduce on zero range") {
  std::vector<int> v;
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto worker = [] (iter a, iter b) { return std::max_element(a,b); };
  auto ret = bam::parallel_reduce(std::begin(v), std::end(v),worker, joiner);
  CHECK(ret == std::end(v));
}

TEST_CASE("parallel_reduce/4", "parallel_reduce on 1 element range") {
  std::vector<int> v(1, 1);
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto worker = [] (iter a, iter b) { return std::max_element(a,b); };
  auto ret = bam::parallel_reduce(std::begin(v), std::end(v),worker, joiner);
  CHECK(ret == std::begin(v));
}

TEST_CASE("parallel_reduce/5", "parallel_reduce on negative elements range") {
  auto worker = [] (int begin, int end) -> int { int ret = 0; for(int i = begin; i != end; ++i) ret += i; return ret; };
  auto joiner = [] (int a, int b) { return a + b; };
  int begin = -100;
  int end = 0;
  int result = bam::parallel_reduce(begin, end, worker, joiner) * -1;
  CHECK(result == begin *  (begin - 1) / 2);
}

TEST_CASE("parallel_reduce/6", "test exception in worker function") {
  std::vector<int> v {1, 2, 3, 4, 5, 6};
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter, iter) ->int  { throw std::runtime_error("testing exception from worker"); return 0; };
  auto joiner = [] (int a, int b) -> int { return std::max(a, b); };
  CHECK_THROWS_AS(bam::parallel_reduce(std::begin(v), std::end(v), worker, joiner), std::runtime_error);
}

TEST_CASE("parallel_reduce/7", "testing range overloads") {
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    auto worker = [] (std::vector<int>::iterator b, std::vector<int>::iterator e) { return std::accumulate(b, e, 0); };
    auto joiner = std::plus<int>();
    CHECK(bam::parallel_reduce(v, worker, joiner) == std::accumulate(v.begin(), v.end(), 0));
    CHECK(bam::parallel_reduce(v, worker, joiner, 1) == std::accumulate(v.begin(), v.end(), 0));
    CHECK(bam::parallel_reduce(v.begin(), v.end(), worker, joiner) == std::accumulate(v.begin(), v.end(), 0));
    CHECK(bam::parallel_reduce(v.begin(), v.end(), worker, joiner, 1) == std::accumulate(v.begin(), v.end(), 0));
}

