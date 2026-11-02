cmake_minimum_required(VERSION 3.22.1)
project(cpp_android_template LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/desktop)

# =============================================================================
# Portable Sources
# =============================================================================
set(PORTABLE_SOURCES
    src/app.h
    src/app.cpp
)

# =============================================================================
# Third-Party Libraries (libs/)
# =============================================================================
# To add a library located in libs/ (or via FetchContent):
#
# Option A: Local subdirectory (e.g. git submodule or folder in libs/)
#   add_subdirectory(libs/my_library)
#
# Option B: FetchContent
#   include(FetchContent)
#   FetchContent_Declare(
#       my_library
#       GIT_REPOSITORY https://github.com/example/my_library.git
#       GIT_TAG        v1.0.0
#   )
#   FetchContent_MakeAvailable(my_library)
# =============================================================================

# =============================================================================
# Platform: Android Target
# =============================================================================
if(ANDROID)
    # 1. Native app glue static library provided by Android NDK
    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    )
    target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
    )

    # 2. Main shared library loaded by NativeActivity
    add_library(cpp_android_template SHARED
        ${PORTABLE_SOURCES}
        platform/android/main.cpp
    )

    target_include_directories(cpp_android_template PRIVATE
        src
    )

    # Prevent linker from stripping ANativeActivity_onCreate
    target_link_options(cpp_android_template PRIVATE -u ANativeActivity_onCreate)

    target_link_libraries(cpp_android_template PRIVATE
        native_app_glue
        android
        log
        EGL
        GLESv3
        # Add third-party target links here:
        # my_library
    )
else()
    # Desktop build placeholder (e.g. GLFW / SDL / desktop platform shim)
    # add_executable(cpp_desktop_template ${PORTABLE_SOURCES} platform/desktop/main.cpp)
    include(FetchContent)
    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)

    # skip wayland and build for X11
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glfw)

    add_executable(desktop src/app.cpp platform/desktop/main.cpp)
    target_link_libraries(desktop PRIVATE glfw GLESv2)
endif()
