set(HEADER_FILES
        FIXPConnection.h
        )

set(SOURCE_FILES
        FIXPConnection.cpp
        )

add_library(FIXPConnection STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(FIXPConnection INTERFACE ../.) #Allow this library to be included using it's directory name

target_link_libraries(FIXPConnection libLog)
target_link_libraries(FIXPConnection ILINK3Messages)
