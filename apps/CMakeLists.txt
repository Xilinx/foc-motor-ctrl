cmake_minimum_required(VERSION 3.10)

project(test_cxx)

# Add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Find the motor_control library
find_package(motor_control REQUIRED)

# Add the include directories for the motor_control library
include_directories(${MOTOR_CONTROL_INCLUDE_DIRS})

# Add application source files
add_executable(${PROJECT_NAME} cmdline.cpp)

# Link application with the motor_control library
target_link_libraries(${PROJECT_NAME} ${MOTOR_CONTROL_LIBRARIES})

