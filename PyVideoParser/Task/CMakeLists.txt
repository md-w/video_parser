cmake_minimum_required(VERSION 3.17)

project(Task)

if (defined $ENV{FFMPEG_DIR})
set(FFMPEG_DIR $ENV{FFMPEG_DIR})
endif()

if (DEFINED FFMPEG_INCLUDE_DIR)
	set(FFMPEG_INCLUDE_DIR "${FFMPEG_INCLUDE_DIR}" CACHE PATH "Path to FFmpeg includes")
else()
	set(FFMPEG_INCLUDE_DIR "${FFMPEG_DIR}/include")
endif()

if (DEFINED FFMPEG_LIB_DIR)
	set(FFMPEG_LIB_DIR "${FFMPEG_LIB_DIR}" CACHE PATH "Path to FFmpeg libraries")
else()
	set(FFMPEG_LIB_DIR "${FFMPEG_DIR}/lib")
endif()

message(STATUS "Searching for FFmpeg libs in ${FFMPEG_LIB_DIR}")
message(STATUS "Searching for FFmpeg headers in ${FFMPEG_INCLUDE_DIR}")

find_path( AVCODEC_INCLUDE_DIR libavcodec/avcodec.h PATHS ${FFMPEG_INCLUDE_DIR})
find_path( AVFORMAT_INCLUDE_DIR libavformat/avformat.h PATHS ${FFMPEG_INCLUDE_DIR})
find_path( AVUTIL_INCLUDE_DIR libavutil/avutil.h PATHS ${FFMPEG_INCLUDE_DIR})

find_library( SWRESAMPLE_LIBRARY swresample PATHS ${FFMPEG_LIB_DIR})
find_library( AVFORMAT_LIBRARY avformat PATHS ${FFMPEG_LIB_DIR})
find_library( AVCODEC_LIBRARY avcodec PATHS ${FFMPEG_LIB_DIR})
find_library( AVUTIL_LIBRARY avutil PATHS ${FFMPEG_LIB_DIR})

add_subdirectory(Core)

#Add target;
add_library(Task STATIC
    src/VideoParser.cpp
    src/FFmpegDemuxer.cpp
    src/MemoryInterfaces.cpp
)

target_include_directories(Task
    PUBLIC inc
    PUBLIC ${AVCODEC_INCLUDE_DIR}
    PUBLIC ${AVFORMAT_INCLUDE_DIR}
    PUBLIC ${AVUTIL_INCLUDE_DIR}
)

target_link_libraries(Task
    PUBLIC Core
    PUBLIC ${SWRESAMPLE_LIBRARY}
    PUBLIC ${AVFORMAT_LIBRARY}
    PUBLIC ${AVCODEC_LIBRARY}
    PUBLIC ${AVUTIL_LIBRARY}
)