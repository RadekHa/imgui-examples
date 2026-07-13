set(NAME "Ui")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Ui/IAppUi.h"
  "Ui/ImageInfo.h"
  "Ui/src/IUiState.h"
  "Ui/src/AppUi.h"
  "Ui/src/AppUi.cpp"
  "Ui/src/StateNull.h"
  "Ui/src/StateLogin.h"
  "Ui/src/StateNull.cpp"
  "Ui/src/StateStart.h"
  "Ui/src/StateStart.cpp"
  "Ui/src/StateLogin.h"
  "Ui/src/StateLogin.cpp"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Ui/src)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PUBLIC Types
  PRIVATE project_warnings imgui TraceLog
)