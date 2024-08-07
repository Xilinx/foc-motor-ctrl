# Copyright (C) 2023-2024 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.10)

# Add application source files
#add_executable(fmc_cmdline cmdline.cpp)

# Link application with the motor_control library
#target_link_libraries(fmc_cmdline ${FOC_MOTOR_CTRL_LIBRARIES})

if(BUILD_CANOPEN_APP)
	set(APP fmc_canopen)

	# Find the required Lely Libraries
	find_package(PkgConfig REQUIRED)
	pkg_search_module(LIBLELY_COAPP REQUIRED liblely-coapp)

	# Source files for the canopen app
	set(SOURCES
		canopen.cpp
	)

	# Create the canopen executable
	add_executable(${APP} ${SOURCES})

	target_compile_features(${APP} PUBLIC cxx_std_14)

	# Link libraries for the canopen executable
	target_link_libraries(${APP} PRIVATE ${FMC_LIB_NAME} ${LIBLELY_COAPP_LIBRARIES})

	target_include_directories(${APP} PUBLIC ${LIBLELY_COAPP_INCLUDE_DIRS} ${FMC_LIBRARIES})

	install(TARGETS ${APP}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

	install(FILES foc-mc.eds
		DESTINATION share/foc-motor-ctrl/
	)

	install(PROGRAMS launch_server.sh
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		RENAME start_motor_server
	)

endif()

if(BUILD_CMDLINE_APP)
	set(APP fmc_cmdline)

	# Source files for the cmdline app
	set(SOURCES
		cmdline.cpp
	)

	# Create the canopen executable
	add_executable(${APP} ${SOURCES})

	target_compile_features(${APP} PUBLIC cxx_std_11)

	# Link libraries for the canopen executable
	target_link_libraries(${APP} PRIVATE ${FMC_LIB_NAME})

	install(TARGETS ${APP}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endif()

if(BUILD_DASHBOARD_APP)
	set(APP start_motor_dashboard)

	install(PROGRAMS launch_dashboard.sh
		DESTINATION ${CMAKE_INSTALL_BINDIR}
		RENAME ${APP}
	)

	install(FILES mc_bokeh.py
		DESTINATION share/foc-motor-ctrl/dashboard
		RENAME main.py
	)

endif()
