
SET(SRC4Client client_main.c)
SET(SRC4Server server_main.c)
SET(CMAKE_C_COMPILER "g++")

MESSAGE(STATUS 
        "2 files:"
        "\n\t ${CMAKE_CURRENT_SOURCE_DIR}/${SRC4Client}"
        "\n\t ${CMAKE_CURRENT_SOURCE_DIR}/${SRC4Server}"
        )

# target

ADD_EXECUTABLE(client ${SRC4Client})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

MESSAGE(STATUS "Output path(after 'make'):"
        "\n\t ${EXECUTABLE_OUTPUT_PATH}"
        "\n\t or ${LIBRARY_OUTPUT_PATH}"
        )

ADD_EXECUTABLE(server ${SRC4Server})
TARGET_LINK_LIBRARIES (server pthread)

# install 

INSTALL(TARGETS         client  server 
        DESTINATION     test4UDP_InstallDir_canDelete
        )

MESSAGE(STATUS "Install path((after 'make install')):"
        "\n\t ${CMAKE_INSTALL_PREFIX} ."
        "\n\t You can change it like this when 'cmake ..':"
        "\n\t cmake -DCMAKE_INSTALL_PREFIX=/usr/local/bin .."
        )


