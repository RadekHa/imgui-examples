# Test cases
set(NAME "ImageTest")

add_library(${NAME} OBJECT)

target_sources(${NAME}
PUBLIC
   ImageTest.cpp
)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME} PRIVATE Catch2::Catch2 Image)
