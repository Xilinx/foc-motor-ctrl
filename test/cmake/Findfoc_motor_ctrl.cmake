# Find the foc_motor_ctrl library
#
# This module defines the following variables:
#
#  FOC_MOTOR_CTRL_FOUND      - True if the foc_motor_ctrl library was found
#  FOC_MOTOR_CTRL_INCLUDE_DIRS - The include directories for the foc_motor_ctrl library
#  FOC_MOTOR_CTRL_LIBRARIES  - The libraries needed to use foc_motor_ctrl
#  FOC_MOTOR_CTRL_LIBRARY_DIRS - The directory where the foc_motor_ctrl library is installed
#

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(FOC_MOTOR_CTRL QUIET foc_motor_ctrl)
endif()

if (NOT FOC_MOTOR_CTRL_FOUND)
    find_library(FOC_MOTOR_CTRL_LIBRARY NAMES foc_motor_ctrl HINTS ~/install/lib)
    if (FOC_MOTOR_CTRL_LIBRARY)
        set(FOC_MOTOR_CTRL_FOUND ON)
        set(FOC_MOTOR_CTRL_INCLUDE_DIRS "~/install/include")
        set(FOC_MOTOR_CTRL_LIBRARIES ${FOC_MOTOR_CTRL_LIBRARY})
        get_filename_component(FOC_MOTOR_CTRL_LIBRARY_DIRS ${FOC_MOTOR_CTRL_LIBRARY} DIRECTORY)
    endif()
endif()

if (FOC_MOTOR_CTRL_FOUND)
    # Print status message
    message(STATUS "Found foc_motor_ctrl: ${FOC_MOTOR_CTRL_LIBRARY}")
else()
    # Print error message
    message(FATAL_ERROR "Could not find foc_motor_ctrl library")
endif()

# Set variables for dependent targets to link against foc_motor_ctrl
set(FOC_MOTOR_CTRL_LIBRARIES ${FOC_MOTOR_CTRL_LIBRARIES} PARENT_SCOPE)
set(FOC_MOTOR_CTRL_INCLUDE_DIRS ${FOC_MOTOR_CTRL_INCLUDE_DIRS} PARENT_SCOPE)


