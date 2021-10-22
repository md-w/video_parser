cmake_minimum_required(VERSION 3.17)

project(PyOpencvWrapper)



set(OpenCV_STATIC TRUE)

# find_package(OpenCV REQUIRED)
message(STATUS "OpenCV_LIBS:" + ${OPENCV_LIBRARIES} + ${OPENCV_LIBRARY_DIR})
message(STATUS "OpenCV_INCLUDE_DIR:" + ${OPENCV_INCLUDE_DIR})
set(OpenCV_STATIC TRUE)
set(OpenCV_RUNTIME vc16)
find_package( OpenCV REQUIRED PATHS ${CMAKE_BINARY_DIR}/thirdparty)

add_executable(vtpl_opencv
    #src/main.cpp
    src/benchmark_opencv.cpp
)

target_link_libraries(vtpl_opencv 
    PUBLIC ${OpenCV_LIBS}
)

add_executable(vtpl_vimagecodec
    #src/main.cpp
    src/AVFReader.cpp
    src/codecManager.cpp
    src/utils.cpp
    src/benchmark_vimagecodec.cpp
)

target_include_directories(vtpl_vimagecodec
    PUBLIC inc
)

target_link_libraries(vtpl_vimagecodec
    PUBLIC ${OpenCV_LIBS}
)
