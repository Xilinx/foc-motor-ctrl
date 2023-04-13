# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(MotorControl)

# Add header files and source files to the library
set(HEADER_FILES
    lib/include/MotorController.hpp
    lib/include/Sensor.hpp
    lib/include/QEI.hpp
    lib/include/Sensorless.hpp
    lib/include/ADC.hpp
    lib/include/GateDriver.hpp
    lib/include/FOC.hpp
)
set(SOURCE_FILES
    lib/src/MotorController.cpp
    lib/src/QEI.cpp
    lib/src/Sensorless.cpp
    lib/src/ADC.cpp
    lib/src/GateDriver.cpp
    lib/src/FOC.cpp
)

# Create the library target
add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories for the library
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add any required dependencies for the library
target_link_libraries(${PROJECT_NAME} PUBLIC <dependencies>)

# Set compile options for the library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# Set install targets for the library
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME})

# Add any tests for the library
enable_testing()
add_subdirectory(test)
