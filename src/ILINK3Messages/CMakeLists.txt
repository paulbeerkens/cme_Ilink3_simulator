set(HEADER_FILES
        ILink3Field.h
        ILink3Headers.h
        ILink3Msg.h
        MessageBuffer.h
        MsgFactory.h
        MsgFactorySettings.h
        )

set(SOURCE_FILES
        ILink3Field.cpp
        ILink3Msg.cpp
        MessageBuffer.cpp
        MsgFactory.cpp
        )

add_library(ILINK3Messages STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ILINK3Messages INTERFACE ../.) #Allow this library to be included using it's directory name

target_link_libraries(ILINK3Messages libLog)
target_link_libraries(ILINK3Messages tinyxml )

add_subdirectory(Test)
add_subdirectory(TestApp)
