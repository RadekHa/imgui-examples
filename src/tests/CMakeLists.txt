set(NAME "TestRunner")

add_executable(${NAME})

target_sources(${NAME}
PRIVATE
   test_01.cpp
)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME} PRIVATE $<TARGET_OBJECTS:CoreTest> Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${FETCHCONTENT_BASE_DIR}/catch2-src/extras)
include(Catch)

catch_discover_tests(${NAME})

