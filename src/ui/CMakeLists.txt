set(NAME "Ui")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Ui/IAppUi.h"
  "Ui/ImageInfo.h"
  "Ui/src/AppUi.h"
  "Ui/src/AppUi.cpp"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Ui/src)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PUBLIC Types
  PRIVATE project_warnings imgui TraceLog
)