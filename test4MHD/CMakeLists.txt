cmake_minimum_required(VERSION 2.8)
PROJECT(test4MHD)
SET(CMAKE_C_COMPILER "g++")

SET(SRC_LIST main.c)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../common/include /usr/local/include)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../common/lib /usr/local/lib)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} LOG 

libnettle.a libp11-kit.so libgmp.a libgnutls.a gcrypt gpg-error 

pthread gmp nettle p11-kit gnutls libmicrohttpd.a)





