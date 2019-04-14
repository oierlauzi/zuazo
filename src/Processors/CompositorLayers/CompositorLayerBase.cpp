#include <Processors/Compositor.h>

#include <Graphics/ImageAttributes.h>
#include <Graphics/MatrixOperations.h>
#include <Graphics/Context.h>
#include <Graphics/GL/UniqueBinding.h>

using namespace Zuazo::Processors;

Compositor::LayerBase::LayerBase() :
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


void Compositor::LayerBase::setup() const{
	m_modelMatrixUbo->bind();
	m_opacityUbo->bind();
}

bool Compositor::LayerBase::hasChanged() const{
	return m_forceRender;
}

void Compositor::LayerBase::calculateModelMatrix(){
	std::lock_guard<std::mutex> lock(m_mutex);
	//m_modelMatrix=Graphics::MatrixOperations::eulerAngle(m_rotation);
	//m_modelMatrix=Graphics::MatrixOperations::scale(m_modelMatrix, m_scale);
	//m_modelMatrix=Graphics::MatrixOperations::translate(m_modelMatrix, m_position);

	const Utils::Mat4x4f rotationMatrix(Graphics::MatrixOperations::eulerAngle(m_rotation));
	const Utils::Mat4x4f scaleMatrix(Graphics::MatrixOperations::scale(m_scale));
	const Utils::Mat4x4f translationMatrix(Graphics::MatrixOperations::translate(m_position));
	m_modelMatrix=translationMatrix * scaleMatrix * rotationMatrix;

	if(m_modelMatrixUbo){
		Graphics::UniqueContext ctx;
		m_modelMatrixUbo->setData(&m_modelMatrix);
		m_forceRender=true;
	}
}