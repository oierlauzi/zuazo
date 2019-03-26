cmake_minimum_required(VERSION 3.9)

project(zuazo 
    VERSION 0.1.0 
    DESCRIPTION "A hardware accelerated video manipulation library"
)

include(GNUInstallDirs)

#Set up project's properties
set (CMAKE_CXX_STANDARD 17) #Set compile parameters


#zuazo's include path
include_directories(include)

#Include ImageMagick (Magick++)
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
add_compile_definitions(MAGICKCORE_QUANTUM_DEPTH=16)
add_compile_definitions(MAGICKCORE_HDRI_ENABLE=false)

#Include nanoSvg
include_directories(lib/nanosvg/include)

#Register all source and header files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS include/*.h)


# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 0.1.0)

# Install library
install(TARGETS ${PROJECT_NAME} DESTINATION lib/)

# Install library headers
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME} FILES_MATCHING REGEX "^.*\\.h$")
