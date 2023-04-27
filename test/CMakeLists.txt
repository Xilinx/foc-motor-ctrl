cmake_minimum_required(VERSION 3.10)

project(cpp_libtest)

# Add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Find the motor_control library
find_package(foc_motor_ctrl REQUIRED)

# Add the include directories for the motor_control library
include_directories(${FOC_MOTOR_CTRL_INCLUDE_DIRS})

# Add application source files
add_executable(${PROJECT_NAME} cpp_libtest.cpp)

# Link application with the motor_control library
target_link_libraries(${PROJECT_NAME} ${FOC_MOTOR_CTRL_LIBRARIES})

