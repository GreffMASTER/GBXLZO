cmake_minimum_required(VERSION 3.22.1)

project(gbxlzo)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall")

add_executable(${PROJECT_NAME} src/main.c src/gbx.c src/minilzo/minilzo.c)


