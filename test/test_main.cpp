// compile this with -std=c++0x -pthread 

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include <bam/parallel_for_each.hpp>
#include <bam/parallel_for.hpp>
#include <bam/parallel_reduce.hpp>
#include <bam/timer.hpp>

#include <vector>
#include <algorithm>

// exception tests on parallel_
// 1 range tests on parallel_
// negative range on parallel_

TEST_CASE("parallel_for/1", "parallel_for on small range ") {
  std::vector<int> v(6, 1);
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * v.size());
}

TEST_CASE("parallel_for/2", "parallel_for on large range range ") {
  std::vector<int> v(25000, 1);
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * v.size());
}

TEST_CASE("parallel_for/3", "parallel_for on 0 range ") {
  std::vector<int> v;
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter begin, iter end) { for(auto it = begin; it != end; ++it) { *it *= 2; } };
  bam::parallel_for(std::begin(v), std::end(v), worker);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * v.size());
}

TEST_CASE("parallel_for_each/1", "compute and accumulate") {
  std::vector<int> v(6, 1);
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * v.size());
}
  
TEST_CASE("parallel_for_each/2", "compute and accumulate on large range") {
  std::vector<int> v(25000, 1);
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * v.size());
}

TEST_CASE("parallel_for_each/3", "compute and accumulate on empty range") {
  std::vector<int> v;
  bam::parallel_for_each(std::begin(v), std::end(v), [] (int& i) { i *= 2; });
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 0);
}

TEST_CASE("parallel_reduce/1", "parallel_reduce on small range") {
  std::vector<int> v {1, 2, 3, 4, 5, 6};
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto ret = bam::parallel_reduce<iter>(std::begin(v), std::end(v), std::max_element<iter>, joiner);
  CHECK(*ret == v.back());
}
  
TEST_CASE("parallel_reduce/2", "parallel_reduce on large range") {
  std::vector<int> v(25000, 1);
  v.back() = 2;
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto ret = bam::parallel_reduce<iter>(std::begin(v), std::end(v), std::max_element<iter>, joiner);
  CHECK(*ret == v.back());
}

TEST_CASE("parallel_reduce/3", "parallel_reduce on zero range") {
  std::vector<int> v;
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto ret = bam::parallel_reduce<iter>(std::begin(v), std::end(v), std::max_element<iter>, joiner);
  CHECK(ret == std::end(v));
}

/* notice that on some gcc versions(e.g. 4.6.3 on fedora16, ubuntu 12.04 is fine) std::this_thread::sleep_for for is bugged and won't compile.
 If it doesn't work, you need to use the following flag on compiling -D_GLIBCXX_USE_NANOSLEEP */
TEST_CASE("timer/1", "elapsed test") {
  bam::timer<> t;
  auto dura = std::chrono::milliseconds(20);
  std::this_thread::sleep_for(dura);
  CHECK(t.elapsed() >= dura.count());
}

TEST_CASE("timer/2", "last_epoch without elapsed") {
  bam::timer<> t;
  auto dura = std::chrono::milliseconds(20);
  std::this_thread::sleep_for(dura);
  CHECK(t.since_last_epoch() >= dura.count());
}

TEST_CASE("timer/3", "last_epoch with elapsed") {
  bam::timer<> t;
  auto dura = std::chrono::milliseconds(20);
  std::this_thread::sleep_for(dura);
  CHECK(t.elapsed() >= dura.count());
  std::this_thread::sleep_for(dura);
  CHECK(t.since_last_epoch() >= (dura).count());
}