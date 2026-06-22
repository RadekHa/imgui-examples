set(NAME "Image")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Image/IImageData.h"
  "Image/src/ImageData.h"
  "Image/src/ImageData.cpp"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Image/src)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PRIVATE project_warnings TraceLog stb::image
)
