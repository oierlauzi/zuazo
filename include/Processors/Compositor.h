#pragma once

#include "../Graphics/Drawtable.h"
#include "../Graphics/FrameGeometry.h"
#include "../Graphics/ShaderUniform.h"
#include "../Graphics/Context.h"
#include "../Graphics/GL/Buffer.h"
#include "../Graphics/GL/Shader.h"
#include "../Video/VideoSourceBase.h"
#include "../Video/VideoStream.h"
#include "../Utils/Resolution.h"
#include "../Video/VideoStream.h"
#include "../Video/VideoConsumerBase.h"
#include "../Utils/Vector.h"
#include "../Utils/Matrix.h"
#include "../ZuazoBase.h"

#include <memory>
#include <vector>

namespace Zuazo::Processors{

class Compositor :
		public Video::TVideoSourceBase<Video::LazyVideoSourcePad>,
		public ZuazoBase
{
public:
	class LayerBase{
		friend Compositor;
	public:
		LayerBase();
		LayerBase(const LayerBase& other)=delete;
		LayerBase(LayerBase&& other)=default;
		virtual ~LayerBase()=default;

		const Utils::Vec3f&						getRotation() const;
		void									setRotation(const Utils::Vec3f& rotation);

		const Utils::Vec3f&						getScale() const;
		void									setScale(const Utils::Vec3f& scale);

		const Utils::Vec3f&						getPosition() const;
		void									setPosition(const Utils::Vec3f& pos);

		float									getOpacity() const;
		void									setOpacity(float alpha);

		void									setRSP(const Utils::Vec3f& rotation, const Utils::Vec3f& scale, const Utils::Vec3f& position);
	protected:
		virtual void							open()=0;
		virtual void							close()=0;

		virtual void							draw() const=0;
		virtual bool							hasChanged() const;
		void									setup() const;

		mutable std::mutex						m_mutex;
		bool									m_forceChange;
	private:
		Utils::Vec3f							m_rotation;
		Utils::Vec3f							m_scale;
		Utils::Vec3f							m_position;
		float									m_opacity;

		Utils::Mat4x4f							m_modelMatrix;
		std::unique_ptr<Graphics::ShaderUniform<Utils::Mat4x4f>> 	m_modelMatrixUbo;
		std::unique_ptr<Graphics::ShaderUniform<float>> 			m_opacityUbo;

		void									calculateModelMatrix();
	};

	class VideoLayer :
			public LayerBase,
			public Video::TVideoConsumerBase<Video::VideoConsumerPad>
	{
	public:
		VideoLayer();
		VideoLayer(const Graphics::Rectangle& rect);
		VideoLayer(const VideoLayer& other)=delete;
		VideoLayer(VideoLayer&& other)=default;
		virtual ~VideoLayer()=default;
	private:
		Graphics::Rectangle						m_rectangle;

		std::unique_ptr<Graphics::FrameGeometry> m_frameGeom;

		const Graphics::Rectangle& 				getRect() const;
		void									setRect(const Graphics::Rectangle& rect);

		SUPPORTS_GETTING_SCALINGMODE
		SUPPORTS_SETTING_SCALINGMODE
		virtual void							setScalingMode(Utils::ScalingMode scaling) override;

		virtual void							open();
		virtual void							close();

		virtual void							draw() const override;
		virtual bool							hasChanged() const override;

		static const std::string				s_vertexShaderSrc;
		static const std::string				s_fragmentShaderSrc;
	};

	Compositor();
	Compositor(const Utils::VideoMode& videoMode);
	Compositor(const Compositor& other)=delete;
	Compositor(Compositor&& other)=default;
	virtual ~Compositor();

	SUPPORTS_GETTING_PIXELFORMAT
	SUPPORTS_SETTING_PIXELFORMAT
	SUPPORTS_LISTING_PIXELFORMATS
	virtual std::set<Utils::PixelFormat>	getSupportedPixelFormats() const override;
	virtual void							setPixelFormat(const Utils::PixelFormat& pixFmt) override;

	SUPPORTS_GETTING_RESOLUTION
	SUPPORTS_SETTING_RESOLUTION
	SUPPORTS_ANY_RESOLUTION
	virtual void							setResolution(const Utils::Resolution& res) override;

	SUPPORTS_GETTING_VIDEOMODE
	SUPPORTS_SETTING_VIDEOMODE
	virtual void							setVideoMode(const Utils::VideoMode& videoMode) override;

	const Utils::Vec3f&						getCameraPosition() const;
	const Utils::Vec3f&						getCameraTarget() const;
	const Utils::Vec3f&						getCameraUpDirection() const;
	Utils::Vec3f							getCameraTargetForAngle(const Utils::Vec3f& angles) const;
	Utils::Vec3f							getDefaultCameraPosition() const;
	Utils::Vec3f							getDefaultCameraTarget() const;
	Utils::Vec3f							getDefaultCameraUpDirection() const;
	void									setCameraPosition(const Utils::Vec3f& pos);
	void									setCameraTarget(const Utils::Vec3f& tgt);
	void									setCameraUpDirection(const Utils::Vec3f& up);
	void									setCamera(const Utils::Vec3f& pos, const Utils::Vec3f& tgt, const Utils::Vec3f& up=Utils::Vec3f(0,1,0));
	void									setDefaultCamera();

	float									getFov() const;
	float									getDefaultFov() const;
	void									setFov(float fov);
	void									setDefaultFov();

	float									getNearClip() const;
	float									getFarClip() const;
	float									getDefaultNearClip() const;
	float									getDefaultFarClip() const;
	void									setNearClip(float near);
	void									setFarClip(float far);
	void									setClipping(float near, float far);
	void									setDefaultClipping();

	template<typename T = LayerBase>
	const std::unique_ptr<T>&				getLayer(u_int32_t idx) const;
	u_int32_t								getLayerCount() const;
	template<typename T = LayerBase>
	void									addLayer(std::unique_ptr<T> layer);
	void									deleteLayer(u_int32_t idx);
	void									swapLayers(u_int32_t idx1, u_int32_t idx2);
	bool									isValidIndex(u_int32_t idx) const;


	virtual void							update() const override;

	virtual void							open() override;
	virtual void							close() override;
private:
	using UpdateableBase::m_updateMutex;

	std::unique_ptr<Graphics::Drawtable>	m_drawtable;
	std::vector<std::unique_ptr<LayerBase>>	m_layers;

	float									m_nearClip;
	float									m_farClip;
	float									m_fov;
	Utils::Vec3f							m_cameraPos;
	Utils::Vec3f							m_cameraTarget;
	Utils::Vec3f							m_cameraUpDir;

	Utils::Mat4x4f							m_viewMatrix;
	std::unique_ptr<Graphics::ShaderUniform<Utils::Mat4x4f>> m_viewMatrixUbo;
	Utils::Mat4x4f							m_projectionMatrix;
	std::unique_ptr<Graphics::ShaderUniform<Utils::Mat4x4f>> m_projectionMatrixUbo;

	void 									resize();
	void									calculateViewMatrix();
	void									calculateProjectionMatrix();
};



inline const Utils::Vec3f&	Compositor::getCameraPosition() const{
	return m_cameraPos;
}

inline const Utils::Vec3f&	Compositor::getCameraTarget() const{
	return m_cameraTarget;
}

inline const Utils::Vec3f&	Compositor::getCameraUpDirection() const{
	return m_cameraUpDir;
}

inline Utils::Vec3f	Compositor::getDefaultCameraPosition() const{
	return Utils::Vec3f(0, 0, m_farClip - m_nearClip);
}

inline Utils::Vec3f	Compositor::getDefaultCameraTarget() const{
	return Utils::Vec3f(0, 0, 0);
}

inline Utils::Vec3f	Compositor::getDefaultCameraUpDirection() const{
	return Utils::Vec3f(0, 1, 0);
}

inline void Compositor::setCameraPosition(const Utils::Vec3f& pos){
	m_cameraPos=pos;
	calculateViewMatrix();
}

inline void Compositor::setCameraTarget(const Utils::Vec3f& tgt){
	m_cameraTarget=tgt;
	calculateViewMatrix();
}

inline void Compositor::setCameraUpDirection(const Utils::Vec3f& up){
	m_cameraUpDir=up;
	calculateViewMatrix();
}

inline void Compositor::setCamera(const Utils::Vec3f& pos, const Utils::Vec3f& tgt, const Utils::Vec3f& up){
	m_cameraPos=pos;
	m_cameraTarget=tgt;
	m_cameraUpDir=up;
	calculateViewMatrix();
}

inline void	Compositor::setDefaultCamera(){
	setCamera(
			getDefaultCameraPosition(),
			getDefaultCameraTarget(),
			getDefaultCameraUpDirection()
	);
}


inline float Compositor::getFov() const{
	return m_fov;
}

inline void Compositor::setFov(float fov){
	m_fov=fov;
	calculateProjectionMatrix();
}

inline void Compositor::setDefaultFov(){
	setFov(
			getDefaultFov()
	);
}

inline float Compositor::getNearClip() const{
	return m_nearClip;
}

inline float Compositor::getFarClip() const{
	return m_farClip;
}

inline float Compositor::getDefaultNearClip() const{
	return std::max(m_videoMode.res.width, m_videoMode.res.height);
}

inline float Compositor::getDefaultFarClip() const{
	return std::max(m_videoMode.res.width, m_videoMode.res.height) * 3;
}

inline void Compositor::setNearClip(float near){
	m_nearClip=near;
	calculateProjectionMatrix();
}

inline void Compositor::setFarClip(float far){
	m_farClip=far;
	calculateProjectionMatrix();
}

inline void Compositor::setClipping(float near, float far){
	m_nearClip=near;
	m_farClip=far;
	calculateProjectionMatrix();
}

inline void Compositor::setDefaultClipping(){
	setClipping(
			getDefaultNearClip(),
			getDefaultFarClip()
	);
}

template<typename T>
inline const std::unique_ptr<T>& Compositor::getLayer(u_int32_t idx) const{
	if(isValidIndex(idx)){
		return dynamic_cast<T>(m_layers[idx]);
	}else
		return std::unique_ptr<T>();
}

inline u_int32_t Compositor::getLayerCount() const{
	return m_layers.size();
}

template<typename T>
inline void	Compositor::addLayer(std::unique_ptr<T> layer){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	m_layers.push_back(std::move(layer));
	if(isOpen() && m_layers.back()){
		Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());
		m_layers.back()->open();
	}
}

inline void	Compositor::deleteLayer(u_int32_t idx){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	if(isValidIndex(idx)){
		if(m_layers[idx] && isOpen()){
			Graphics::UniqueContext ctx(Graphics::Context::getMainCtx());
			m_layers[idx]->close();
		}
		m_layers.erase(m_layers.begin() + idx);
	}
}

inline void	Compositor::swapLayers(u_int32_t idx1, u_int32_t idx2){
	std::lock_guard<std::mutex> lock(m_updateMutex);
	if(isValidIndex(idx1) && isValidIndex(idx2)){
		//TODO
	}
}

inline bool	Compositor::isValidIndex(u_int32_t idx) const{
	return idx < m_layers.size();
}





inline const Utils::Vec3f& Compositor::LayerBase::getRotation() const{
	return m_rotation;
}

inline void	Compositor::LayerBase::setRotation(const Utils::Vec3f& rotation){
	m_rotation=rotation;
	calculateModelMatrix();
}

inline const Utils::Vec3f& Compositor::LayerBase::getScale() const{
	return m_scale;
}

inline void	Compositor::LayerBase::setScale(const Utils::Vec3f& scale){
	m_scale=scale;
	calculateModelMatrix();
}

inline const Utils::Vec3f& Compositor::LayerBase::getPosition() const{
	return m_position;
}

inline void	Compositor::LayerBase::setPosition(const Utils::Vec3f& pos){
	m_position=pos;
	calculateModelMatrix();
}

inline float Compositor::LayerBase::getOpacity() const{
	return m_opacity;
}

inline void Compositor::LayerBase::setOpacity(float alpha){
	std::lock_guard<std::mutex> lock(m_mutex);
	m_opacity=alpha;
	Graphics::UniqueContext ctx;
	m_opacityUbo->setData(m_opacity);
}

inline void  Compositor::LayerBase::setRSP(const Utils::Vec3f& rotation, const Utils::Vec3f& scale, const Utils::Vec3f& position){
	m_rotation=rotation;
	m_scale=scale;
	m_position=position;
	calculateModelMatrix();
}
}
