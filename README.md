#What is BAM?

BAM is a small library which offers parallel constructs like parallel_for, parallel_reduce or a task-pool.
It is thought to simplify multithreading and act as a lightweight alternative to TBB/(PPL).

#Example

    #include <bam/parallel_for_each.hpp>
    #include <boost/range/algorithm.hpp>
    #include <vector>
    #include <iterator>

    int main() {
        auto compute_the_answer = [] (int& x) { x = 42; };
        std::vector<int> v{1, 2, 3, 4, 5, 6};
        bam::parallel_for_each(v, compute_the_answer);
        boost::copy(v, std::ostream_iterator<int>(std::cout, " ")); // 42 42 42 42 42 42
    }

#What does it offer?

 - parallel_for
 - parallel_for_each
 - parallel_reduce
 - parallel_transform
 - improved version of std::async
 - task_pool
 - timer
 - parallel_invoke
 - parallel_find
 - parallel_copy

#What do I need?

BAM is a header only library, all you need is the include directory and a recent compiler, like GCC 4.7 or clang 3.3.

The only third party dependency is boost. Lowest known working version is 1.50. Older versions should work without a problem though.
This dependency was first added in version 0.2.0, which means that if you don't have boost you can use all versions till 0.1.0.

You can use the library by either dropping the include folder directly into your project tree or by including it from your compile options.
On compiling you have to use both the -std=c++0x and -pthread option.

#Tests

Tests are done using the great Catch Testing framework (https://github.com/philsquared/Catch). Again when running the tests, don't forget to compile with the -pthread option

#Documentation

The documentation serves as a tutorial which explains all the algorithms and possibilities. It is written with Latex - again to learn the language. You can compile the .tex file to get the newest version. Simply install a full latex distribution and then run `pdflatex doc.tex` in the root folder.

#Thanks

Thanks go to all the people on stackoverflow.com who help me to learn C++ every day and to Anthony Williams for his wonderful book C++ Concurrency in Action.
