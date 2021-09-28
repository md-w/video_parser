cmake_minimum_required(VERSION 3.17)

project(PyVideoParser)
add_subdirectory(Task)

pybind11_add_module(PyVideoParser 
    src/PyVideoParser.cpp)

add_library(VideoParser SHARED
    src/PyVideoParser.cpp)

target_include_directories(PyVideoParser 
    PUBLIC inc)

target_compile_definitions(PyVideoParser
    PRIVATE GENERATE_PYTHON_BINDINGS)

target_link_libraries(PyVideoParser
    PUBLIC Task)

target_include_directories(VideoParser
    PUBLIC inc)

target_compile_definitions(VideoParser
    PRIVATE VP_EXPORTS)

target_link_libraries(VideoParser
    PUBLIC Task)

install(FILES $<TARGET_FILE:PyVideoParser>	                                DESTINATION bin)
install(FILES $<TARGET_FILE:VideoParser>	                                DESTINATION bin)