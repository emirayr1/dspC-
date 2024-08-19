cmake_minimum_required(VERSION 3.20)

project(CMakeTemplate)

set (CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

message(STATUS "SOURCES: ${SOURCES}")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)