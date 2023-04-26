# Find the motor_control library
#
# This module defines the following variables:
#
#  MOTOR_CONTROL_FOUND      - True if the motor_control library was found
#  MOTOR_CONTROL_INCLUDE_DIRS - The include directories for the motor_control library
#  MOTOR_CONTROL_LIBRARIES  - The libraries needed to use motor_control
#  MOTOR_CONTROL_LIBRARY_DIRS - The directory where the motor_control library is installed
#

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(MOTOR_CONTROL QUIET motor_control)
endif()

if (NOT MOTOR_CONTROL_FOUND)
    find_library(MOTOR_CONTROL_LIBRARY NAMES motor_control HINTS ~/install/lib)
    if (MOTOR_CONTROL_LIBRARY)
        set(MOTOR_CONTROL_FOUND ON)
        set(MOTOR_CONTROL_INCLUDE_DIRS "~/install/include")
        set(MOTOR_CONTROL_LIBRARIES ${MOTOR_CONTROL_LIBRARY})
        get_filename_component(MOTOR_CONTROL_LIBRARY_DIRS ${MOTOR_CONTROL_LIBRARY} DIRECTORY)
    endif()
endif()

if (MOTOR_CONTROL_FOUND)
    # Print status message
    message(STATUS "Found motor_control: ${MOTOR_CONTROL_LIBRARY}")
else()
    # Print error message
    message(FATAL_ERROR "Could not find motor_control library")
endif()

# Set variables for dependent targets to link against motor_control
set(MOTOR_CONTROL_LIBRARIES ${MOTOR_CONTROL_LIBRARIES} PARENT_SCOPE)
set(MOTOR_CONTROL_INCLUDE_DIRS ${MOTOR_CONTROL_INCLUDE_DIRS} PARENT_SCOPE)


