#include <Processors/Compositor.h>

#include <Graphics/ImageAttributes.h>
#include <Graphics/MatrixOperations.h>
#include <Graphics/GL/UniqueBinding.h>

using namespace Zuazo::Processors;

std::weak_ptr<Zuazo::Graphics::GL::Program>	Compositor::VideoLayer::s_shader;

Compositor::VideoLayer::VideoLayer(const Graphics::Rectangle& rect) :
		m_rectangle(rect)
{
}

const Zuazo::Graphics::Rectangle& Compositor::VideoLayer::getRect() const{
	return m_rectangle;
}

void Compositor::VideoLayer::setRect(const Graphics::Rectangle& rect){
	m_rectangle=rect;
	if(m_frameGeom){
		m_frameGeom->setGeometry(rect);
		m_forceRender=true;
	}
}

void Compositor::VideoLayer::setScalingMode(Utils::ScalingMode scaling){
	m_scalingMode=scaling;
	if(m_frameGeom){
		m_frameGeom->setScalingMode(m_scalingMode);
		m_forceRender=true;
	}
}

Zuazo::Utils::Vec3f Compositor::VideoLayer::getAvgPosition() const{
	return getPosition() - m_rectangle.center;
}

void Compositor::VideoLayer::draw() const{
	setup();

	std::shared_ptr<const Graphics::Frame> frame=m_videoConsumerPad.get();

	if(frame){
		m_frameGeom->setFrame(frame);
		m_frameGeom->draw();
	}
}

bool Compositor::VideoLayer::needsRender() const{
	return m_videoConsumerPad.hasChanged() || m_forceRender;
}

void Compositor::VideoLayer::open(){
	if(s_shader.expired()){
		//There is no available shader, create it
		m_shader=std::shared_ptr<Graphics::GL::Program>(new Graphics::GL::Program(
			std::string(
				#include "../../../data/shaders/video_layer.vert"
			),
			std::string(
				#include "../../../data/shaders/video_layer.frag"
			)
		));

		//Assign the block bindings
		m_shader->setUniformBlockBinding("model", MODEL_MATRIX_INDEX);
		m_shader->setUniformBlockBinding("view", VIEW_MATRIX_INDEX);
		m_shader->setUniformBlockBinding("projection", PROJECTION_MATRIX_INDEX);
		m_shader->setUniformBlockBinding("data", OPACITY_INDEX);

		//Save the shader for a future use by others
		s_shader=m_shader;
	}else{
		//Get the available shader
		m_shader=s_shader.lock();
	}
	m_frameGeom=std::unique_ptr<Graphics::FrameGeometry>(new Graphics::FrameGeometry(*m_shader, "in_vertex", "in_uv"));

	m_frameGeom->setGeometry(m_rectangle);
	m_frameGeom->setScalingMode(m_scalingMode);

	LayerBase::open();
}

void Compositor::VideoLayer::close(){
	m_frameGeom.reset();
	m_shader.reset();
	LayerBase::close();
}
