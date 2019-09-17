include(CTest)
enable_testing()

#Find all the examples
file(GLOB_RECURSE TESTS *.cpp)

foreach( test_src ${TESTS} )
    get_filename_component(test_bin_name ${test_src} NAME_WE)
    message(STATUS "Test found: ${test_bin_name}")
    add_executable(${test_bin_name} ${test_src})
    add_test(${test_bin_name} ${test_bin_name})
endforeach()