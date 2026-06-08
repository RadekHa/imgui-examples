set(NAME "Core")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "Core/AppEvents.h"
  "Core/Application.cpp"
  "Core/Application.hpp"
  "Core/AppUi.cpp"
  "Core/DpiHandler.cpp"
  "Core/DpiHandler.h"
  "Core/EventBus.h"
  "Core/EventBusImpl.h"
  "Core/ImGuiPass.cpp"
  "Core/Log.cpp"
  "Core/Log.hpp"
  "Core/SdlContext.cpp"
  "Core/SdlContext.h"
  "Core/SdlEventTranslator.cpp"
  "Core/SdlPathService.cpp"
  "Core/SdlRenderer.cpp"
  "Core/SdlWindow.cpp"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
  PRIVATE project_warnings Tracy::TracyClient
  PUBLIC fmt spdlog SDL2::SDL2 imgui Settings)


add_subdirectory(Tests)
