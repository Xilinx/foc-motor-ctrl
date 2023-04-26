# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(foc_motor_ctrl)

# Add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Find the motor_control library
find_package(libiio REQUIRED)

# Find the pybind11 library
find_package(pybind11 REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})

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
    lib/src/python_bind.cpp
)

# Create the library target
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories for the library
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>)

#Create Python module target for the motor-control
pybind11_add_module(py_${PROJECT_NAME} lib/src/python_bind.cpp)

# Add any required dependencies for the library
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBIIO_LIBRARIES} pybind11::embed)
target_link_libraries(py_${PROJECT_NAME} PRIVATE ${PROJECT_NAME})

# Set library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# Set compile options for the library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
#target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wno-unused-variable -Wno-unused-parameter)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

# Add python module name to the Macro to be used for pybind11 module
add_compile_definitions(PY_MODULE_NAME=py_${PROJECT_NAME})

include(GNUInstallDirs)

# Set install targets for the library
install(TARGETS ${PROJECT_NAME} py_${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY lib/include/ DESTINATION include)

# generate and install CMake package configuration files
include(CMakePackageConfigHelpers)

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME})

export(PACKAGE ${PROJECT_NAME})

# Add any tests for the library
# add_subdirectory(test)
