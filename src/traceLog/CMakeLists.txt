set(NAME "TraceLog")

include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

add_library(${NAME} STATIC
  "TraceLog/Log.hpp"
  "TraceLog/Log.cpp"
  "TraceLog/Tracy.hpp"
)

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${NAME} PRIVATE cxx_std_20)
target_link_libraries(${NAME}
  PRIVATE project_warnings
  PUBLIC Tracy::TracyClient fmt spdlog
)
