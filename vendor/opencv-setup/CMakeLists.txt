
set(BUILD_LIST "core,imgproc,videoio" CACHE STRING "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python3 OFF CACHE BOOL "" FORCE)
set(WITH_FFMPEG OFF CACHE BOOL "" FORCE)
set(WITH_IPP OFF CACHE BOOL "" FORCE)
set(BUILD_WITH_STATIC_CRT OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(opencv)

add_library(opencv::core ALIAS opencv_core)
target_include_directories(opencv_core INTERFACE 
    "$<BUILD_INTERFACE:${opencv_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${opencv_SOURCE_DIR}/modules/core/include>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
)

add_library(opencv::videoio ALIAS opencv_videoio)
target_include_directories(opencv_videoio INTERFACE 
    "$<BUILD_INTERFACE:${opencv_SOURCE_DIR}/modules/videoio/include>"
)

add_library(opencv::imgproc ALIAS opencv_imgproc)
target_include_directories(opencv_imgproc INTERFACE 
    "$<BUILD_INTERFACE:${opencv_SOURCE_DIR}/modules/imgproc/include>"
)
