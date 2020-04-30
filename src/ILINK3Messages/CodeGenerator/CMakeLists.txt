project (CodeGenerator)

add_executable(CodeGenerator CodeGenerator.cpp)

target_link_libraries(CodeGenerator tinyxml)
target_link_libraries(CodeGenerator ILINK3Common)
