# src/events/CMakeLists.txt
set(NAME "Events")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

# Header-only
add_library(${NAME} INTERFACE
  Events/Bus.h
  Events/BusImpl.h
  Events/Events.h
)

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

