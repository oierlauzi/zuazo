#pragma once

#include "RendererBase.h"
#include "BlendingMode.h"
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
	using HasChangedCallback = std::function<bool(const LayerBase&, const RendererBase&)>;
	using DrawCallback = std::function<void(const LayerBase&, const RendererBase&, Graphics::CommandBuffer&)>;
	using RenderPassCallback = std::function<void(LayerBase&, Graphics::RenderPass)>;

	LayerBase(	const RendererBase* renderer,
				TransformCallback transformCbk = {},
				OpacityCallback opacityCbk = {},
				BlendingModeCallback blendingModeCbk = {},
				HasChangedCallback hasChangedCbk = {},
				DrawCallback drawCbk = {},
				RenderPassCallback renderPassCbk = {} );
	LayerBase(const LayerBase& other) = delete;
	LayerBase(LayerBase&& other);
	virtual ~LayerBase();

	LayerBase&							operator=(const LayerBase& other) = delete;
	LayerBase&							operator=(LayerBase&& other);

	void								setRenderer(const RendererBase* renderer);
	const RendererBase* 				getRenderer() const;

	void								setTransform(const Math::Transformf& trans);
	const Math::Transformf& 			getTransform() const;
	
	void								setOpacity(float opa);
	float								getOpacity() const;

	void								setBlendingMode(BlendingMode mode);
	BlendingMode						getBlendingMode() const;

	bool								hasAlpha() const;
	bool								hasChanged(const RendererBase& renderer) const;
	void								draw(const RendererBase& renderer, Graphics::CommandBuffer& cmd) const;

	Graphics::RenderPass				getRenderPass() const;

protected:
	void								setTransformCallback(TransformCallback cbk);
	const TransformCallback&			getTransformCallback() const;

	void								setOpacityCallback(OpacityCallback cbk);
	const OpacityCallback&				getOpacityCallback() const;

	void								setBlendingModeCallback(BlendingModeCallback cbk);
	const BlendingModeCallback&			getBlendingModeCallback() const;

	void								setHasChangedCallback(HasChangedCallback cbk);
	const HasChangedCallback&			getHasChangedCallback() const;

	void								setDrawCallback(DrawCallback cbk);
	const DrawCallback&					getDrawCallback() const;

	void								setRenderPassCallback(RenderPassCallback cbk);
	const RenderPassCallback&			getRenderPassCallback() const;

private:
	struct Impl;
	Utils::Pimpl<Impl>					m_impl;

};


}