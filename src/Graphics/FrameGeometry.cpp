#include <Graphics/FrameGeometry.h>

#include <Graphics/Frame.h>
#include <Graphics/Context.h>
#include <Graphics/GL/UniqueBinding.h>

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstring>

using namespace Zuazo::Graphics;

FrameGeometry::FrameGeometry(const GL::Program& shader, const std::string& vertAtrrib, const std::string& texAtrrib) :
	m_scalingMode(Utils::ScalingMode::Stretched),
	m_frame(nullptr),
	m_shader(shader),
	m_vertAttribLoc(m_shader.getAttributeLoc(vertAtrrib)),
	m_texAttribLoc(m_shader.getAttributeLoc(texAtrrib))
{
	m_vertexArray=std::unique_ptr<VertexArray>(new VertexArray);
	m_vertexArray->enableAttribute<VectorComponent, 3>(m_vertAttribLoc, 4);
	m_vertexArray->enableAttribute<float, 2>(m_texAttribLoc, 4);
}

void FrameGeometry::setScalingMode(Utils::ScalingMode scaling){
	if(m_scalingMode != scaling){
		m_scalingMode=scaling;
		calculateTexCoords();
	}
}

void FrameGeometry::setGeometry(const Graphics::Rectangle& rect){
	if(m_rectangle != rect){
		m_rectangle=rect;

		Quad quad(m_rectangle);
		m_vertexArray->uploadVertices<VectorComponent, 3>(m_vertAttribLoc, quad.toVertexBuffer());

		calculateTexCoords();
	}
}

void FrameGeometry::setFrame(std::shared_ptr<const Frame>& frame){
	const Utils::Resolution& newRes=frame ? frame->getAttributes().res : Utils::Resolution(0, 0);
	const Utils::Resolution& oldRes=m_frame ? m_frame->getAttributes().res : Utils::Resolution(0, 0);
	m_frame=frame;
	if(newRes != oldRes){
		calculateTexCoords();
	}
}

void FrameGeometry::draw() const{
	if(m_frame){
		Graphics::GL::UniqueBinding<GL::Program> shaderBind(m_shader);
		Graphics::GL::UniqueBinding<VertexArray> vaoBind(*m_vertexArray);
		Graphics::GL::UniqueBinding<GL::Texture2D> texBind(m_frame->getTexture());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void FrameGeometry::calculateTexCoords(){
	if(m_frame){
		//Update the texture coordinates
		float width=m_frame->getAttributes().res.width;
		float height=m_frame->getAttributes().res.height;
		float sX=m_rectangle.size.x / width;
		float sY=m_rectangle.size.y / height;
		float s;

		//Evaluate drawing area's dimensions
		switch(m_scalingMode){
		case Utils::ScalingMode::Boxed:
			//Scale with the minimum factor
			s=std::min(sX, sY);
			sX=s;
			sY=s;
			break;
		case Utils::ScalingMode::Cropped:
			//Scale with the maximum factor
			s=std::max(sX, sY);
			sX=s;
			sY=s;
			break;
		case Utils::ScalingMode::Stretched:
			//Scale independently.
			//Neither sX nor sY needs to be modified
			break;
		case Utils::ScalingMode::ClampVert:
			//Scale according to sY
			sX=sY;
			break;
		case Utils::ScalingMode::ClampHor:
			//Scale according to sX
			sY=sX;
			break;
		default:
			//This should not happen
			//Don't show anything
			sX=0;
			sY=0;
			break;
		}

		float diffX = m_rectangle.size.x / (2 * width * sX);
		float diffY = m_rectangle.size.y / (2 * height * sY);

		Zuazo::Utils::Vec2f texCoords[4]={  //Invert Y coordinates
				Zuazo::Utils::Vec2f(0.5f - diffX, 	0.5f + diffY),
				Zuazo::Utils::Vec2f(0.5f - diffX, 	0.5f - diffY),
				Zuazo::Utils::Vec2f(0.5f + diffX, 	0.5f + diffY),
				Zuazo::Utils::Vec2f(0.5f + diffX, 	0.5f - diffY),
		};

		m_vertexArray->uploadVertices<float, 2>(m_texAttribLoc, texCoords);
	}
}
