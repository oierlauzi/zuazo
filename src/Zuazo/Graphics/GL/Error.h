#pragma once

#include <glad/glad.h>
#include <map>
#include <string>

namespace Zuazo::Graphics::GL{

inline std::string getError(){
	const std::map<GLuint, std::string> errors={
			{GL_NO_ERROR, 						"NO ERROR"},
			{GL_INVALID_ENUM, 					"INVALID ENUM"},
			{GL_INVALID_VALUE, 					"INVALID VALUE"},
			{GL_INVALID_OPERATION, 				"INVALID OPERATION"},
			{GL_OUT_OF_MEMORY, 					"OUT OF MEMORY"},
			{GL_INVALID_FRAMEBUFFER_OPERATION, 	"INVALID FRAMEBUFFER OPERATION"}
	};

	return errors.at(glGetError());
}

}
