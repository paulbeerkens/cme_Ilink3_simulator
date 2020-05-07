set(HEADER_FILES
        Generated/ILink3ConstGen.h
        Generated/ILink3EnumsGen.h
        Generated/ILink3MsgsGen.h
        ILink3Headers.h
        Generated/ILink3Msg.h
        ILink3MsgDefinition.h
        MessageBuffer.h
        MsgFactory.h
        MsgFactorySettings.h
        )

set(SOURCE_FILES
        Generated/ILink3Msg.cpp
        ILink3MsgDefinition.cpp
        MessageBuffer.cpp
        MsgFactory.cpp
        )

message (PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
message (CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
        COMMAND CodeGenerator ${PROJECT_SOURCE_DIR}/data/Cme.Futures.iLink3.Sbe.v8.5.xml
        DEPENDS ${PROJECT_SOURCE_DIR}/data/Cme.Futures.iLink3.Sbe.v8.5.xml
        OUTPUT Generated/ILink3EnumsGen.h
               Generated/ILink3ConstGen.h
               Generated/ILink3MsgsGen.h
)

add_library(ILINK3Messages STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ILINK3Messages INTERFACE ../.) #Allow this library to be included using it's directory name
target_include_directories(ILINK3Messages PUBLIC ${CMAKE_CURRENT_BINARY_DIR}) #adds the location of the generated files


target_link_libraries(ILINK3Messages libLog)
target_link_libraries(ILINK3Messages tinyxml)
target_link_libraries(ILINK3Messages libWebServer)
target_link_libraries(ILINK3Messages ILINK3Common)
target_link_libraries(ILINK3Messages FIXPConnection)

add_subdirectory(Test)
add_subdirectory(TestApp)
add_subdirectory(CodeGenerator)
add_subdirectory(Generated)
