set(HEADER_FILES
        Ilink3EnumsGen.h
        ILink3Headers.h
        ILink3Msg.h
        ILink3MsgDefinition.h
        MessageBuffer.h
        MsgFactory.h
        MsgFactorySettings.h
        )

set(SOURCE_FILES
        ILink3Msg.cpp
        ILink3MsgDefinition.cpp
        MessageBuffer.cpp
        MsgFactory.cpp
        )

message (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
message (CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
        COMMAND CodeGenerator ${PROJECT_SOURCE_DIR}/data/Cme.Futures.iLink3.Sbe.v8.5.xml
        DEPENDS ${PROJECT_SOURCE_DIR}/data/Cme.Futures.iLink3.Sbe.v8.5.xml
        OUTPUT Ilink3EnumsGen.h
)

add_library(ILINK3Messages STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ILINK3Messages INTERFACE ../.) #Allow this library to be included using it's directory name
target_include_directories(ILINK3Messages PUBLIC ${CMAKE_CURRENT_BINARY_DIR}) #adds the location of the generated files


target_link_libraries(ILINK3Messages libLog)
target_link_libraries(ILINK3Messages tinyxml)
target_link_libraries(ILINK3Messages libWebServer)
target_link_libraries(ILINK3Messages ILINK3Common)

add_subdirectory(Test)
add_subdirectory(TestApp)
add_subdirectory(CodeGenerator)
