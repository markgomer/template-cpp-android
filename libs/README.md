# Third-Party Libraries (`libs/`)

Place third-party submodules, vendor dependencies, or libraries in this directory.

## Integrating Libraries

### Option A: `add_subdirectory` (Vendored / Git Submodule)

1. Add your library into a subdirectory here:
   ```bash
   git submodule add https://github.com/raysan5/raylib.git libs/raylib
   ```
2. In the root `CMakeLists.txt`, register the library:
   ```cmake
   add_subdirectory(libs/raylib)
   ```
3. Link the target to `cpp_android_template`:
   ```cmake
   target_link_libraries(cpp_android_template PRIVATE
       native_app_glue
       android
       log
       EGL
       GLESv3
       raylib
   )
   ```

### Option B: CMake `FetchContent` (Automated Download)

In the root `CMakeLists.txt`, declare and fetch remote dependencies during configuration:
```cmake
include(FetchContent)

FetchContent_Declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG        v0.9.4
)
FetchContent_MakeAvailable(cglm)

target_link_libraries(cpp_android_template PRIVATE
    cglm
)
```
