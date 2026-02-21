cmake_minimum_required(VERSION 3.10)
project(ChordDHT)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chord_dht main.cpp node.cpp node.h)
