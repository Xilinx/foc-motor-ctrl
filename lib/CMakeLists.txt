# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

# Define Library Target
add_library(${FMC_LIB_NAME} SHARED)

# Set library properties
set_target_properties(${FMC_LIB_NAME} PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# Add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Find the dependent libraries
find_package(libiio REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${LIBIIO_INCLUDE_DIRS}
)

# Add library sources
target_sources(${FMC_LIB_NAME} PRIVATE
    src/motor-control.cpp
    src/sensors/qei.cpp
    src/sensors/smo.cpp
    src/foc.cpp
    src/pwm.cpp
    src/svpwm.cpp
    src/adchub.cpp
    src/sensors/sensor.cpp
    src/interface/iio_drv.cpp
    src/python_bind.cpp
)

# Set include directories for the library
target_include_directories(${FMC_LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

#Create Python module target for the motor-control
pybind11_add_module(${PY_FMC_LIB} src/python_bind.cpp)

# Add any required dependencies for the library
target_link_libraries(${FMC_LIB_NAME}
	PUBLIC
		${LIBIIO_LIBRARIES}
	PRIVATE
		pybind11::embed
)

target_link_libraries(${PY_FMC_LIB} PRIVATE ${FMC_LIB_NAME})

# Set compile options for the library
target_compile_features(${FMC_LIB_NAME} PUBLIC cxx_std_11)
target_compile_options(${FMC_LIB_NAME} PRIVATE -Wall -Wextra -pedantic)

# Add python module name to the Macro to be used for pybind11 module
add_compile_definitions(PY_MODULE_NAME=${PY_FMC_LIB})

# Set install targets for the library
install(TARGETS ${FMC_LIB_NAME} ${PY_FMC_LIB}
        EXPORT ${FMC_LIB_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION include)

# generate and install CMake package configuration files
include(CMakePackageConfigHelpers)

install(EXPORT ${FMC_LIB_NAME}-targets
        FILE ${FMC_LIB_NAME}-targets.cmake
        NAMESPACE ${FMC_LIB_NAME}::
        DESTINATION lib/cmake/${FMC_LIB_NAME}
)

export(PACKAGE ${FMC_LIB_NAME})
