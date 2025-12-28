cmake_minimum_required(VERSION 3.16)
project(cpp_market_event_engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" OFF)

add_library(market_engine
    src/engine.cpp
    src/parser.cpp
    src/state_store.cpp
)
target_include_directories(market_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(market_engine PUBLIC cxx_std_17)

# Warnings (portable-ish)
if (MSVC)
  target_compile_options(market_engine PRIVATE /W4 /permissive-)
else()
  target_compile_options(market_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(market_event_engine src/main.cpp)
target_link_libraries(market_event_engine PRIVATE market_engine)

if (BUILD_TESTS)
  enable_testing()
  add_executable(test_engine tests/test_engine.cpp)
  target_link_libraries(test_engine PRIVATE market_engine)
  add_test(NAME engine_tests COMMAND test_engine)
endif()
