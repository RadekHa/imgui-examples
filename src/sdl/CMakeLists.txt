set(NAME "Sdl")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Sdl/DpiHandler.h"              "Sdl/src/DpiHandler.cpp"
  "Sdl/FrameContext.h"
  "Sdl/IPathService.h"
  "Sdl/ImGuiPass.h"               "Sdl/src/ImGuiPass.cpp"
  "Sdl/SdlCameraTexture.h"        "Sdl/src/SdlCameraTexture.cpp"
  "Sdl/SdlContext.h"              "Sdl/src/SdlContext.cpp"
  "Sdl/SdlEventTranslator.h"      "Sdl/src/SdlEventTranslator.cpp"
  "Sdl/SdlPathService.h"          "Sdl/src/SdlPathService.cpp"
  "Sdl/SdlRenderer.h"             "Sdl/src/SdlRenderer.cpp"
  "Sdl/SdlUtils.h"                "Sdl/src/SdlUtils.cpp"
  "Sdl/SdlWindow.h"               "Sdl/src/SdlWindow.cpp"
  "Sdl/SdlWrapper.h"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Sdl/src)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PUBLIC  SDL2::SDL2 imgui Settings Events
  PRIVATE project_warnings TraceLog Image
)
