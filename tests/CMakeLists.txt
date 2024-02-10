add_executable(tests tests.cpp)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(tests PRIVATE -g)
target_link_libraries(tests GTest::gtest_main game_of_life)
include(GoogleTest)
gtest_discover_tests(tests)

