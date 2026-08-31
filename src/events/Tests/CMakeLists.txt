# Test cases
set(NAME "EventsTest")

add_library(${NAME} OBJECT)

target_sources(${NAME}
PUBLIC
   EventBusTest.cpp
)

target_compile_features(${NAME} PRIVATE cxx_std_20)

target_link_libraries(${NAME} PRIVATE Catch2::Catch2 Events)