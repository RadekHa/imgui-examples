set(NAME "TestRunner")

add_library(${NAME} OBJECT)

target_sources(${NAME} PUBLIC test_01.cpp)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME} PRIVATE Catch2::Catch2)

