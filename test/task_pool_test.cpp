#include "../include/bam/task_pool.hpp"
#include "catch.hpp"

#include <numeric>

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
