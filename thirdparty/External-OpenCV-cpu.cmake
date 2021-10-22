message("External project: opencv_cpu")
set(opencv_version 4.5.4)

include(ExternalProject)

ExternalProject_Add(opencv_cpu_contrib
    GIT_REPOSITORY https://github.com/opencv/opencv_contrib
    GIT_TAG ${opencv_version}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/opencv-src/opencv_contrib
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
if(MSVC)
ExternalProject_Add(opencv_cpu
    GIT_REPOSITORY https://github.com/opencv/opencv
    GIT_TAG ${opencv_version}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/opencv-src/opencv
    BINARY_DIR ${CMAKE_BINARY_DIR}/opencv-build/opencv
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    DEPENDS opencv_cpu_contrib
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=$<CONFIG>
        #-DWITH_INF_ENGINE:BOOL=ON
        -DENABLE_CXX11=ON
        -DBUILD_opencv_core:BOOL=ON
        -DBUILD_opencv_imgproc:BOOL=ON
        -DBUILD_opencv_highgui:BOOL=ON
        -DBUILD_opencv_video:BOOL=ON
        -DBUILD_opencv_flann:BOOL=ON
        -DBUILD_opencv_ml:BOOL=ON
        -DBUILD_opencv_features2d:BOOL=ON
        -DWITH_FFMPEG:BOOL=ON
        -DBUILD_opencv_calib3d:BOOL=ON
        -DBUILD_opencv_objdetect:BOOL=ON
        -DBUILD_opencv_contrib:BOOL=ON
        -DBUILD_JPEG:BOOL=ON
        -DBUILD_ZLIB:BOOL=ON
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DWITH_CUDA:BOOL=OFF    
        -DWITH_NVCUVID:BOOL=OFF
        -DWITH_EIGEN:BOOL=OFF
        -DBUILD_DOCS:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_opencv_python2=OFF
        -DBUILD_opencv_python3=OFF
        -DWITH_PYTHON=OFF
        -DBUILD_NEW_PYTHON_SUPPORT:BOOL=OFF
        -DBUILD_WITH_DEBUG_INFO:BOOL=OFF
        -DBUILD_PACKAGE:BOOL=OFF
        -DBUILD_opencv_apps:BOOL=OFF
        -DBUILD_opencv_gpu:BOOL=OFF
        -DBUILD_opencv_legacy:BOOL=OFF        
        -DBUILD_opencv_java:BOOL=OFF        
        -DBUILD_opencv_photo:BOOL=OFF
        -DBUILD_opencv_nonfree:BOOL=OFF
        -DBUILD_opencv_ocl:BOOL=OFF
        -DBUILD_opencv_stitching:BOOL=OFF
        -DBUILD_opencv_superres:BOOL=OFF
        -DBUILD_opencv_ts:BOOL=OFF
        -DBUILD_opencv_videostab:BOOL=OFF        
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_PERF_TESTS:BOOL=OFF        
        -DBUILD_WITH_CAROTENE:BOOL=OFF
        -DBUILD_opencv_world:BOOL=OFF
        -DWITH_IPP:BOOL=OFF
        -DBUILD_WITH_STATIC_CRT:BOOL=OFF
        -DBUILD_FAT_JAVA_LIB:BOOL=OFF
        -DOPENCV_EXTRA_MODULES_PATH=${CMAKE_BINARY_DIR}/opencv-src/opencv_contrib/modules
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
        #-DInferenceEngine_DIR:PATH="C:/Program Files (x86)/IntelSWTools/openvino_2019.1.087/inference_engine/share"
        -DINF_ENGINE_RELEASE=2019010001
)
else()
ExternalProject_Add(opencv_cpu
    GIT_REPOSITORY https://github.com/opencv/opencv
    GIT_TAG ${opencv_version}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/opencv-src/opencv
    BINARY_DIR ${CMAKE_BINARY_DIR}/opencv-build/opencv
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    DEPENDS opencv_cpu_contrib
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=$<CONFIG>
        #-DWITH_INF_ENGINE:BOOL=ON
        -DENABLE_CXX11=ON
        -DBUILD_opencv_core:BOOL=ON
        -DBUILD_opencv_imgproc:BOOL=ON
        -DBUILD_opencv_highgui:BOOL=ON
        -DBUILD_opencv_video:BOOL=ON
        -DBUILD_opencv_flann:BOOL=ON
        -DBUILD_opencv_ml:BOOL=ON
        -DBUILD_opencv_features2d:BOOL=ON
        -DWITH_FFMPEG:BOOL=ON
        -DBUILD_opencv_calib3d:BOOL=ON
        -DBUILD_opencv_objdetect:BOOL=ON
        -DBUILD_opencv_contrib:BOOL=ON
        -DBUILD_JPEG:BOOL=ON
        -DBUILD_ZLIB:BOOL=ON
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DWITH_CUDA:BOOL=OFF    
        -DWITH_NVCUVID:BOOL=OFF
        -DWITH_EIGEN:BOOL=OFF
        -DBUILD_DOCS:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_NEW_PYTHON_SUPPORT:BOOL=OFF
        -DBUILD_WITH_DEBUG_INFO:BOOL=OFF
        -DBUILD_PACKAGE:BOOL=OFF
        -DBUILD_opencv_apps:BOOL=OFF
        -DBUILD_opencv_gpu:BOOL=OFF
        -DBUILD_opencv_legacy:BOOL=OFF        
        -DBUILD_opencv_java:BOOL=OFF        
        -DBUILD_opencv_photo:BOOL=OFF
        -DBUILD_opencv_nonfree:BOOL=OFF
        -DBUILD_opencv_ocl:BOOL=OFF
        -DBUILD_opencv_stitching:BOOL=OFF
        -DBUILD_opencv_superres:BOOL=OFF
        -DBUILD_opencv_ts:BOOL=OFF
        -DBUILD_opencv_videostab:BOOL=OFF        
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_PERF_TESTS:BOOL=OFF        
        -DBUILD_WITH_CAROTENE:BOOL=OFF
        -DBUILD_opencv_world:BOOL=OFF
        -DWITH_IPP:BOOL=OFF
        -DBUILD_WITH_STATIC_CRT:BOOL=OFF
        -DBUILD_FAT_JAVA_LIB:BOOL=OFF
        -DOPENCV_EXTRA_MODULES_PATH=${CMAKE_BINARY_DIR}/opencv-src/opencv_contrib/modules
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/thirdparty
        #-DInferenceEngine_DIR:PATH="C:/Program Files (x86)/IntelSWTools/openvino_2019.1.087/inference_engine/share"
        -DINF_ENGINE_RELEASE=2019010001
)
endif()

if(MSVC)
    if(MSVC_VERSION EQUAL 1400)
        set(OpenCV_MSVC_PATH vc8)
    elseif(MSVC_VERSION EQUAL 1500)
        set(OpenCV_MSVC_PATH vc9)
    elseif(MSVC_VERSION EQUAL 1600)
        set(OpenCV_MSVC_PATH vc10)
    elseif(MSVC_VERSION EQUAL 1700)
        set(OpenCV_MSVC_PATH vc11)
    elseif(MSVC_VERSION EQUAL 1800)
        set(OpenCV_MSVC_PATH vc12)
    elseif(MSVC_VERSION EQUAL 1900)
        set(OpenCV_MSVC_PATH vc14)
    elseif(MSVC_VERSION MATCHES "^191[0-9]$")
        set(OpenCV_MSVC_PATH vc15)
    elseif(MSVC_VERSION MATCHES "^192[0-9]$")
        set(OpenCV_MSVC_PATH vc16)
    elseif(MSVC_VERSION MATCHES "^193[0-9]$")
        set(OpenCV_MSVC_PATH vc17)
    else()
        message(WARNING "OpenCV does not recognize MSVC_VERSION \"${MSVC_VERSION}\". Cannot set OpenCV_MSVC_PATH")
    endif()
endif()

set(OPENCV_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/include/)
if(NOT MSVC)
  set(OPENCV_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/lib/)
else()
  set(OPENCV_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/x64/${OpenCV_MSVC_PATH}/lib)
endif()

set(OPENCV_LIBRARIES opencv_imgproc opencv_core opencv_highgui opencv_video opencv_videoio opencv_imgcodecs opencv_features2d)

include_directories(${OPENCV_INCLUDE_DIR})
link_directories(${OPENCV_LIBRARY_DIR})

if(EXISTS "${CMAKE_BINARY_DIR}/thirdparty/OpenCVConfig.cmake")
    include(${CMAKE_BINARY_DIR}/thirdparty/OpenCVConfig.cmake)
    add_custom_target(rerun)
else()
    add_custom_target(rerun ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS opencv)
endif()
