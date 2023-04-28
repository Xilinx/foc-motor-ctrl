cmake_minimum_required(VERSION 3.10)

##### cpp_libtest #####
# Add application source files
add_executable(cpp_libtest cpp_libtest.cpp)

target_compile_features(cpp_libtest PUBLIC cxx_std_11)

# Link application with the motor_control library
target_link_libraries(cpp_libtest ${FMC_LIB_NAME})

# Install the application
install(TARGETS cpp_libtest
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



##### py_libtest #####

install(PROGRAMS py_libtest.py
	DESTINATION ${CMAKE_INSTALL_BINDIR}
	RENAME py_libtest
)
