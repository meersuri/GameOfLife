add_executable(main main.cpp universe.cpp cell.cpp painter.cpp animator.cpp)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(main PRIVATE -g -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined)
target_link_libraries(main libasan.a libubsan.a)

add_library(game_of_life STATIC universe.cpp cell.cpp painter.cpp animator.cpp)
target_include_directories(game_of_life PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(bench benchmark.cpp universe.cpp cell.cpp)
target_include_directories(bench PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(bench PRIVATE -O3 -march=native)
