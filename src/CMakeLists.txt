add_executable(main main.cpp universe.cpp cell.cpp)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(main PRIVATE -g -O0 -Wall -Wextra)
