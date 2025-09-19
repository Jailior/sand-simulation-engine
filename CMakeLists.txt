cmake_minimum_required(VERSION 3.15)
project(SandSimulationEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_executable(SandSimulationEngine main.cpp)
target_link_libraries(SandSimulationEngine ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
