set(NAME "Camera")

add_library(${NAME} STATIC
  Camera/ICamera.h
  Camera/Camera.h
  Camera/Camera.cpp
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PRIVATE TraceLog project_warnings opencv::core opencv::videoio opencv::imgproc
  PUBLIC  
)
