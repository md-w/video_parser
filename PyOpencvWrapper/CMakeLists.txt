cmake_minimum_required(VERSION 3.17)

project(PyOpencvWrapper)

find_package(Iconv REQUIRED)
message("Iconv ${Iconv_LIBRARY}")

set(OpenCV_STATIC TRUE)
message(STATUS "OpenCV_Search path:" + ${CMAKE_SOURCE_DIR}/thirdparty/install)
find_package( OpenCV REQUIRED PATHS ${CMAKE_SOURCE_DIR}/thirdparty/install)
if(WIN32)
set(ONNXRUNTIME_ROOTDIR "${CMAKE_SOURCE_DIR}/thirdparty/install")
else()
set(ONNXRUNTIME_ROOTDIR "/usr/local")
endif()

include_directories("${ONNXRUNTIME_ROOTDIR}/include" "${ONNXRUNTIME_ROOTDIR}/include/onnxruntime/core/session" "${ONNXRUNTIME_ROOTDIR}/include/onnxruntime/core/providers/cpu")
link_directories("${ONNXRUNTIME_ROOTDIR}/lib")

add_library(ultra_face
    src/UltraFace.cpp
    Sort_tracker/src/Hungarian.cpp
    Sort_tracker/src/KalmanTracker.cpp
    Sort_tracker/src/Sort_tracker.cpp
)

target_include_directories(ultra_face
    PUBLIC inc
    PUBLIC Sort_tracker/inc

)

target_link_libraries(ultra_face
    PUBLIC onnxruntime
    PUBLIC ${OpenCV_LIBS}
)

add_executable(vtpl_opencv
    src/benchmark_opencv.cpp
)

target_include_directories(vtpl_opencv
    PUBLIC inc
)

target_link_libraries(vtpl_opencv
    PUBLIC ${OpenCV_LIBS}
    PUBLIC ultra_face
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
    PUBLIC ultra_face
)

add_executable(vtpl_ultraface
    src/UltraFaceMain.cpp
)

target_include_directories(vtpl_ultraface
    PUBLIC inc
)

target_link_libraries(vtpl_ultraface
    PUBLIC ${OpenCV_LIBS}
    PUBLIC ultra_face
)

pybind11_add_module(PyOpencvWrapper
    src/PyOpencvWrapper.cpp)

add_library(OpencvWrapper SHARED
    src/PyOpencvWrapper.cpp)

target_include_directories(PyOpencvWrapper
    PUBLIC inc)

target_compile_definitions(PyOpencvWrapper
    PRIVATE GENERATE_PYTHON_BINDINGS)

target_link_libraries(PyOpencvWrapper
    PUBLIC ultra_face)

target_include_directories(OpencvWrapper
    PUBLIC inc)

target_compile_definitions(OpencvWrapper
    PRIVATE VP_EXPORTS)

target_link_libraries(PyOpencvWrapper
    PUBLIC ultra_face)

install(FILES $<TARGET_FILE:PyOpencvWrapper>	                            DESTINATION bin)
install(FILES $<TARGET_FILE:OpencvWrapper>	                                DESTINATION bin)
