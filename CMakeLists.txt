cmake_minimum_required(VERSION 3.22)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code
        src/main.cpp
        src/BPlusTree.cpp
        src/account.cpp
        src/train.cpp
        src/time.cpp
)