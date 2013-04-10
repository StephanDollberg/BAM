#include "catch.hpp"
#include <boost/range/algorithm/equal.hpp>
#include "../include/bam/parallel_copy.hpp"

TEST_CASE("parallel_copy/1", "iterators; only functional tests, rest is based on parallel_for") {
    std::vector<int> v{1, 2, 3};
    std::vector<int> v2(v.size());
    bam::parallel_copy(v.begin(), v.end(), v2.begin());
    CHECK(boost::equal(v, v2));
}

TEST_CASE("parallel_copy/2", "range; only functional tests, rest is based on parallel_for") {
    std::vector<int> v{1, 2, 3};
    std::vector<int> v2(v.size());
    bam::parallel_copy(v, v2.begin());
    CHECK(boost::equal(v, v2));
}
