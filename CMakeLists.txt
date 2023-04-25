# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(motor_control)

# Add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Find the motor_control library
find_package(libiio REQUIRED)

# Add header files and source files to the library
set(HEADER_FILES
    lib/include/motor-control/motor-control.hpp
)
set(SOURCE_FILES
    lib/src/motor-control.cpp
    lib/src/sensors/qei.cpp
    lib/src/sensors/smo.cpp
    lib/src/sensors/sensor.cpp
    lib/src/interface/iio_drv.cpp
)

# Create the library target
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories for the library
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
    $<INSTALL_INTERFACE:include>
)

# Set library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# Add any required dependencies for the library
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBIIO_LIBRARIES})

# Set compile options for the library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
#target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-unused-variable -Wno-unused-parameter)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# Set install targets for the library
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include)

install(DIRECTORY lib/include/ DESTINATION include)

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME})

# Add any tests for the library
# add_subdirectory(apps)
