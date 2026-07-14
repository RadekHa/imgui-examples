set(NAME "Types")

add_library(${NAME} INTERFACE
  "Types/ImageInfo.h"
  "Types/DataModel.h"
)

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
