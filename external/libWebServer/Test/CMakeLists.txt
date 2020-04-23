cmake_minimum_required(VERSION 3.8)

set(SOURCE_FILES
        atest.cpp
        Running.cpp
        URLRequesTests.cpp
        WebUtilsTest.cpp
        )

find_package(Threads)

add_executable(gtestLibWebserver ${SOURCE_FILES})

target_link_libraries(gtestLibWebserver gtest_main gtest)
target_link_libraries(gtestLibWebserver ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(gtestLibWebserver libWebServer )