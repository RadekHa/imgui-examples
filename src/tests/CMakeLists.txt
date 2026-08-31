set(NAME "TestRunner")

add_executable(${NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.cpp
)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME}
    PRIVATE
    $<TARGET_OBJECTS:CoreTest>
    $<TARGET_OBJECTS:EventsTest>
    $<TARGET_OBJECTS:ImageTest>
    Image
    Catch2::Catch2WithMain
)

list(APPEND CMAKE_MODULE_PATH ${FETCHCONTENT_BASE_DIR}/catch2-src/extras)
include(Catch)

catch_discover_tests(${NAME})
