#pragma once

#include "GL/Shader.h"
#include "GL/Buffer.h"
#include "../Utils/Vector.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <array>

namespace Zuazo::Graphics{


class VertexArray{
public:
	VertexArray();
	VertexArray(const VertexArray& other)=delete;
	VertexArray(VertexArray&& other);
	virtual ~VertexArray();

	void					enableAttribute(GLuint attribute);
	void					disableAttribute(GLuint attribute);
	void					disbleAllAttributes();

	void					uploadVertices(GLuint attribute, const void* data, size_t size);
	template<int dim, typename Q>
	void					uploadVertices(GLuint attribute, const std::vector<Utils::Vec<dim, Q>>& vector);
	template<size_t lenght, int dim, typename Q>
	void					uploadVertices(GLuint attribute, const std::array<Utils::Vec<dim, Q>, lenght>& vector);

	void					bind() const;
	void					unbind() const;
private:
	struct VBO{
		GL::VertexArrayBuffer	glVbo;
		size_t					allocatedSize=0;
	};

	std::map<GLuint, std::unique_ptr<VBO>> m_vbos;
};


inline void VertexArray::enableAttribute(GLuint attribute){

}

inline void VertexArray::disableAttribute(GLuint attribute){

}

inline void VertexArray::disbleAllAttributes(){

}

inline void	VertexArray::uploadVertices(GLuint attribute, const void* data, size_t size){

}

template<int dim, typename Q>
inline void	VertexArray::uploadVertices(GLuint attribute, const std::vector<Utils::Vec<dim, Q>>& vector){

}

template<size_t lenght, int dim, typename Q>
inline void	VertexArray::uploadVertices(GLuint attribute, const std::array<Utils::Vec<dim, Q>, lenght>& vector){

}

inline void	VertexArray::bind() const{

}

inline void	VertexArray::unbind() const{

}


}
