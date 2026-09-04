set(NAME "Sdl")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Sdl/DpiHandler.h"              "Sdl/DpiHandler.cpp"
  "Sdl/FrameContext.h"
  "Sdl/IPathService.h"
  "Sdl/ImGuiPass.h"               "Sdl/ImGuiPass.cpp"
  "Sdl/SdlCameraTexture.h"        "Sdl/SdlCameraTexture.cpp"
  "Sdl/SdlContext.h"              "Sdl/SdlContext.cpp"
  "Sdl/SdlEventTranslator.h"      "Sdl/SdlEventTranslator.cpp"
  "Sdl/SdlPathService.h"          "Sdl/SdlPathService.cpp"
  "Sdl/SdlRenderer.h"             "Sdl/SdlRenderer.cpp"
  "Sdl/SdlUtils.h"                "Sdl/SdlUtils.cpp"
  "Sdl/SdlWindow.h"               "Sdl/SdlWindow.cpp"
  "Sdl/SdlWrapper.h"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PUBLIC  SDL3::SDL3-shared Events Types
  PRIVATE project_warnings TraceLog Image Camera imgui Settings
)
