#include <Processors/Compositor.h>

#include <Graphics/ImageAttributes.h>
#include <Graphics/MatrixOperations.h>
#include <Graphics/GL/UniqueBinding.h>

using namespace Zuazo::Processors;

Compositor::LayerBase::LayerBase() :
	m_useCount(0),
	m_anchorage(Utils::Vec3f(0, 0, 0)),
	m_rotation(Utils::Vec3f(0, 0, 0)),
	m_scale(Utils::Vec3f(1, 1, 1)),
	m_position(Utils::Vec3f(0, 0, 0)),
	m_opacity(1.0),
	m_forceRender(true)
{
	calculateModelMatrix();
}

void Compositor::LayerBase::open(){
	m_modelMatrixUbo=std::unique_ptr<Graphics::ShaderUniform> (
			new Graphics::ShaderUniform(MODEL_MATRIX_INDEX, sizeof(Utils::Mat4x4f))
	);
	m_modelMatrixUbo->setData(&m_modelMatrix);
	m_opacityUbo=std::unique_ptr<Graphics::ShaderUniform> (
			new Graphics::ShaderUniform(OPACITY_INDEX, sizeof(float))
	);
	m_opacityUbo->setData(&m_opacity);
	m_forceRender=true;
}

void Compositor::LayerBase::close(){
	m_modelMatrixUbo.reset();
	m_opacityUbo.reset();
	m_forceRender=true;
}

Zuazo::Utils::Vec3f Compositor::LayerBase::getAvgPosition() const{
	return getPosition();
}

void Compositor::LayerBase::setup() const{
	m_modelMatrixUbo->bind();
	m_opacityUbo->bind();
	m_forceRender=false;
}

Zuazo::Utils::Vec3f Compositor::LayerBase::project(const Utils::Vec3f& vec) const{
	Utils::Vec4f projection=m_modelMatrix * Utils::Vec4f(vec, 1.0);
	return Utils::Vec3f(projection.x, projection.y, projection.z);
}

void Compositor::LayerBase::calculateModelMatrix(){
	const Utils::Mat4x4f anchorageMatrix(Graphics::MatrixOperations::translate(-m_anchorage));
	const Utils::Mat4x4f rotationMatrix(Graphics::MatrixOperations::eulerAngle(m_rotation));
	const Utils::Mat4x4f scaleMatrix(Graphics::MatrixOperations::scale(m_scale));
	const Utils::Mat4x4f translationMatrix(Graphics::MatrixOperations::translate(m_position));
	m_modelMatrix=translationMatrix * scaleMatrix * rotationMatrix * anchorageMatrix;

	if(m_modelMatrixUbo){
		m_modelMatrixUbo->setData(&m_modelMatrix);
		m_forceRender=true;
	}
}
