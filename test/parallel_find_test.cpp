#include "catch.hpp"
#include "../include/bam/parallel_find.hpp"

TEST_CASE("parallel_find/1", "testing positive") {
    std::vector<int> v {1, 2, 3, 4, 5, 6};
    CHECK(bam::parallel_find(v, 3) == v.begin() + 2);
}

TEST_CASE("parallel_find/2", "testing negative") {
    std::vector<int> v {1, 2, 3, 4, 5, 6};
    CHECK(bam::parallel_find(v, 8) == v.end());
}
