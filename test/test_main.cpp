// compile this with -std=c++0x -pthread 
// alternatively run run_test.sh

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include "../include/bam/parallel_for_each.hpp"
#include "../include/bam/parallel_for.hpp"
#include "../include/bam/parallel_reduce.hpp"
#include "../include/bam/parallel_invoke.hpp"
#include "../include/bam/async.hpp"
#include "../include/bam/task_pool.hpp"
#include "../include/bam/timer.hpp"

#include <vector>
#include <algorithm>
#include <exception>
#include <atomic>

// TODO grainsize tests on parallel_
// TODO exception tests on task_pool

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
  CHECK_THROWS_AS(bam::parallel_for(std::begin(v), std::end(v), worker), std::runtime_error);
}

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

TEST_CASE("parallel_reduce/4", "parallel_reduce on 1 element range") {
  std::vector<int> v(1, 1);
  typedef std::vector<int>::iterator iter;
  auto joiner = [] (iter a, iter b) { return *a > *b ? a : b; };
  auto ret = bam::parallel_reduce<iter>(std::begin(v), std::end(v), std::max_element<iter>, joiner);
  CHECK(ret == std::begin(v));
}

TEST_CASE("parallel_reduce/5", "parallel_reduce on negative elements range") {
  auto worker = [] (int begin, int end) { int ret = 0; for(int i = begin; i != end; ++i) ret += i; return ret; };
  auto joiner = [] (int a, int b) { return a + b; };
  int begin = -100;
  int end = 0;
  int result = bam::parallel_reduce<int>(begin, end, worker, joiner) * -1;
  CHECK(result == begin *  (begin - 1) / 2);
}

TEST_CASE("parallel_reduce/6", "test exception in worker function") {
  std::vector<int> v {1, 2, 3, 4, 5, 6};
  typedef std::vector<int>::iterator iter;
  auto worker = [] (iter, iter) ->int  { throw std::runtime_error("testing exception from worker"); return 0; };
  auto joiner = [] (int a, int b) -> int { return std::max(a, b); };
  CHECK_THROWS_AS(bam::parallel_reduce<int>(std::begin(v), std::end(v), worker, joiner), std::runtime_error);
}

TEST_CASE("parallel_invoke/1", "testing 3 functions") {
  std::vector<int> v { 1, 2, 3 };
  auto foo1 = [&] () { v[0] *= 2; };
  auto foo2 = [&] () { v[1] *= 2; };
  auto foo3 = [&] () { v[2] *= 2; };

  bam::parallel_invoke(foo1, foo2, foo3);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 12);
}

TEST_CASE("async/1", "testing basic async functionality") {
  auto fut = bam::async([] { return 42; });
  CHECK(fut.get() == 42);
}

TEST_CASE("async/2", "testing std::launch::async behaviour") {
  std::atomic<int> var(0);
  auto fut = bam::async([&] { while(!var) { std::this_thread::yield(); }});
  var = 1;
  CHECK(var.load() == 1);
}

TEST_CASE("async/3", "testing std::launch::deferred behaviour") {
  std::atomic<int> var(0);
  auto fut = bam::async(std::launch::deferred, [&] { var = 1; });
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  CHECK(var.load() == 0);
}

TEST_CASE("async/4", "testing unblocking behaviour on std::launch::async") {
  std::atomic<int> var(0);
  bam::async([&] { while(!var) { std::this_thread::yield(); }});
  var = 1;
  CHECK(var.load() == 1);
}

TEST_CASE("task_pool/1", "task_pool add one task") {
  std::vector<int> v(1, 1);
  bam::task_pool pool;
  pool.add([&] () { v[0] *= 2; });
  pool.wait();
  CHECK(v[0] == 2);
}

TEST_CASE("task_pool/2", "task_pool add large amount of tasks") {
  std::vector<int> v(10000, 1);
  bam::task_pool pool;
  for(auto it = std::begin(v); it != std::end(v); ++it) {
    pool.add([] (int& x) { x *= 2; }, std::ref(*it));
  }
  pool.wait();
  CHECK(std::accumulate(std::begin(v), std::end(v), 0.0) == 2 * v.size());
}

TEST_CASE("task_pool/3", "adding tasks, then waiting for and then re-add") {
  std::atomic<int> i (0);
  auto worker = [&] () { ++i; };
  bam::task_pool pool;
  pool.add(worker);
  pool.add(worker);
  pool.wait();
  pool.add(worker);
  pool.add(worker);
  pool.wait_and_finish();
  CHECK(i.load() == 4);
}

TEST_CASE("task_pool/4", "return value of function from task_pool") {
  bam::task_pool pool;
  auto ret1 = pool.add([] () { return 1; });
  auto ret2 = pool.add([] () { return 2; });
  CHECK(ret1.get() == 1);
  CHECK(ret2.get() == 2);
}

TEST_CASE("task_pool/5", "adding methods which throw") {
  bam::task_pool pool;
  auto ret1 = pool.add([] () { throw std::runtime_error("task_pool excep 1"); });
  auto ret2 = pool.add([] () { throw std::runtime_error("task_pool excep 2"); });
  CHECK_THROWS_AS(ret1.get(), std::runtime_error);
  CHECK_THROWS_AS(ret2.get(), std::runtime_error);
}

/* notice that on some gcc versions(e.g. 4.6.3 on fedora16, ubuntu 12.04 is fine) std::this_thread::sleep_for for is bugged/wrong configured and won't compile.
 If it doesn't work, you need to use the following flag on compiling -D_GLIBCXX_USE_NANOSLEEP or just configure your gcc to make it standard */
TEST_CASE("timer/1", "elapsed test") {
  bam::basic_timer t;
  auto dura = std::chrono::milliseconds(20);
  std::this_thread::sleep_for(dura);
  CHECK(t.elapsed() >= dura.count());
}

TEST_CASE("timer/2", "last_epoch without elapsed") {
  bam::basic_timer t;
  auto dura = std::chrono::milliseconds(20);
  std::this_thread::sleep_for(dura);
  CHECK(t.since_last_epoch() >= dura.count());
}

TEST_CASE("timer/3", "last_epoch with elapsed") {
  bam::timer<std::chrono::milliseconds> t;
  auto dura = std::chrono::milliseconds(20);
  std::this_thread::sleep_for(dura);
  CHECK(t.elapsed() >= dura.count());
  std::this_thread::sleep_for(dura);
  CHECK(t.since_last_epoch() >= (dura).count());
}
