cmake_minimum_required(VERSION 3.8)

set(SOURCE_FILES
        TestTinyXMLApp.cpp
        )

find_package(Threads)



add_executable(TestTinyXMLApp ${SOURCE_FILES})

target_link_libraries(TestTinyXMLApp ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(TestTinyXMLApp tinyxml )