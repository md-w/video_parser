cmake_minimum_required(VERSION 3.17)

project(VideoParser LANGUAGES CUDA CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (WIN32)
set(FFMPEG_DIR ${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg CACHE PATH "Path to FFMpeg")
endif()


set(TRACK_TOKEN_ALLOCATIONS FALSE CACHE BOOL "Debug memory allocations within VPF")

if(TRACK_TOKEN_ALLOCATIONS)
	add_definitions(-DTRACK_TOKEN_ALLOCATIONS)
endif(TRACK_TOKEN_ALLOCATIONS)

#Undef MIN & MAX & C runtime warnings for Windows
if(WIN32)
	add_definitions(-DNOMINMAX)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)
add_subdirectory(thirdparty/pybind11)
add_subdirectory(PyVideoParser)

#Bring all the stuff together
# install(FILES $<TARGET_FILE:TC_CORE>										DESTINATION bin)
# install(FILES $<TARGET_FILE:TC>												DESTINATION bin)

if(GENERATE_PYTHON_BINDINGS)
	install(FILES $<TARGET_FILE:PyVideoParser>								DESTINATION bin)
endif(GENERATE_PYTHON_BINDINGS)

