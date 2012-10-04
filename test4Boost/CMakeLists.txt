cmake_minimum_required(VERSION 2.8)

project(test)

SET(CMAKE_C_COMPILER "g++")
SET(SRC_LIST main.cpp)

#INCLUDE_DIRECTORIES(/usr/local/include)
#LINK_DIRECTORIES(/usr/local/lib)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} boost_thread boost_system)

