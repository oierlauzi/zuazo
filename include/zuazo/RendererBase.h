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

	using DepthStencilFormatCallback = std::function<void(RendererBase&, DepthStencilFormat)>;
	using CameraCallback = std::function<void(RendererBase&, const Camera& camera)>;
	using RenderPassQueryCallback = std::function<const Graphics::RenderPass&(const RendererBase&)>;

	enum DescriptorBindings {
		DESCRIPTOR_BINDING_PROJECTION_MATRIX,

		DESCRIPTOR_BINDING_COUNT,
	};

	using UniformBufferSizes = Utils::BufferView<const std::pair<uint32_t, size_t>>;
	using DescriptorPoolSizes = Utils::BufferView<const vk::DescriptorPoolSize>;
	using ViewportSizeCallback = std::function<void(RendererBase&, Math::Vec2f)>;

	static constexpr uint32_t DESCRIPTOR_SET = 0;

	RendererBase(	DepthStencilFormatCallback depthStencilCbk = {},
					CameraCallback cameraCbk = {},
					RenderPassQueryCallback renderPassQueryCbk = {} );
	RendererBase(const RendererBase& other) = delete;
	RendererBase(RendererBase&& other);
	virtual ~RendererBase();

	RendererBase& 							operator=(const RendererBase& other) = delete;
	RendererBase&							operator=(RendererBase&& other);

	Math::Vec2f								getViewportSize() const noexcept;
	void									setViewportSizeCallback(ViewportSizeCallback cbk);
	const ViewportSizeCallback&				getViewportSizeCallback() const noexcept;

	void									setDepthStencilFormat(DepthStencilFormat fmt);
	DepthStencilFormat						getDepthStencilFormat() const noexcept;

	void									setCamera(const Camera& cam);
	const Camera&							getCamera() const;

	void									setLayers(Utils::BufferView<const LayerRef> layers);
	Utils::BufferView<const LayerRef>		getLayers() const;

	bool									layersHaveChanged() const;
	void									draw(Graphics::CommandBuffer& cmd);

	const Graphics::RenderPass&				getRenderPass() const;

	static UniformBufferSizes				getUniformBufferSizes() noexcept;
	static DescriptorPoolSizes				getDescriptorPoolSizes() noexcept;
	static vk::DescriptorSetLayout			getDescriptorSetLayout(const Graphics::Vulkan& vulkan);
	static vk::PipelineLayout				getBasePipelineLayout(const Graphics::Vulkan& vulkan);

protected:
	void									setViewportSize(Math::Vec2f size);

	void									setDepthStencilFormatCallback(DepthStencilFormatCallback cbk);
	const DepthStencilFormatCallback&		getDepthStencilFormatCallback() const;

	void									setCameraCallback(CameraCallback cbk);
	const CameraCallback&					getCameraCallback() const;

	void									setRenderPassQueryCallbackCallback(RenderPassQueryCallback cbk);
	const RenderPassQueryCallback&			getRenderPassQueryCallback() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>						m_impl;		

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