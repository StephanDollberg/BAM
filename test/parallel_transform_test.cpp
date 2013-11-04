#include "catch.hpp"
#include "../include/bam/parallel_transform.hpp"

TEST_CASE("parallel_transform/1", "parallel_transform on zero range") {
    std::vector<int> v(0, 1);
    auto foo = [] (int x) { return x + 1; };
    bam::parallel_transform(v.begin(), v.end(), v.begin(), foo);
    CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_transform/2", "parallel_transform on one element range") {
    std::vector<int> v(1, 1);
    auto foo = [] (int x) { return x + 1; };
    bam::parallel_transform(v.begin(), v.end(), v.begin(), foo);
    CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_transform/3", "parallel_transform on X element range") {
    std::vector<int> v(6, 1);
    auto foo = [] (int x) { return x + 1; };
    bam::parallel_transform(v.begin(), v.end(), v.begin(), foo);
    CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}

TEST_CASE("parallel_transform/4", "parallel_transform on X element range; ranged version") {
    std::vector<int> v(6, 1);
    auto foo = [] (int x) { return x + 1; };
    bam::parallel_transform(v, v.begin(), foo);
    CHECK(std::accumulate(std::begin(v), std::end(v), 0) == 2 * static_cast<int>(v.size()));
}
