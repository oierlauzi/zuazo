#pragma once

#include "../Utils/Resolution.h"
#include "../Utils/ScalingMode.h"
#include "DrawableBase.h"
#include "Geometry.h"
#include "GL/Shader.h"
#include "GL/Texture2D.h"
#include "GL/Buffer.h"
#include "GL/UniqueBinding.h"
#include "GL/VertexArray.h"

#include <array>
#include <memory>

namespace Zuazo::Graphics{

class Frame;

class FrameGeometry :
	public DrawableBase
{
public:
	typedef std::array<Utils::Vec<VectorComponent, 2>, 4> TextureCoordenates;

	FrameGeometry(const GL::Shader& shader, const std::string& vertAtrrib, const std::string& texAtrrib);
	FrameGeometry(const FrameGeometry& other)=delete;
	FrameGeometry(FrameGeometry&& other)=default;
	virtual ~FrameGeometry()=default;

	void 									setScalingMode(Utils::ScalingMode scaling=Utils::ScalingMode::Stretched);
	void									setGeometry(const Rectangle& rect);
	void									setFrame(std::shared_ptr<const Frame>& frame);

	virtual void							draw() const override;
private:
	Rectangle								m_rectangle;
	std::shared_ptr<const Frame>			m_frame;
	Utils::ScalingMode						m_scalingMode;

	const GL::Shader&						m_shader;
	const GLuint							m_vertAttribLoc;
	const GLuint							m_texAttribLoc;
	std::unique_ptr<GL::VertexArray>		m_vao;
	std::unique_ptr<GL::VertexArrayBuffer>	m_quadVbo;
	std::unique_ptr<GL::VertexArrayBuffer>	m_texCoordVbo;

	void									calculateTexCoords();
};
}
