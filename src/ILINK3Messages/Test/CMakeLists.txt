cmake_minimum_required(VERSION 3.8)

set(SOURCE_FILES
        MessageBufferTests.cpp
        )

find_package(Threads)

add_executable(gtestILink3Messages ${SOURCE_FILES})

target_link_libraries(gtestILink3Messages gtest_main gtest)
target_link_libraries(gtestILink3Messages ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(gtestILink3Messages ILINK3Messages )
