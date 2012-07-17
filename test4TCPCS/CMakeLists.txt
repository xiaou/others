cmake_minimum_required(VERSION 2.8.7)

project(test4TCPCS)

SET(CMAKE_C_COMPILER "g++")

SET(SRC4Client client.c)
SET(SRC4Server server2.c)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# target add unp lib

SET(UNP_LIB_DIR ${PROJECT_SOURCE_DIR}/../unp_lib)
INCLUDE_DIRECTORIES(${UNP_LIB_DIR})
LINK_DIRECTORIES(${UNP_LIB_DIR})

# target

ADD_EXECUTABLE(client ${SRC4Client})
TARGET_LINK_LIBRARIES(client libunp.a)

ADD_EXECUTABLE(server ${SRC4Server})
TARGET_LINK_LIBRARIES(server unp)





