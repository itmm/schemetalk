cmake_minimum_required(VERSION 3.17)
project(st)

set(CMAKE_CXX_STANDARD 17)

add_custom_command(
    COMMAND c-cat --base=${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/chap-*.md | md-patcher
    DEPENDS ${CMAKE_SOURCE_DIR}/chap-*.md
    OUTPUT err.cpp err.h invocation.cpp invocation.h io.cpp io.h map.h node.cpp node.h parser.cpp parser.h space.h st.cpp token.h
        num.h arith.cpp arith.h
)

add_executable(st err.cpp invocation.cpp io.cpp node.cpp parser.cpp st.cpp arith.cpp)
include_directories(st .)

