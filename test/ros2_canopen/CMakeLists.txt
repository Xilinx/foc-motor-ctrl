# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.8)
project(kria_motor_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(canopen REQUIRED)
find_package(lely_core_libraries REQUIRED)

cogen_dcf(402_system)
generate_dcf(402_control)

install(DIRECTORY
  launch
  urdf
  DESTINATION share/${PROJECT_NAME}
  )

ament_package()
