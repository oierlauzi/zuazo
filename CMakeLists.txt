cmake_minimum_required(VERSION 3.9)
include(GNUInstallDirs)

#Project info
project(zuazo 
	VERSION 0.1.0 
	DESCRIPTION "A hardware accelerated video manipulation library"
)

#Subdirectories
add_subdirectory(${PROJECT_SOURCE_DIR}/shaders/)
add_subdirectory(${PROJECT_SOURCE_DIR}/doc/doxygen/)

#Set compiler's options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

#Register all source and header files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE INLINE_SOURCES ${PROJECT_SOURCE_DIR}/include/*.inl)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

#Generate a include file for all the headers
foreach(header ${HEADERS})
	file(RELATIVE_PATH header ${PROJECT_SOURCE_DIR}/include ${header})
	set(HEADER_LIST "${HEADER_LIST}#include \"${header}\"\n") #Append a include to the include list
endforeach()
set(INCLUDES_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/Includes.h")
configure_file(${PROJECT_SOURCE_DIR}/include/Includes.h.in ${INCLUDES_HEADER} @ONLY)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(
	${PROJECT_NAME} PROPERTIES 
		SOVERSION ${PROJECT_VERSION}	
)
add_dependencies(${PROJECT_NAME} shaders)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/)
target_include_directories(${PROJECT_NAME} PRIVATE ${SHADER_INCLUDE_DIR}/)

# Install library's binary files and headers
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)
install(FILES ${INCLUDES_HEADER} DESTINATION include/${PROJECT_NAME})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING REGEX "^.*\\.(h|inl)$")
