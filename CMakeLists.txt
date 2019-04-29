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
add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=false)

#Include nanoSvg
include_directories(lib/nanosvg/include)

#Generate the pixel format list from libavutil/pixfmt.h
find_file (AV_PIXEL_FORMAT_DIR libavutil/pixfmt.h)
if(AV_PIXEL_FORMAT_DIR-NOTFOUND)
    message(FATAL_ERROR "\"pixfmt.h\" was not found! Check if you have correctly installed all the dependencies!")
endif(AV_PIXEL_FORMAT_DIR-NOTFOUND)

file(READ ${AV_PIXEL_FORMAT_DIR} AV_PIXEL_FORMAT_FILE)
string(REGEX REPLACE "AV_PIX_FMT_NE" "" AV_PIXEL_FORMAT_FILE ${AV_PIXEL_FORMAT_FILE})
string(REGEX REPLACE "AV_PIX_FMT_NB" "" AV_PIXEL_FORMAT_FILE ${AV_PIXEL_FORMAT_FILE})
string(REGEX REPLACE "#define" "" AV_PIXEL_FORMAT_FILE ${AV_PIXEL_FORMAT_FILE})
string(REGEX REPLACE "((#if)|(#else))[^#]*" "" AV_PIXEL_FORMAT_FILE ${AV_PIXEL_FORMAT_FILE})
string(REGEX MATCHALL "(\n[ \t]*AV_PIX_FMT_)([^/,=\n;() ]+)" AV_PIXEL_FORMAT_ENUM ${AV_PIXEL_FORMAT_FILE})


foreach(AV_PIXEL_FORMAT ${AV_PIXEL_FORMAT_ENUM})
    string(REGEX REPLACE "[\n\t ]" "" AV_PIXEL_FORMAT ${AV_PIXEL_FORMAT})
    string(REPLACE "AV_PIX_FMT_" "PIX_FMT_" ZUAZO_PIXEL_FORMAT ${AV_PIXEL_FORMAT})
    message("Supported pixel format found: ${ZUAZO_PIXEL_FORMAT}")
    set(PIX_FMT_LIST "${PIX_FMT_LIST}constexpr PixelFormat ${ZUAZO_PIXEL_FORMAT} = ${AV_PIXEL_FORMAT};\n") #Append the pixel format to the list   
    set(PIX_FMT_SUPPORT_DEFINITIONS "${PIX_FMT_SUPPORT_DEFINITIONS}#define ${ZUAZO_PIXEL_FORMAT}_SUPPORT\n")
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Utils/PixelFormat.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Utils/PixelFormat.h @ONLY)

#Generate the codec list from libavcodec/avcodec.h
find_file (AV_CODECID_DIR libavcodec/avcodec.h)
if(AV_CODECID_DIR-NOTFOUND)
    message(FATAL_ERROR "\"avcodec.h\" was not found! Check if you have correctly installed all the dependencies!")
endif(AV_CODECID_DIR-NOTFOUND)

file(READ ${AV_CODECID_DIR} AV_CODECID_FILE)
string(REGEX REPLACE "AV_PIX_FMT_NE" "" AV_CODECID_FILE ${AV_CODECID_FILE})
string(REGEX REPLACE "AV_PIX_FMT_NB" "" AV_CODECID_FILE ${AV_CODECID_FILE})
string(REGEX REPLACE "#define" "" AV_CODECID_FILE ${AV_CODECID_FILE})
string(REGEX REPLACE "((#if)|(#else))[^#]*" "" AV_CODECID_FILE ${AV_CODECID_FILE})
string(REGEX MATCHALL "(\n[ \t]*AV_CODEC_ID_)([^/,=\n;() ]+)" AV_CODECID_ENUM ${AV_CODECID_FILE})


foreach(AV_CODECID ${AV_CODECID_ENUM})
    string(REGEX REPLACE "[\n\t ]" "" AV_CODECID ${AV_CODECID})
    string(REPLACE "AV_CODEC_ID_" "CODEC_" ZUAZO_CODEC ${AV_CODECID})
    message("Supported codec found: ${ZUAZO_CODEC}")
    set(CODEC_LIST "${CODEC_LIST}constexpr Codec ${ZUAZO_CODEC} = ${AV_CODECID};\n") #Append the pixel format to the list   
    set(CODEC_SUPPORT_DEFINITIONS "${CODEC_SUPPORT_DEFINITIONS}#define ${ZUAZO_CODEC}_SUPPORT\n")
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Utils/Codec.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Utils/Codec.h @ONLY)

#Register all source and header files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS include/*.h)

#Generate a include file for all the headers
foreach(HEADER ${HEADERS})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" HEADER ${HEADER}) #Delete absolute the directory part
    set(INCLUDE_LIST "${INCLUDE_LIST}#include \"${HEADER}\"\n") #Append a include to the include list
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/Includes.h @ONLY)

#Generate documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target( doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
    message(SEND_ERROR "Doxygen is not installed!")
endif (DOXYGEN_FOUND)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)

# Install library headers
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING REGEX "^.*\\.h$")
