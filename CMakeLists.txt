# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

project(foc_motor_ctrl)

set(FMC_LIB_NAME ${PROJECT_NAME})
set(PY_FMC_LIB py_${PROJECT_NAME})

# Options for building apps and tests
option(BUILD_TEST "Build the test applications to test the libraries" OFF)
option(BUILD_CANOPEN_APP "Build the canopen based motor control application" OFF)
option(BUILD_CMDLINE_APP "Build the command line based motor control application" OFF)
option(BUILD_DASHBOARD_APP "Build the python based motor control application dashboard" OFF)
option(SKIP_POST_INSTALL "Skip the post installation, that can be taken care by packaging system" OFF)

include(GNUInstallDirs)

# Include the subdirectories for building the library, apps and test
add_subdirectory(lib)
add_subdirectory(apps)

if(BUILD_TEST)
	add_subdirectory(test)
endif()
