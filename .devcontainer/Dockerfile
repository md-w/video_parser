FROM ubuntu:20.04

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends ca-certificates software-properties-common \
    git curl wget clang-format ninja-build build-essential gdb python3-tk python3-pip python3-setuptools \
    python3-dev python3-venv ffmpeg libavcodec-dev libavfilter-dev libavformat-dev libavutil-dev libgtk-3-dev pkg-config

RUN wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.20.5/cmake-3.20.5-linux-x86_64.sh \
    && sh cmake.sh --prefix=/usr/local/ --exclude-subdir && rm -rf cmake.sh

ENV SHELL /bin/bash

RUN pip3 install -U pip
RUN pip3 install numpy
RUN pip3 install virtualenv
RUN pip3 install black pylint pytest yapf
# RUN pip3 install numpy opencv-python
RUN pip install ruamel.yaml

RUN mkdir opencv_build && cd opencv_build && \
    git clone --depth 1 --origin "4.5.4" https://github.com/opencv/opencv_contrib && \
    git clone --depth 1 --origin "4.5.4" https://github.com/opencv/opencv && \
    mkdir cmake-build && cd cmake-build && \
    cmake -DENABLE_CXX11=ON \
        -DBUILD_opencv_core:BOOL=ON \
        -DBUILD_opencv_imgproc:BOOL=ON \
        -DBUILD_opencv_highgui:BOOL=ON \
        -DBUILD_opencv_video:BOOL=ON \
        -DBUILD_opencv_flann:BOOL=ON \
        -DBUILD_opencv_ml:BOOL=ON \
        -DBUILD_opencv_features2d:BOOL=ON \
        -DWITH_FFMPEG:BOOL=ON \
        -DBUILD_opencv_calib3d:BOOL=ON \
        -DBUILD_opencv_objdetect:BOOL=ON \
        -DBUILD_opencv_contrib:BOOL=ON \
        -DBUILD_JPEG:BOOL=ON \
        -DBUILD_ZLIB:BOOL=ON \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DWITH_CUDA:BOOL=OFF \
        -DWITH_NVCUVID:BOOL=OFF \
        -DWITH_EIGEN:BOOL=OFF \
        -DBUILD_DOCS:BOOL=OFF \
        -DBUILD_EXAMPLES:BOOL=OFF \
        -DBUILD_opencv_python2=OFF \
        -DBUILD_opencv_python3=ON \
        -DWITH_PYTHON=ON \
        -DBUILD_NEW_PYTHON_SUPPORT:BOOL=ON \
        -DENABLE_FAST_MATH:BOOL=ON \
        -DBUILD_WITH_DEBUG_INFO:BOOL=OFF \
        -DBUILD_PACKAGE:BOOL=OFF \
        -DBUILD_opencv_apps:BOOL=OFF \
        -DBUILD_opencv_gpu:BOOL=OFF \
        -DBUILD_opencv_legacy:BOOL=OFF \
        -DBUILD_opencv_java:BOOL=OFF \
        -DBUILD_opencv_photo:BOOL=OFF \
        -DBUILD_opencv_nonfree:BOOL=OFF \
        -DBUILD_opencv_ocl:BOOL=OFF \
        -DBUILD_opencv_stitching:BOOL=OFF \
        -DBUILD_opencv_superres:BOOL=OFF \
        -DBUILD_opencv_ts:BOOL=OFF \
        -DBUILD_opencv_videostab:BOOL=OFF \
        -DBUILD_TESTS:BOOL=OFF \
        -DBUILD_PERF_TESTS:BOOL=OFF \
        -DBUILD_WITH_CAROTENE:BOOL=OFF \
        -DBUILD_opencv_world:BOOL=OFF \
        -DWITH_IPP:BOOL=OFF \
        -DBUILD_WITH_STATIC_CRT:BOOL=OFF \
        -DBUILD_FAT_JAVA_LIB:BOOL=OFF \
        -DOPENCV_EXTRA_MODULES_PATH="../opencv_contrib/modules" \
        "../opencv" -G"Ninja" && \
        cmake --build . --config Release --target install && \
        cd ../.. && \
        rm -rf opencv_build

RUN mkdir onnxruntime_build && cd onnxruntime_build && \
    git clone --depth 1 --origin "1.9.1" --recursive https://github.com/microsoft/onnxruntime.git && \
    cd onnxruntime && \
    sh build.sh --config Release --target install --skip_tests --build_shared_lib --parallel && \
    cd ../.. && \
    rm -rf onnxruntime_build

RUN mkdir poco_build && cd poco_build && \
    git clone --depth 1 --origin "poco-serial-1.11.0-release" --recursive https://github.com/vtpl1/poco.git && \
    mkdir cmake-build && cd cmake-build && \
    cmake -DBUILD_SHARED_LIBS=NO \
        "../poco" -G"Ninja" && \
        cmake --build . --config Release --target install && \
        cd ../.. && \
        rm -rf poco_build

RUN mkdir spdlog_build && cd spdlog_build && \
    git clone --depth 1 --origin "v1.9.2" --recursive https://github.com/gabime/spdlog.git && \
    mkdir cmake-build && cd cmake-build && \
    cmake -DBUILD_SHARED_LIBS=NO \
        "../spdlog" -G"Ninja" && \
        cmake --build . --config Release --target install && \
        cd ../.. && \
        rm -rf spdlog_build

RUN mkdir abseil_build && cd abseil_build && \
    git clone --depth 1 --origin "lts_2021_03_24" --recursive https://github.com/abseil/abseil-cpp.git && \
    mkdir cmake-build && cd cmake-build && \
    cmake -DABSL_ENABLE_INSTALL=ON \
        "../abseil-cpp" -G"Ninja" && \
        cmake --build . --config Release --target install && \
        cd ../.. && \
        rm -rf abseil_build

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

RUN groupmod --gid $USER_GID $USERNAME \
    && usermod --uid $USER_UID --gid $USER_GID $USERNAME \
    && chown -R $USER_UID:$USER_GID /home/$USERNAME