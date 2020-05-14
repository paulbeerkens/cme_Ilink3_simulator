set(HEADER_FILES
        ILink3CompositeDefinition.h
        ILink3FieldDefinition.h
        ILink3EnumDefinition.h
        ILink3MsgDefinition.h
        )

set(SOURCE_FILES
        ILink3CompositeDefinition.cpp
        ILink3FieldDefinition.cpp
        ILink3EnumDefinition.cpp
        ILink3MsgDefinition.cpp
        )

add_library(ILINK3Common STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(ILINK3Common INTERFACE ../.) #Allow this library to be included using it's directory name





