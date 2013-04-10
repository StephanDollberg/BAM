#include "../include/bam/async.hpp"
#include "catch.hpp"

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
