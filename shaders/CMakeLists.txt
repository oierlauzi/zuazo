#Based on https://gitlab.freedesktop.org/monado/monado/blob/e86320cd65db20aaed4b3b797e005f6001a2a5d6/cmake/SPIR-V.cmake

#Require glslangValidator
find_program(GLSLANGVALIDATOR_EXECUTABLE glslangValidator)
if(NOT GLSLANGVALIDATOR_EXECUTABLE)
	message(FATAL_ERROR "glslangValidator required")
endif()

#Find Zuazo's shader library
set(ZUAZO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
message(STATUS "Zuazo found at: ${ZUAZO_INCLUDE_DIR}")

#Get all the shaders on this path
file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.vert ${CMAKE_CURRENT_SOURCE_DIR}/*.frag)

foreach(SHADER_PATH ${SHADERS})
	get_filename_component(VAR_NAME ${SHADER_PATH} NAME )
	string(MAKE_C_IDENTIFIER ${VAR_NAME} VAR_NAME)

	set(HEADER_PATH "${CMAKE_CURRENT_BINARY_DIR}/${VAR_NAME}.h")

	add_custom_command(
		OUTPUT ${HEADER_PATH}
		COMMAND ${GLSLANGVALIDATOR_EXECUTABLE} -I${ZUAZO_INCLUDE_DIR}/zuazo/shaders/ -V ${SHADER_PATH} --vn ${VAR_NAME} -o ${HEADER_PATH}
		DEPENDS ${SHADER_PATH}
		COMMENT "Compiling ${SHADER_PATH}"
	)

	list(APPEND SHADER_HEADERS ${HEADER_PATH})
endforeach()

#Exported stuff
set(SHADER_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
add_custom_target(shaders
	DEPENDS ${SHADER_HEADERS}
)