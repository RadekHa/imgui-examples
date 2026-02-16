include(FetchContent)

# Dependencies

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
  GIT_TAG "v3.11.0"
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG 11.0.2
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG docking
)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-2.30.9
)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG v1.15.0
)

FetchContent_Declare(
  tracy
  GIT_REPOSITORY "https://github.com/wolfpld/tracy.git"
  GIT_TAG master
#  GIT_TAG v0.13.2
)

# Settings

# For SDL2 to be able to override options
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(FMT_INSTALL OFF)
set(SDL2_DISABLE_SDL2MAIN ON)
set(SPDLOG_FMT_EXTERNAL ON)

add_subdirectory(imgui-setup)

# Populate

FetchContent_MakeAvailable(Catch2 fmt imgui SDL2 spdlog tracy)
