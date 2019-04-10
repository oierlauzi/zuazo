#include <Processors/Compositor.h>

#include <Graphics/ImageAttributes.h>
#include <Graphics/MatrixOperations.h>
#include <Graphics/ShaderPool.h>
#include <Graphics/Context.h>
#include <Graphics/GL/UniqueBinding.h>
#include <Graphics/GL/Error.h>

using namespace Zuazo::Processors;

const u_int32_t MODEL_MATRIX_INDEX			=0;
const u_int32_t VIEW_MATRIX_INDEX 			=1;
const u_int32_t PROJECTION_MATRIX_INDEX 	=2;
const u_int32_t OPACITY_INDEX 				=3;

/*
 * COMPOSITOR
 */

Compositor::Compositor() :
		m_nearClip(100.0),
		m_farClip(10000.0),
		m_fov(M_PI / 4),
		m_cameraPos(Utils::Vec3f(0, 0, (m_farClip - m_nearClip) / 2)),
		m_cameraTarget(Utils::Vec3f(0, 0, 0)),
		m_cameraUpDir(Utils::Vec3f(0, 1, 0))
{
	calculateViewMatrix();
	calculateProjectionMatrix();

	open();
}

Compositor::Compositor(const Utils::VideoMode& videoMode) :
		m_cameraTarget(0, 0, 0),
		m_cameraUpDir(0, 1, 0)
{
	m_videoMode=videoMode;
	m_nearClip=getDefaultNearClip();
	m_farClip=getDefaultFarClip();
	m_cameraPos=getDefaultCameraPosition();
	m_fov=getDefaultFov();

	calculateViewMatrix();
	calculateProjectionMatrix();

	open();
}

Compositor::~Compositor(){
	close();
}

std::set<Zuazo::Utils::PixelFormat>	Compositor::getSupportedPixelFormats() const{
	std::set<Zuazo::Utils::PixelFormat> pixFmts;
	for(u_int32_t i=0; Graphics::compatiblePixelFormats[i]; i++){
		pixFmts.insert(Graphics::compatiblePixelFormats[i]);
	}

	return pixFmts;
}

void Compositor::setPixelFormat(const Utils::PixelFormat& pixFmt){
	m_videoMode.pixFmt=pixFmt;
	resize();
}

void Compositor::setResolution(const Utils::Resolution& res){
	m_videoMode.res=res;
	resize();
}

void Compositor::setVideoMode(const Utils::VideoMode& videoMode){
	m_videoMode=videoMode;
	resize();
}

Zuazo::Utils::Vec3f Compositor::getCameraTargetForAngle(const Utils::Vec3f& angles) const{
	Utils::Vec4f forwardDir(0, 0, -1, 0); //By default camera looks at -z direction
	Utils::Mat4x4f rotationMatrix(Graphics::MatrixOperations::eulerAngle(angles));
	Utils::Vec4f direction4=rotationMatrix * forwardDir;
	Utils::Vec3f direction(direction4.x, direction4.y, direction4.z);
	return Utils::Vec3f(m_cameraPos + direction);
}

float Compositor::getDefaultFov() const{
	double height=m_videoMode.res.height;
	double distance=glm::distance(m_cameraTarget, m_cameraPos);
	return 2 * atan(height / 2 / distance);
}

void Compositor::update() const{
	bool hasChanged=false;
	for(auto& layer : m_layers){
		if(layer){
			if(layer->hasChanged()){
				hasChanged=true;
				break;
			}
		}
	}

	if(hasChanged || m_forceRender){
		m_forceRender=false;

		m_drawtable->begin();

		//Attach the view and projection matrix to the shader
		m_viewMatrixUbo->bind();
		m_projectionMatrixUbo->bind();

		//Set the viewport for rendering
		glViewport(0, 0, m_videoMode.res.width, m_videoMode.res.height);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDepthFunc(GL_LESS);

		for(auto& layer : m_layers){
			layer->draw();
		}

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		m_videoSourcePad.push(m_drawtable->finish());
	}
}

void Compositor::open(){
	Graphics::ImageAttributes att(
			m_videoMode.res,
			Graphics::PixelFormat(m_videoMode.pixFmt)
	);

	Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());
	m_drawtable=std::unique_ptr<Graphics::Drawtable>(new Graphics::Drawtable(att));

	m_viewMatrixUbo=std::unique_ptr<Graphics::ShaderUniform<Utils::Mat4x4f>>(
			new Graphics::ShaderUniform<Utils::Mat4x4f>(VIEW_MATRIX_INDEX)
	);
	m_viewMatrixUbo->setData(m_viewMatrix);

	m_projectionMatrixUbo=std::unique_ptr<Graphics::ShaderUniform<Utils::Mat4x4f>>(
			new Graphics::ShaderUniform<Utils::Mat4x4f>(PROJECTION_MATRIX_INDEX)
	);
	m_projectionMatrixUbo->setData(m_projectionMatrix);

	for(auto& layer : m_layers){
		layer->open();
	}

	m_forceRender=true;

	Video::TVideoSourceBase<Video::LazyVideoSourcePad>::enable();
	ZuazoBase::open();
}

void Compositor::close(){
	Video::TVideoSourceBase<Video::LazyVideoSourcePad>::disable();

	Graphics::UniqueContext ctx;

	for(auto& layer : m_layers){
		layer->close();
	}

	m_drawtable.reset();
	m_viewMatrixUbo.reset();
	m_projectionMatrixUbo.reset();

	m_drawtable.reset();
	ZuazoBase::close();
}

void Compositor::resize(){
	if(m_drawtable){
		Graphics::ImageAttributes att(
				m_videoMode.res,
				Graphics::PixelFormat(m_videoMode.pixFmt)
		);

		Graphics::UniqueContext ctx;
		m_drawtable->resize(att);
	}

	calculateProjectionMatrix();
}

void Compositor::calculateViewMatrix(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_viewMatrix=Graphics::MatrixOperations::lookAt(m_cameraPos, m_cameraTarget, m_cameraUpDir);

	if(m_viewMatrixUbo){
		Graphics::UniqueContext ctx;
		m_viewMatrixUbo->setData(m_viewMatrix);
	}

	m_forceRender=true;
}

void Compositor::calculateProjectionMatrix(){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	if(m_videoMode.res)
		m_projectionMatrix=Graphics::MatrixOperations::perspective(m_videoMode.res, m_fov, m_nearClip, m_farClip);
	else
		m_projectionMatrix=Utils::Mat4x4f();

	//Flip vertically, as we are treating texture's data from top to bottom
	m_projectionMatrix=Graphics::MatrixOperations::scale(m_projectionMatrix, Utils::Vec3f(1, -1, 1));

	if(m_projectionMatrixUbo){
		Graphics::UniqueContext ctx;
		m_projectionMatrixUbo->setData(m_projectionMatrix);
	}

	m_forceRender=true;
}

/*
 * LAYER BASE
 */

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
	m_modelMatrixUbo=std::unique_ptr<Graphics::ShaderUniform<Utils::Mat4x4f>> (new Graphics::ShaderUniform<Utils::Mat4x4f>(MODEL_MATRIX_INDEX));
	m_opacityUbo=std::unique_ptr<Graphics::ShaderUniform<float>> (new Graphics::ShaderUniform<float>(OPACITY_INDEX));
	m_modelMatrixUbo->setData(m_modelMatrix);
	m_opacityUbo->setData(m_opacity);
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
		m_modelMatrixUbo->setData(m_modelMatrix);
		m_forceRender=true;
	}
}


/*
 * VIDEO LAYER
 */

Compositor::VideoLayer::VideoLayer(const Graphics::Rectangle& rect) :
		m_rectangle(rect)
{
}

const Zuazo::Graphics::Rectangle& Compositor::VideoLayer::getRect() const{
	return m_rectangle;
}

void Compositor::VideoLayer::setRect(const Graphics::Rectangle& rect){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_rectangle=rect;
	if(m_frameGeom){
		Graphics::UniqueContext ctx;
		m_frameGeom->setGeometry(rect);
		m_forceRender=true;
	}
}

void Compositor::VideoLayer::setScalingMode(Utils::ScalingMode scaling){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_scalingMode=scaling;
	if(m_frameGeom){
		Graphics::UniqueContext ctx;
		m_frameGeom->setScalingMode(m_scalingMode);
		m_forceRender=true;
	}
}

void Compositor::VideoLayer::draw() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	setup();

	std::shared_ptr<const Graphics::Frame> fr=m_videoConsumerPad.get();

	if(fr){
		m_frameGeom->setFrame(fr);
		m_frameGeom->draw();
	}
}

bool Compositor::VideoLayer::hasChanged() const{
	return m_videoConsumerPad.hasChanged() || m_forceRender;
}

void Compositor::VideoLayer::open(){
	m_shader=std::unique_ptr<Graphics::SharedObject<Shader>>(new Graphics::SharedObject<Shader>);
	m_frameGeom=std::unique_ptr<Graphics::FrameGeometry>(new Graphics::FrameGeometry(m_shader->get(), "in_vertex", "in_uv"));

	m_frameGeom->setGeometry(m_rectangle);
	m_frameGeom->setScalingMode(m_scalingMode);

	LayerBase::open();
}

void Compositor::VideoLayer::close(){
	m_frameGeom.reset();
	LayerBase::close();
}

Compositor::VideoLayer::Shader::Shader() :
	Graphics::GL::Shader(
		std::string(
			#include "../../data/shaders/video_layer.vert"
		),
		std::string(
			#include "../../data/shaders/video_layer.frag"
		)
	)
{
	setUniformBlockBinding("model", MODEL_MATRIX_INDEX);
	setUniformBlockBinding("view", VIEW_MATRIX_INDEX);
	setUniformBlockBinding("projection", PROJECTION_MATRIX_INDEX);
	setUniformBlockBinding("data", OPACITY_INDEX);
}
