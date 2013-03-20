#!/usr/bin/python

import os
import subprocess
import argparse

# creates name of executable test
def make_name():
    return 'tester'

# get all cpp files in current dir
def get_all_files():
    files = os.listdir('.')

    def  filterer(string):
        if string.rfind('.cpp') == -1:
            return False
        else:
            return True

    filtered_files = filter(filterer, files)
    filtered_files.remove('test_runner.cpp')

    return filtered_files

# prepares passed file arguments to match real files
def prepare_files(files):
    def mapper(item):
        return item + '_test.cpp'

    prepared_files = map(mapper, files)

    return prepared_files

# reads compiler option form command line args
def make_compiler(args):
    print args.compiler,
    return args.compiler 

# creates standard and standard library flags
def make_standard_flags(args):
    compiler = "".join(args.compiler)
    standard_flags = ['-std=c++11']

    if compiler == 'clang++':
        standard_flags.append('-stdlib=libc++')

    return standard_flags

# creates error flags
def make_error_flags():
    return ['-Wall', '-W', '-Wextra', '-pedantic-errors']

# creates compile options like -o etc
def make_compile(files):
    if len(files) == 0:
        files = get_all_files()
    else:
        files = prepare_files(files)

    name = make_name()
    return ['-o', name, 'test_runner.cpp'] + files

# creates additional defines
def make_defines():
    return ['-D_GLIBCXX_USE_SCHED_YIELD', '-D_GLIBCXX_USE_NANOSLEEP']

# adds extra needed flags like pthread
def make_extra_flags(args):
    def mapper(arg):
        return "-" + arg

    mapped_args=map(mapper, args.args)

    return ['-pthread'] + mapped_args

# creates optimization options
def make_optimization():
    return ['-O2']

# combines all options
def make_compile_options(args):
    return make_compiler(args) + make_standard_flags(args) + make_error_flags() + make_extra_flags(args) + make_defines() + make_optimization() + make_compile(args.files)

# actual main function, creates compile args and then compiles and runs
def main():
    parser = argparse.ArgumentParser(description="bam test runner")
    parser.add_argument('--compiler', nargs='+', help='enter the compiler to use', default = ['g++'], dest='compiler')
    parser.add_argument('--files', nargs='+', help='enter the files which you want to test, non == all', default =[], dest='files')
    parser.add_argument('--args', nargs='+', help='enter additional options which need to be passed to the compiler', default=[], dest='args')

    args = parser.parse_args()

    print 'compiling ...'
    print 'using', 

    options = make_compile_options(args)
    name = make_name()

    print options
    
    if subprocess.call(options) == 0:
        print 'compiled successfully ...'
        print 'running tests now ...'
        subprocess.call(['./' + name])
        subprocess.call(['rm', '-rf', name])


if __name__ == '__main__':
    main()
