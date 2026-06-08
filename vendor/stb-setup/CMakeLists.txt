FetchContent_MakeAvailable(stb_image)

add_library(stb_image_internal INTERFACE)

target_include_directories(stb_image_internal INTERFACE ${stb_image_download_SOURCE_DIR})

add_library(stb::image ALIAS stb_image_internal)
