#include "../include/bam/timer.hpp"
#include "catch.hpp"

#include <thread>

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
