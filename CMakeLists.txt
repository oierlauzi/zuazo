cmake_minimum_required(VERSION 3.9)
include(GNUInstallDirs)

#Project info
project(zuazo 
	VERSION 0.1.0 
	DESCRIPTION "A hardware accelerated video manipulation library"
)

#Subdirectories
add_subdirectory(${PROJECT_SOURCE_DIR}/doc/doxygen/)

#Set compiler's options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

#Register all source and header files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE INLINE_SOURCES ${PROJECT_SOURCE_DIR}/include/*.inl)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

# Create the shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(
	${PROJECT_NAME} PROPERTIES 
		SOVERSION ${PROJECT_VERSION}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/)

# Install library's binary files and headers
install(TARGETS ${PROJECT_NAME} 
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
