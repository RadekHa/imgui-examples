set(NAME "Camera")

add_library(${NAME} STATIC
  Camera/ICamera.h
  Camera/src/Camera.h
  Camera/src/Camera.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Camera/src)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PRIVATE TraceLog project_warnings opencv::core opencv::videoio opencv::imgproc
)
