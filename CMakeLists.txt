cmake_minimum_required(VERSION 3.0...3.22.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(
    waterfall
    VERSION 0.1.0
    LANGUAGES C CXX
)
set(CMAKE_TOOLCHAIN_FILE ./vcpkg/scripts/buildsystems/vcpkg.cmake)

#-fsanitize=address -fno-omit-frame-pointer

add_executable(waterfall.exe
	main.c
	Application/adapter.c
	Application/device.c
	library/linear/MatrixN.c
	library/linear/Matrix.c
	library/linear/VectorN.c
	library/linear/Vector.c
	library/rxi/Array.c
)
set_target_properties(waterfall.exe PROPERTIES
    C_STANDARD 23
    COMPILE_WARNING_AS_ERROR ON
		RUNTIME_OUTPUT_DIRECTORY ../
)
target_compile_definitions(waterfall.exe PRIVATE
    RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources"
)
set(glfw3_DIR ./vcpkg_installed/x64-linux/share/glfw3)
find_package(glfw3 CONFIG REQUIRED)
set(libwebsockets_DIR ./vcpkg_installed/x64-linux/share/libwebsockets)
find_package(libwebsockets CONFIG REQUIRED)
include_directories("./library")
if (NOT EMSCRIPTEN)
include_directories("./vcpkg_installed/x64-linux/include")
endif()
add_subdirectory(library/glfw3webgpu)
add_subdirectory(library/dawn)
add_subdirectory(library/cimgui)
target_link_options(waterfall.exe PRIVATE -lstdc++)
target_link_libraries(waterfall.exe PRIVATE stdc++ glfw webgpu glfw3webgpu cimgui websockets)

if (MSVC)
    target_compile_options(waterfall.exe PRIVATE /W4)
else()
    target_compile_options(waterfall.exe PRIVATE -Wall -Wextra -pedantic)
endif()
if(XCODE)
    set_target_properties(waterfall.exe PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
endif()

if (EMSCRIPTEN)
		set_target_properties(waterfall.exe PROPERTIES SUFFIX ".html")
    target_link_options(waterfall.exe PRIVATE
        -sUSE_GLFW=3 # Use Emscripten-provided GLFW
        -sUSE_WEBGPU # Handle WebGPU symbols
        -sALLOW_MEMORY_GROWTH
        --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/resources"
#        -sASYNCIFY # Required by WebGPU-C++
    )
endif()

target_copy_webgpu_binaries(waterfall.exe)
