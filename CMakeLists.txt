cmake_minimum_required(VERSION 3.22.1)

project(gbxlzo)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall")

add_executable(${PROJECT_NAME} src/main.c src/gbx.c src/minilzo/minilzo.c)
file(COPY TestFiles DESTINATION ${CMAKE_BINARY_DIR})

enable_testing()

add_test(
    NAME AutoTest
    COMMAND gbxlzo TestFiles/BenchTMN.Challenge.Gbx
)

add_test(
    NAME DecompressTest
    COMMAND gbxlzo -d TestFiles/BenchTMN.Challenge.Gbx
)

add_test(
    NAME CompressTest
    COMMAND gbxlzo -c TestFiles/BenchTMN.Challenge.Gbx
)

add_test(
    NAME OutputTest
    COMMAND gbxlzo TestFiles/BenchTMN.Challenge.Gbx -o out.Challenge.Gbx
)

