#!/bin/bash

#project:

#Clean
rm -rf  test/*

#Enter test (and create if not exists):
mkdir test 2>/dev/null
cd test

#project init
../bin/project init -n make
../bin/project init -n make_exe_fast --exe
../bin/project init -n make_lib_fast --lib

../bin/project init -n bash -b bash
../bin/project init -n bash_exe_fast -b bash --exe
../bin/project init -n bash_lib_fast -b bash --lib

#project info #1
project_info(){
    cd $1
    ../../bin/project info
    cd ..
}

project_info make
project_info make_exe_fast
project_info make_lib_fast

project_info bash
project_info bash_exe_fast
project_info bash_lib_fast

#project scripts: run #1
project_run(){
    cd $1
    chmod +x run
    chmod +x .scripts/*.sh
    ./run
    cd ..
}

project_exe(){
    cd $1_exe_fast
    ./$1_exe_fast
    cd ..
    cd $1_lib_fast
    ./test/test
    cd ..
}

project_run make
project_run make_exe_fast
project_run make_lib_fast

project_exe make

project_run bash
project_run bash_exe_fast
project_run bash_lib_fast

project_exe bash

#project enable log
project_enable(){
    cd $1
    ../../bin/project enable log
    cd ..
}

project_enable make
project_enable make_exe_fast
project_enable make_lib_fast

project_enable bash
project_enable bash_exe_fast
project_enable bash_lib_fast

#project info #2

project_info make
project_info make_exe_fast
project_info make_lib_fast

project_info bash
project_info bash_exe_fast
project_info bash_lib_fast

#project script: run #2

project_run make
project_run make_exe_fast
project_run make_lib_fast

project_exe make

project_run bash
project_run bash_exe_fast
project_run bash_lib_fast

project_exe bash

#project disable log
project_disable(){
    cd $1
    ../../bin/project disable log
    cd ..
}

project_disable make
project_disable make_exe_fast
project_disable make_lib_fast

project_disable bash
project_disable bash_exe_fast
project_disable bash_lib_fast

#project info #3

project_info make
project_info make_exe_fast
project_info make_lib_fast

project_info bash
project_info bash_exe_fast
project_info bash_lib_fast

#project script: run #3

project_run make
project_run make_exe_fast
project_run make_lib_fast

project_exe make

project_run bash
project_run bash_exe_fast
project_run bash_lib_fast

project_exe bash

#Exit test:
cd ..