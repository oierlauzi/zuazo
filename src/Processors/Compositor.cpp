#include <Processors/Compositor.h>

#include <Graphics/ImageAttributes.h>
#include <Graphics/MatrixOperations.h>
#include <Graphics/GL/UniqueBinding.h>
#include <Graphics/GL/Error.h>

#include <algorithm>

using namespace Zuazo::Processors;

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
	m_fov=getDefaultFov();
	m_cameraPos=getDefaultCameraPosition();

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

void Compositor::update() const{
	bool needsRender=false;
	for(auto layer : m_depthOrderedLayers){
		if(layer){
			if(layer->needsRender()){
				needsRender=true;
				break;
			}
		}
	}

	if(needsRender || m_forceRender){
		m_forceRender=false;

		std::stable_sort(m_depthOrderedLayers.begin(), m_depthOrderedLayers.end(), m_comp);

		m_drawtable->begin();

		//Attach the view and projection matrix to the shader
		m_viewMatrixUbo->bind();
		m_projectionMatrixUbo->bind();

		//Set the viewport for rendering
		glViewport(0, 0, m_videoMode.res.width, m_videoMode.res.height);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LEQUAL);

		//Draw all the layers
		for(auto ite=m_depthOrderedLayers.begin(); ite != m_depthOrderedLayers.end(); ++ite){
			(*ite)->draw();
		}

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		m_videoSourcePad.push(m_drawtable->finish());
	}
}

void Compositor::open(){
	if(!isOpen()){
		Graphics::ImageAttributes att(
				m_videoMode.res,
				Graphics::PixelFormat(m_videoMode.pixFmt)
		);

		m_drawtable=std::unique_ptr<Graphics::Drawtable>(new Graphics::Drawtable(att));
		m_drawtable->setDepthTest(true);

		m_viewMatrixUbo=std::unique_ptr<Graphics::ShaderUniform>(
				new Graphics::ShaderUniform(LayerBase::VIEW_MATRIX_INDEX, sizeof(Utils::Mat4x4f))
		);
		m_viewMatrixUbo->setData(&m_viewMatrix);

		m_projectionMatrixUbo=std::unique_ptr<Graphics::ShaderUniform>(
				new Graphics::ShaderUniform(LayerBase::PROJECTION_MATRIX_INDEX, sizeof(Utils::Mat4x4f))
		);
		m_projectionMatrixUbo->setData(&m_projectionMatrix);

		for(auto& layer : m_layers){
			layer->use();
		}

		m_forceRender=true;

		Video::TVideoSourceBase<Video::LazyVideoSourcePad>::enable();
		ZuazoBase::open();
	}
}

void Compositor::close(){
	if(isOpen()){
		Video::TVideoSourceBase<Video::LazyVideoSourcePad>::disable();

		for(auto& layer : m_layers){
			layer->unuse();
		}

		m_drawtable.reset();
		m_viewMatrixUbo.reset();
		m_projectionMatrixUbo.reset();

		m_drawtable.reset();
		ZuazoBase::close();
	}
}

void Compositor::setLayers(const std::vector<std::shared_ptr<LayerBase>>& layers){
	//Convert the shared pointers into raw pointers for eficiency
	std::vector<LayerBase*> layerPtrs;
	layerPtrs.reserve(layers.size());
	for(auto& layer : layers){
		layerPtrs.push_back(layer.get());
	}

	if(isOpen()){
		std::vector<LayerBase*> sortedLayers(std::move(layerPtrs));
		std::vector<LayerBase*> sortedOldLayers(std::move(m_depthOrderedLayers));

		//std::set_difference needs the elements ordered, sort them
		std::sort(sortedLayers.begin(), sortedLayers.end());
		std::sort(sortedOldLayers.begin(), sortedOldLayers.end());

		//Calculate added layers (layers - m_layers)
		std::vector<LayerBase*> addedLayers;
		std::set_difference(
			sortedLayers.begin(), sortedLayers.end(),
			sortedOldLayers.begin(), sortedOldLayers.end(),
			std::inserter(addedLayers, addedLayers.begin())
		);

		//Calculate deleted layers (m_layers - layers)
		std::vector<LayerBase*> deletedLayers;
		std::set_difference(
			sortedOldLayers.begin(), sortedOldLayers.end(),
			sortedLayers.begin(), sortedLayers.end(),
			std::inserter(deletedLayers, deletedLayers.begin())
		);

		for(auto& layer : addedLayers){
			layer->use();
		}

		for(auto& layer : deletedLayers){
			layer->unuse();
		}

		layerPtrs=std::move(sortedLayers); //Restore
	}

	m_layers=layers;
	m_depthOrderedLayers=std::move(layerPtrs); //Conversion already done for sorted layers, copy it

	m_forceRender=true;
}

void Compositor::resize(){
	if(m_drawtable){
		Graphics::ImageAttributes att(
				m_videoMode.res,
				Graphics::PixelFormat(m_videoMode.pixFmt)
		);

		m_drawtable->resize(att);
	}

	calculateProjectionMatrix();
}

void Compositor::calculateViewMatrix(){
	m_viewMatrix=Graphics::MatrixOperations::lookAt(m_cameraPos, m_cameraTarget, m_cameraUpDir);

	if(m_viewMatrixUbo){
		m_viewMatrixUbo->setData(&m_viewMatrix);
	}

	m_comp=LayerComp(m_projectionMatrix * m_viewMatrix);
	m_forceRender=true;
}

void Compositor::calculateProjectionMatrix(){
	if(m_videoMode.res)
		m_projectionMatrix=Graphics::MatrixOperations::perspective(m_videoMode.res, m_fov, m_nearClip, m_farClip);
	else
		m_projectionMatrix=Utils::Mat4x4f();

	//Flip vertically, as we are treating texture's data from top to bottom
	m_projectionMatrix=Graphics::MatrixOperations::scale(m_projectionMatrix, Utils::Vec3f(1, -1, 1));

	if(m_projectionMatrixUbo){
		m_projectionMatrixUbo->setData(&m_projectionMatrix);
	}

	m_comp=LayerComp(m_projectionMatrix * m_viewMatrix);
	m_forceRender=true;
}
