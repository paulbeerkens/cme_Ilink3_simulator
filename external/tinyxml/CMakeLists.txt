set(HEADER_FILES
        tinystr.h
        tinyxml.h
        )

set(SOURCE_FILES
        tinystr.cpp
        tinyxml.cpp
        tinyxmlerror.cpp
        tinyxmlparser.cpp
        )

#add_definitions(-DTIXML_USE_STL)

add_library(tinyxml STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(tinyxml INTERFACE ../.) #Allow this library to be included using it's directory name