cmake_minimum_required(VERSION 2.8)
project(OverBotFramework)

find_package( Threads REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -Wall")


set(SOURCE_FILES
        Source/FrameComms.cpp
        Source/Source.cpp
        Source/utils.cpp
        )

set(HEADER_FILES 
        Header/utils.h
        Header/FrameComms.h
        ../Shared/shared.h
        )

include_directories(
)

link_directories(${CMAKE_SOURCE_DIR}/lib)
add_executable(OverBotFramework ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(OverBotFramework ${CMAKE_THREAD_LIBS_INIT})
