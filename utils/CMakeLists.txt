find_package(absl COMPONENTS debugging failure_signal_handler PATHS ${CMAKE_SOURCE_DIR}/thirdparty/install_debug)
find_package(spdlog PATHS ${CMAKE_SOURCE_DIR}/thirdparty/install_debug)

add_library(utils STATIC
    src/filesystem.cpp
    src/logging.cpp
    inc/threadsafe_queue.h)

target_include_directories(utils
    PUBLIC inc)
target_link_libraries(utils
    PUBLIC absl::debugging
    PUBLIC absl::failure_signal_handler
    PUBLIC spdlog::spdlog)