# Copyright (C) 2023 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

# FindLibIIO
# ----------
# Finds the IIO library
#
# This will define the following variables:
#
#  LIBIIO_FOUND - system has libiio
#  LIBIIO_INCLUDE_DIRS - libiio include directory
#  LIBIIO_LIBRARIES - libiio library
#  LIBIIO_DEFINITIONS - compiler switches required for using libiio
#  LIBIIO_VERSION - libiio version

find_package(PkgConfig)
pkg_check_modules(PC_LIBIIO QUIET libiio)

find_path(LIBIIO_INCLUDE_DIRS iio.h
          HINTS ${PC_LIBIIO_INCLUDEDIR} ${PC_LIBIIO_INCLUDE_DIRS}
          PATH_SUFFIXES libiio)

find_library(LIBIIO_LIBRARIES NAMES iio libiio
             HINTS ${PC_LIBIIO_LIBDIR} ${PC_LIBIIO_LIBRARY_DIRS})

set(LIBIIO_DEFINITIONS ${PC_LIBIIO_CFLAGS_OTHER})
set(LIBIIO_VERSION ${PC_LIBIIO_VERSION})

set(_LIBIIO_REQUIRED_VARS LIBIIO_LIBRARIES LIBIIO_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libiio
                                  REQUIRED_VARS ${_LIBIIO_REQUIRED_VARS}
                                  VERSION_VAR LIBIIO_VERSION)

mark_as_advanced(LIBIIO_INCLUDE_DIRS LIBIIO_LIBRARIES)
