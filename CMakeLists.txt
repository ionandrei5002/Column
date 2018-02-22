cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)

set(HEADERS types.h bytebuffer.h column.h operators.h value.h array.h)

project(Column)

add_executable(${PROJECT_NAME} main.cpp ${HEADERS})

target_compile_options(${PROJECT_NAME}
  PRIVATE
    -flto
    -march=native
    -m64
    -std=c++17
    -O3
#    -O
#    -g3
    -floop-parallelize-all
    -ftree-parallelize-loops=4
)
