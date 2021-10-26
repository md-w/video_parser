cmake_minimum_required(VERSION 3.17)

project(PyOpencvWrapper)
set(OpenCV_STATIC TRUE)
message(STATUS "OpenCV_Search path:" + ${CMAKE_SOURCE_DIR}/thirdparty/install)
find_package( OpenCV REQUIRED PATHS ${CMAKE_SOURCE_DIR}/thirdparty/install)
set(ONNXRUNTIME_ROOTDIR ${CMAKE_SOURCE_DIR}/thirdparty/install)
message(STATUS "OpenCV_LIBS" ${OpenCV_LIBS})
if(NOT ONNXRUNTIME_ROOTDIR)
  if(WIN32)
    set(ONNXRUNTIME_ROOTDIR "C:/Program Files (x86)/onnxruntime")
  else()
    include_directories("/usr/local/include/onnxruntime")
  endif()
endif()

include_directories("${ONNXRUNTIME_ROOTDIR}/include" "${ONNXRUNTIME_ROOTDIR}/include/onnxruntime/core/session" "${ONNXRUNTIME_ROOTDIR}/include/onnxruntime/core/providers/cpu")
link_directories("${ONNXRUNTIME_ROOTDIR}/lib")

add_executable(vtpl_opencv
    #src/main.cpp
    src/benchmark_opencv.cpp
    src/UltraFace.cpp
)

target_include_directories(vtpl_opencv
    PUBLIC inc
)

target_link_libraries(vtpl_opencv
    PRIVATE onnxruntime
    PUBLIC ${OpenCV_LIBS}
)

add_executable(vtpl_vimagecodec
    #src/main.cpp
    src/AVFReader.cpp
    src/codecManager.cpp
    src/utils.cpp
    src/benchmark_vimagecodec.cpp
    src/UltraFace.cpp
)

target_include_directories(vtpl_vimagecodec
    PUBLIC inc
)

target_link_libraries(vtpl_vimagecodec
    PRIVATE onnxruntime
    PUBLIC ${OpenCV_LIBS}
)

add_executable(vtpl_ultraface
    src/UltraFace.cpp
    src/UltraFacemain.cpp
)

target_include_directories(vtpl_ultraface
    PUBLIC inc
)

target_link_libraries(vtpl_ultraface
    PRIVATE onnxruntime
    PUBLIC ${OpenCV_LIBS}
)
