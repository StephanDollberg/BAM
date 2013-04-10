#include "../include/bam/parallel_invoke.hpp"
#include "catch.hpp"

TEST_CASE("parallel_invoke/1", "testing 3 functions") {
  std::vector<int> v { 1, 2, 3 };
  auto foo1 = [&] () { v[0] *= 2; };
  auto foo2 = [&] () { v[1] *= 2; };
  auto foo3 = [&] () { v[2] *= 2; };

  bam::parallel_invoke(foo1, foo2, foo3);
  CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 12);
}