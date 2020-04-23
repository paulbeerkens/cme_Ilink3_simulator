cmake_minimum_required(VERSION 3.8)

set(HEADER_FILES
        CMESimulator.h)

set(SOURCE_FILES
        CMESimulator.cpp
        Main.cpp)

add_executable(CMESimulator ${SOURCE_FILES} ${HEADER_FILES})

#external libraries
target_link_libraries(CMESimulator libWebServer libLog)

#internal libraries
target_link_libraries(CMESimulator MarketSegmentGateway)