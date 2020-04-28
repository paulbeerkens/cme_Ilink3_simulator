set(HEADER_FILES
        ILink3FieldDefinition.h
        ILink3Headers.h
        ILink3Msg.h
        ILink3MsgDefinition.h
        MessageBuffer.h
        MsgFactory.h
        MsgFactorySettings.h
        )

set(SOURCE_FILES
        ILink3FieldDefinition.cpp
        ILink3Msg.cpp
        ILink3MsgDefinition.cpp
        MessageBuffer.cpp
        MsgFactory.cpp
        )

add_library(ILINK3Messages STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ILINK3Messages INTERFACE ../.) #Allow this library to be included using it's directory name

target_link_libraries(ILINK3Messages libLog)
target_link_libraries(ILINK3Messages tinyxml)
target_link_libraries(ILINK3Messages libWebServer)

add_subdirectory(Test)
add_subdirectory(TestApp)
