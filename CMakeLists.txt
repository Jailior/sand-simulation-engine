cmake_minimum_required(VERSION 3.15)
project(SandSimulationEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_executable(SandSimulationEngine src/main.cpp src/algorithm.cpp src/texture_tools.cpp)
target_link_libraries(SandSimulationEngine ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
