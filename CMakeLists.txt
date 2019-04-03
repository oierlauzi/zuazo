cmake_minimum_required(VERSION 3.9)

project(zuazo 
    VERSION 0.1.0 
    DESCRIPTION "A hardware accelerated video manipulation library"
)

include(GNUInstallDirs)

#Set up project's properties
set (CMAKE_CXX_STANDARD 17) #Set compile parameters

#Check if the needed libraries are installed
find_package(ImageMagick REQUIRED Magick++)
find_package(glfw3 REQUIRED)

#zuazo's include path
include_directories(include)

#Include ImageMagick (Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
add_compile_definitions(MAGICKCORE_QUANTUM_DEPTH=16)
add_compile_definitions(MAGICKCORE_HDRI_ENABLE=false)

#Include nanoSvg
include_directories(lib/nanosvg/include)

#Register all source and header files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS include/*.h)

#Generate a include file for all the headers
foreach(HEADER ${HEADERS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" HEADER ${HEADER}) #Delete absolute the directory part
    set(INCLUDE_LIST "${INCLUDE_LIST}#include \"${HEADER}\"\n") #Append a include to the include list
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h)

#Generate documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    message("Building doxygen documentation")

    add_custom_target( doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
        
endif (DOXYGEN_FOUND)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)

# Install library headers
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING REGEX "^.*\\.h$")
