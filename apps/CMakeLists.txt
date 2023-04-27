cmake_minimum_required(VERSION 3.10)

if(BUILD_CANOPEN_APP)
	#Source file s
endif()
# Add application source files
#add_executable(fmc_cmdline cmdline.cpp)

# Link application with the motor_control library
#target_link_libraries(fmc_cmdline ${FOC_MOTOR_CTRL_LIBRARIES})

if(BUILD_CANOPEN_APP)
	set(APP fmc_canopen)

	# Source files for the canopen app
	set(SOURCES
		canopen.cpp
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
	set(APP bokeh_dashboard)

	# Source files for the python app
	set(SOURCES
		mc_bokeh.py
	)

	# Nothing to build

	install(FILES ${SOURCES}
		DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
endif()
