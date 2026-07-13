set(NAME "Ui")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Ui/AppUi.h"
  "Ui/ImageInfo.h"

  "Ui/AppUi.cpp"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src/core)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PRIVATE project_warnings imgui TraceLog
)