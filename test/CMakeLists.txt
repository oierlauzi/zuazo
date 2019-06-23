add_custom_target(test)

#Find all the examples
file(GLOB_RECURSE TESTS *.cpp)

foreach( test_src ${TESTS} )
    get_filename_component(test_bin_name ${test_src} NAME_WE)
    message(STATUS "Test found: ${test_bin_name}")
    add_executable(${test_bin_name} ${test_src})
    add_dependencies(test ${test_bin_name})
endforeach()