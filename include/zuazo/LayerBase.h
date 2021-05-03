#pragma once

#include "RendererBase.h"
#include "BlendingMode.h"
#include "RenderingLayer.h"
#include "Utils/Pimpl.h"
#include "Graphics/Vulkan.h"
#include "Graphics/CommandBuffer.h"
#include "Graphics/RenderPass.h"
#include "Math/Transform.h"

#include <functional>

namespace Zuazo {

class LayerBase {
public:
	using TransformCallback = std::function<void(LayerBase&, const Math::Transformf&)>;
	using OpacityCallback = std::function<void(LayerBase&, float)>;
	using BlendingModeCallback = std::function<void(LayerBase&, BlendingMode)>;
	using RenderingLayerCallback = std::function<void(LayerBase&, RenderingLayer)>;
	using HasChangedCallback = std::function<bool(const LayerBase&, const RendererBase&)>;
	using HasAlphaCallback = std::function<bool(const LayerBase&)>;
	using DrawCallback = std::function<void(const LayerBase&, const RendererBase&, Graphics::CommandBuffer&)>;
	using RenderPassCallback = std::function<void(LayerBase&, const Graphics::RenderPass&)>;


	LayerBase(	const RendererBase* renderer,
				TransformCallback transformCbk = {},
				OpacityCallback opacityCbk = {},
				BlendingModeCallback blendingModeCbk = {},
				RenderingLayerCallback renderModeCbk = {},
				HasChangedCallback hasChangedCbk = {},
				HasAlphaCallback hasAlphaCbk = {},
				DrawCallback drawCbk = {},
				RenderPassCallback renderPassCbk = {} );
	LayerBase(const LayerBase& other) = delete;
	LayerBase(LayerBase&& other);
	virtual ~LayerBase();

	LayerBase&							operator=(const LayerBase& other) = delete;
	LayerBase&							operator=(LayerBase&& other);

	void								setRenderer(const RendererBase* renderer);
	const RendererBase* 				getRenderer() const noexcept;

	void								setTransform(const Math::Transformf& trans);
	const Math::Transformf& 			getTransform() const noexcept;
	
	void								setOpacity(float opa);
	float								getOpacity() const noexcept;

	void								setBlendingMode(BlendingMode mode);
	BlendingMode						getBlendingMode() const noexcept;

	void								setRenderingLayer(RenderingLayer mode);
	RenderingLayer						getRenderingLayer() const noexcept;

	bool								hasBlending() const;
	bool								hasChanged(const RendererBase& renderer) const;
	void								draw(const RendererBase& renderer, Graphics::CommandBuffer& cmd) const;

	const Graphics::RenderPass&			getRenderPass() const noexcept;

protected:
	void								setTransformCallback(TransformCallback cbk);
	const TransformCallback&			getTransformCallback() const noexcept;

	void								setOpacityCallback(OpacityCallback cbk);
	const OpacityCallback&				getOpacityCallback() const noexcept;

	void								setBlendingModeCallback(BlendingModeCallback cbk);
	const BlendingModeCallback&			getBlendingModeCallback() const noexcept;

	void								setRenderingLayerCallback(RenderingLayerCallback cbk);
	const RenderingLayerCallback&		getRenderingLayerCallback() const noexcept;

	void								setHasChangedCallback(HasChangedCallback cbk);
	const HasChangedCallback&			getHasChangedCallback() const noexcept;

	void								setHasAlphaCallback(HasAlphaCallback cbk);
	const HasAlphaCallback&				getHasAlphaCallback() const noexcept;

	void								setDrawCallback(DrawCallback cbk);
	const DrawCallback&					getDrawCallback() const noexcept;

	void								setRenderPassCallback(RenderPassCallback cbk);
	const RenderPassCallback&			getRenderPassCallback() const noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;

};


}