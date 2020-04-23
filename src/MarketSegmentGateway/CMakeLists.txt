
set (HEADER_FILES
        MarketSegmentGateway.h
        MSGWSettings.h)

set(SOURCE_FILES
        MarketSegmentGateway.cpp
        )

add_library(MarketSegmentGateway STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(MarketSegmentGateway INTERFACE ../.) #Allow this library to be included using it's directory name

target_link_libraries(MarketSegmentGateway libLog)
target_link_libraries(MarketSegmentGateway FIXPConnection)