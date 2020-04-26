set(HEADER_FILES
        ILink3Headers.h
        MessageBuffer.h
        MsgFactory.h
        )

set(SOURCE_FILES
        MessageBuffer.cpp
        MsgFactory.cpp
        )

add_library(ILINK3Messages STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ILINK3Messages INTERFACE ../.) #Allow this library to be included using it's directory name

target_link_libraries(ILINK3Messages libLog)
add_subdirectory(Test)
