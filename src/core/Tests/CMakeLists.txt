# Test cases
set(NAME "ResourcesTest")

add_executable(${NAME})

target_sources(${NAME}
PUBLIC
   Resources.spec.cpp
)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME} PRIVATE $<TARGET_OBJECTS:TestRunner> Catch2::Catch2WithMain Core)

list(APPEND CMAKE_MODULE_PATH ${FETCHCONTENT_BASE_DIR}/catch2-src/extras)
include(Catch)

catch_discover_tests(${NAME})
