cmake_minimum_required(VERSION 3.31.6)

project("GAME")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)


add_subdirectory(SDL)
add_subdirectory(SDL_image)
add_subdirectory(SDL_mixer)
add_subdirectory(SDL_ttf)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL_image/include
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL_mixer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/SDL_ttf/include
)

add_library(main SHARED)

target_sources(main PRIVATE main.cpp)

target_link_libraries(main
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf
        # EGL and other dependent libraries required for drawing
        # and interacting with Android system
        EGL
        GLESv3
        jnigraphics
        android
        log)