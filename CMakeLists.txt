cmake_minimum_required(VERSION 3.9)

#Project info
project(zuazo 
    VERSION 0.1.0 
    DESCRIPTION "A hardware accelerated video manipulation library"
)

#zuazo's include path
include_directories(${PROJECT_SOURCE_DIR}/include)

include(GNUInstallDirs)

#Set compiler's options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

#Subdirectories TODO
add_subdirectory(${PROJECT_SOURCE_DIR}/doc/doxygen/)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/)
add_subdirectory(${PROJECT_SOURCE_DIR}/test/)

#Register all source and header files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE INLINE_SOURCES include/*.inl)
file(GLOB_RECURSE HEADERS include/*.h)

#Generate a include file for all the headers
foreach(header ${HEADERS})
    file(RELATIVE_PATH header ${PROJECT_SOURCE_DIR}/include ${header})
    set(HEADER_LIST "${HEADER_LIST}#include \"${header}\"\n") #Append a include to the include list
endforeach()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h @ONLY)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})

# Install library's binary files and headers
install(
    TARGETS ${PROJECT_NAME} 
    DESTINATION lib/
)
install(
    DIRECTORY include/ 
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING REGEX "^.*\\.(h|inl)$"
)
