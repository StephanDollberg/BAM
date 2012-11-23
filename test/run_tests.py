#!/usr/bin/python

import os
import subprocess
import argparse

def make_name():
    return 'tester'

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

def make_compiler(args):
    return args.compiler 

def make_standard_flags(args):
    compiler = "".join(args.compiler)
    standard_flags = ['-std=c++11']

    if compiler == 'clang++':
        standard_flags.append('-stdlib=libc++')

    return standard_flags

def make_error_flags():
    return ['-Wall', '-Werror', '-Wextra', '-pedantic-errors']

def make_compile(files):
    if len(files) == 0:
        files = get_all_files()

    name = make_name()
    return ['-o', name, 'test_runner.cpp'] + files

def make_defines():
    return ['-D_GLIBCXX_USE_SCHED_YIELD', '-D_GLIBCXX_USE_NANOSLEEP']

def make_optimization():
    return ['-O2']

def make_compile_options(args):
    return make_compiler(args) + make_standard_flags(args) + make_error_flags() + make_defines() + make_optimization() + make_compile(args.files)

def main():
    parser = argparse.ArgumentParser(description="bam test runner")
    parser.add_argument('--compiler', nargs='+', help='enter the compiler to use', default = ['clang++'], dest='compiler')
    parser.add_argument('--files', nargs='+', help='enter the files which you want to test, non == all', default =[], dest='files')

    args = parser.parse_args()

    options = make_compile_options(args)
    name = make_name()

    if subprocess.call(options) == 0:
        subprocess.call(['./' + name])
        subprocess.call(['rm', '-rf', name])


if __name__ == '__main__':
    main()