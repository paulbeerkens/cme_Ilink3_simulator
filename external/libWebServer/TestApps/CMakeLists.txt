cmake_minimum_required(VERSION 3.8)

set(SOURCE_FILES Main.cpp)

add_executable(TestApp ${SOURCE_FILES})

target_link_libraries(TestApp PUBLIC libWebServer)