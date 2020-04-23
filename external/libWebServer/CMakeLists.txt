cmake_minimum_required(VERSION 3.8)
project(libWebServer VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

#Do not add another lib to the library name
set(CMAKE_STATIC_LIBRARY_PREFIX "")

add_compile_options(-Wall -Wextra -pedantic -Werror)

#set code coverage flags for debug builds only
#set(CMAKE_CXX_FLAGS_DEBUG "-fprofile-arcs -ftest-coverage") //breakpoints stop working when added

add_subdirectory(webserver)
add_subdirectory(TestApps)
add_subdirectory(Test)



#installation
#file(GLOB HEADERS *.h)
#install(FILES ${HEADERS} DESTINATION ~/source/pkg/include/${PROJECT_NAME})
#install(TARGETS ${PROJECT_NAME} DESTINATION ~/source/pkg/lib)
#add_custom_target(install_${PROJECT_NAME}
#        $(MAKE) install
#        DEPENDS ${PROJECT_NAME}
#        COMMENT "Installing ${PROJECT_NAME}")



