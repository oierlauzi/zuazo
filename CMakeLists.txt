cmake_minimum_required(VERSION 3.9)

project(zuazo 
    VERSION 0.1.0 
    DESCRIPTION "A hardware accelerated video manipulation library"
)

add_subdirectory(${PROJECT_SOURCE_DIR}/doc/doxygen/)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/)
add_subdirectory(${PROJECT_SOURCE_DIR}/test/)

include(GNUInstallDirs)

#Set compiler's options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

#Check if the needed libraries are installed
find_package(ImageMagick REQUIRED Magick++)
find_package(glfw3 REQUIRED)

#zuazo's include path
include_directories(include)

#Include ImageMagick (Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=false)

#Register all source and header files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE INLINE_SOURCES include/*.inl)
file(GLOB_RECURSE HEADERS include/*.h)

#Generate a include file for all the headers
foreach(HEADER ${HEADERS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" HEADER ${HEADER}) #Delete absolute the directory part
    set(INCLUDE_LIST "${INCLUDE_LIST}#include \"${HEADER}\"\n") #Append a include to the include list
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h @ONLY)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)

# Install library headers
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING REGEX "^.*\\.h$")