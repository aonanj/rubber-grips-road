cmake_minimum_required(VERSION 3.16)
project(RubberGripsRoad VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sensor_lib
    src/SensorChannel.cpp
    src/Display.cpp
    src/SimulationSensors.cpp
)

target_include_directories(sensor_lib PUBLIC include)

add_executable(tire_monitor src/main.cpp)

target_link_libraries(tire_monitor PRIVATE sensor_lib)
