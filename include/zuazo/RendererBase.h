#pragma once

#include "DepthStencilFormat.h"
#include "Graphics/Vulkan.h"
#include "Graphics/CommandBuffer.h"
#include "Graphics/RenderPass.h"
#include "Math/Transform.h"
#include "Utils/Area.h"
#include "Utils/Pimpl.h"
#include "Utils/Limit.h"

#include <functional>
#include <array>
#include <utility>

namespace Zuazo {

class LayerBase;

class RendererBase {
public:
	class Camera;
	using LayerRef = std::reference_wrapper<const LayerBase>;

	using DepthStencilFormatCallback = std::function<void(RendererBase&, const Utils::Limit<DepthStencilFormat>&)>;
	using CameraCallback = std::function<void(RendererBase&, const Camera& camera)>;
	using RenderPassQueryCallback = std::function<Graphics::RenderPass(const RendererBase&)>;

	enum DescriptorBindings {
		DESCRIPTOR_BINDING_PROJECTION_MATRIX,
		DESCRIPTOR_BINDING_OUTPUT_COLOR_TRANSFER,

		DESCRIPTOR_COUNT
	};

	using UniformBufferLayout = std::array<Utils::Area, DESCRIPTOR_COUNT>;


	RendererBase(	Utils::Limit<DepthStencilFormat> depthStencil = {},
					DepthStencilFormatCallback internalDepthStencilCbk = {},
					CameraCallback cameraCbk = {},
					RenderPassQueryCallback renderPassQueryCbk = {} );
	RendererBase(const RendererBase& other) = delete;
	RendererBase(RendererBase&& other);
	virtual ~RendererBase();

	RendererBase& 							operator=(const RendererBase& other) = delete;
	RendererBase&							operator=(RendererBase&& other);

	void									setDepthStencilFormatCompatibilityCallback(DepthStencilFormatCallback cbk);
	const DepthStencilFormatCallback&		getDepthStencilFormatCompatibilityCallback() const;

	void									setDepthStencilFormatCallback(DepthStencilFormatCallback cbk);
	const DepthStencilFormatCallback&		getDepthStencilFormatCallback() const;

	void									setDepthStencilFormatLimits(Utils::Limit<DepthStencilFormat> lim);
	const Utils::Limit<DepthStencilFormat>&	getDepthStencilFormatLimits() const;
	const Utils::Limit<DepthStencilFormat>& getDepthStencilFormatCompatibility() const;
	const Utils::Limit<DepthStencilFormat>&	getDepthStencilFormat() const;

	void									setCamera(const Camera& cam);
	const Camera&							getCamera() const;

	void									setLayers(Utils::BufferView<const LayerRef> layers);
	Utils::BufferView<const LayerRef>		getLayers() const;

	bool									layersHaveChanged() const;
	void									draw(Graphics::CommandBuffer& cmd);

	Graphics::RenderPass					getRenderPass() const;

	static UniformBufferLayout				getUniformBufferLayout(const Graphics::Vulkan& vulkan);
	static vk::DescriptorSetLayout			getDescriptorSetLayout(const Graphics::Vulkan& vulkan);
	static vk::PipelineLayout				getPipelineLayout(const Graphics::Vulkan& vulkan);

protected:
	void									setDepthStencilFormatCompatibility(Utils::Limit<DepthStencilFormat> comp);

	void									setInternalDepthStencilFormatCallback(DepthStencilFormatCallback cbk);
	const DepthStencilFormatCallback&		getInternalDepthStencilFormatCallback() const;

	void									setCameraCallback(CameraCallback cbk);
	const CameraCallback&					getCameraCallback() const;

	void									setRenderPassQueryCallbackCallback(RenderPassQueryCallback cbk);
	const RenderPassQueryCallback&			getRenderPassQueryCallback() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>		m_impl;		

};



class RendererBase::Camera {
public:
	enum class Projection {
		ORTHOGONAL,
		FRUSTUM
	};

	explicit Camera(const Math::Transformf& trf = Math::Transformf(),
					Projection projection		= Projection::ORTHOGONAL,
					float nearClip				= -10e3,
					float farClip 				= +10e3,
					float fov					= 0.0f ); // FOV Unused for orthogonal
	Camera(const Camera& other);
	~Camera();

	Camera& 				operator=(const Camera& other);

	bool					operator==(const Camera& other) const;
	bool					operator!=(const Camera& other) const;

	void					setTransform(const Math::Transformf& trf);
	const Math::Transformf&	getTransform() const;

	void					setProjection(Projection proj);
	Projection				getProjection() const;

	void					setNearClip(float near);
	float					getNearClip() const;

	void					setFarClip(float far);
	float					getFarClip() const;

	void					setFieldOfView(float fov);
	float					getFieldOfView() const;

	Math::Mat4x4f			calculateMatrix(const Math::Vec2f& size) const;
	Math::Mat4x4f			calculateViewMatrix() const;
	Math::Mat4x4f			calculateProjectionMatrix(const Math::Vec2f& size) const;

private:
	Math::Transformf		m_transform;
	Projection				m_projection;
	float					m_nearClip;
	float					m_farClip;
	float					m_fieldOfView;

};

}