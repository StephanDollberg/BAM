#What is BAM?

BAM is a small library which offers parallel constructs like parallel_for, parallel_reduce or a task-pool.
It is thought to simplify multithreading and act as a lightweight alternative to TBB/(PPL). 

In addition it serves me to store code I frequently use and to learn all the multithreading features C++11 offers.

#What do I need?

BAM is a header only library, all you need is the include directory and a recent compiler, like GCC 4.6.

You can use the library by either dropping the include folder directly into your project tree or by including it from your compile options.
On compiling you have to use both the -std=c++0x and -pthread option.

#Tests

Tests are done using the great Catch Testing framework (https://github.com/philsquared/Catch). Again when running the tests, don't forget to compile with the -pthread option

#Documentation

The Documentation serves as a tutorial which explains all the algorithms and possiblities. It is written with Latex - again to learn the language. The pdf version might be out of date from time to time as I don't commit the latest version on every push. You can compile the .tex file to get the newest version.

#Thanks

Thanks go to all the people on stackoverflow.com who help me to learn C++ every day and to Anthony Williams for his wonderfull book C++ Concurrency in Action.
