cmake_minimum_required(VERSION 3.17)
project(xtx)

set(CMAKE_CXX_STANDARD 17)

add_executable(xtx main.cpp node.cpp space.cpp token.cpp invocation.cpp
    err.cpp read.cpp print.cpp eval.cpp map.cpp num.cpp arith.cpp cmd.cpp
    dyn.cpp
)
include_directories(xtx .)