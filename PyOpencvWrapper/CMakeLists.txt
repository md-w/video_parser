cmake_minimum_required(VERSION 3.17)

project(PyOpencvWrapper)

find_package(Git REQUIRED)

include(ExternalProject)

# OpenCV
set(OPENCV_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/opencv)
ExternalProject_Add(opencv
        GIT_REPOSITORY https://github.com/opencv/opencv
        GIT_TAG 4.5.4 # 4.0.0-alpha
        SOURCE_DIR opencv
        BINARY_DIR opencv-build
        CMAKE_ARGS
          -DBUILD_SHARED_LIBS=OFF
          -DBUILD_opencv_world=OFF
          -DBUILD_opencv_java=OFF
          -DBUILD_opencv_python2=OFF
          -DBUILD_opencv_python3=OFF
          -DWITH_PYTHON=OFF
          -DWITH_CUDA=OFF
          -DBUILD_opencv_apps=OFF
          -DBUILD_EXAMPLES=OFF
          -DBUILD_DOCS=OFF
          -DBUILD_PERF_TESTS=OFF
          -DBUILD_TESTS=OFF
          -DWITH_TBB=ON
          -DWITH_OPENMP=ON
          -DWITH_IPP=ON
          -DCMAKE_INSTALL_PREFIX=${OPENCV_INSTALL_LOCATION}
        )
# include_directories(${OPENCV_INSTALL_LOCATION}/include/opencv4)
# link_directories(${OPENCV_INSTALL_LOCATION}/lib)
set(OpenCV_STATIC TRUE)

find_package( OpenCV REQUIRED PATHS ${OPENCV_INSTALL_LOCATION})

add_executable(vtpl
    src/main.cpp
)

target_link_libraries(vtpl 
    PUBLIC ${OpenCV_LIBS}
)
