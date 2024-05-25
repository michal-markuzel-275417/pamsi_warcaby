cmake_minimum_required(VERSION 3.28)
project(pamsi_warcaby)

set(CMAKE_CXX_STANDARD 17)

add_executable(pamsi_warcaby main.cpp
        checkersBoard.cpp
        checkersBoard.h
        gameHandler.cpp
        gameHandler.h
        gameAlgorithm.cpp
        gameAlgorithm.h)
