add_custom_target(examples)

#Find all the examples
file(GLOB_RECURSE EXAMPLES *.cpp)

foreach( example_src ${EXAMPLES} )
    get_filename_component(example_bin_name ${example_src} NAME_WE)
    message(STATUS "Example found: ${example_bin_name}")
    add_executable(${example_bin_name} ${example_src})
    add_dependencies(${example_bin_name} examples)
endforeach()
