add_custom_target(examples)

#Find all the examples
file(GLOB_RECURSE EXAMPLES *.cpp)

foreach( example_src ${EXAMPLES} )
    get_filename_component(example_bin_name ${example_src} NAME_WE)
    message(STATUS "Example found: ${example_bin_name}")
    add_executable(${example_bin_name} ${example_src})
    target_link_libraries(${example_bin_name} ${PROJECT_NAME})
endforeach()
