#include <zuazo/LayerBase.h>

#include <zuazo/Utils/StaticId.h>

#include <utility>

namespace Zuazo {

struct LayerBase::Impl {
	const RendererBase*								renderer;

	Math::Transformf								transform;
	float											opacity;
	BlendingMode									blendingMode;
	RenderingLayer									renderingLayer;

	Graphics::RenderPass							renderPass;

	TransformCallback								transformCallback;
	OpacityCallback									opacityCallback;
	BlendingModeCallback							blendingModeCallback;
	RenderingLayerCallback							renderingLayerCallback;
	HasChangedCallback								hasChangedCallback;
	DrawCallback									drawCallback;
	RenderPassCallback								renderPassCallback;


	Impl(	const RendererBase* renderer,
			TransformCallback transformCbk,
			OpacityCallback opacityCbk,
			BlendingModeCallback blendingModeCbk,
			RenderingLayerCallback renderingLayerCbk,
			HasChangedCallback hasChangedCbk,
			DrawCallback drawCbk,
			RenderPassCallback renderPassCbk )
		: renderer(renderer)
		, transform()
		, opacity(1.0f)
		, blendingMode(BlendingMode::OPACITY)
		, renderingLayer(RenderingLayer::SCENE)
		, renderPass(renderer ? renderer->getRenderPass() : Graphics::RenderPass())
		, transformCallback(std::move(transformCbk))
		, opacityCallback(std::move(opacityCbk))
		, blendingModeCallback(std::move(blendingModeCbk))
		, renderingLayerCallback(std::move(renderingLayerCbk))
		, hasChangedCallback(std::move(hasChangedCbk))
		, drawCallback(std::move(drawCbk))
		, renderPassCallback(std::move(renderPassCbk))
	{
	}

	~Impl() = default;


	void setRenderer(LayerBase& base, const RendererBase* rend) {
		renderer = rend;
		const auto rendPass = renderer ? renderer->getRenderPass() : Graphics::RenderPass();

		if(renderPass != rendPass) {
			renderPass = rendPass;
			Utils::invokeIf(renderPassCallback, base, renderPass);
		}
	}

	const RendererBase* getRenderer() const noexcept {
		return renderer;
	}


	void setTransform(LayerBase& base, const Math::Transformf& trans) {
		if(transform != trans) {
			transform = trans;
			Utils::invokeIf(transformCallback, base, transform);
		}
	}
	
	const Math::Transformf& getTransform() const noexcept {
		return transform;
	}
	
	
	void setOpacity(LayerBase& base, float opa) {
		if(opacity != opa) {
			opacity = opa;
			Utils::invokeIf(opacityCallback, base, opacity);
		}
	}

	float getOpacity() const noexcept {
		return opacity;
	}


	void setBlendingMode(LayerBase& base, BlendingMode mode) {
		if(blendingMode != mode) {
			blendingMode = mode;
			Utils::invokeIf(blendingModeCallback, base, blendingMode);
		}
	}

	BlendingMode getBlendingMode() const noexcept {
		return blendingMode;
	}

	void setRenderingLayer(LayerBase& base, RenderingLayer mode) {
		if(renderingLayer != mode) {
			renderingLayer = mode;
			Utils::invokeIf(renderingLayerCallback, base, renderingLayer);
		}
	}

	RenderingLayer getRenderingLayer() const noexcept {
		return renderingLayer;
	}



	bool hasAlpha() const {
		return opacity != 1.0f; //TODO also take into account the actual layer
	}

	bool hasChanged(const LayerBase& base, const RendererBase& renderer) const {
		return hasChangedCallback ? hasChangedCallback(base, renderer) : true;
	}

	void draw(const LayerBase& base, const RendererBase& renderer, Graphics::CommandBuffer& cmd) const {
		Utils::invokeIf(drawCallback, base, renderer, cmd);
	}


	Graphics::RenderPass getRenderPass() const noexcept {
		return renderPass;
	}


	void setTransformCallback(TransformCallback cbk) {
		transformCallback = std::move(cbk);
	}

	const TransformCallback& getTransformCallback() const noexcept {
		return transformCallback;
	}

	
	void setOpacityCallback(OpacityCallback cbk) {
		opacityCallback = std::move(cbk);
	}

	const OpacityCallback& getOpacityCallback() const noexcept {
		return opacityCallback;
	}

	
	void setBlendingModeCallback(BlendingModeCallback cbk) {
		blendingModeCallback = std::move(cbk);
	}

	const BlendingModeCallback& getBlendingModeCallback() const noexcept {
		return blendingModeCallback;
	}


	void setRenderingLayerCallback(RenderingLayerCallback cbk) {
		renderingLayerCallback = std::move(cbk);
	}

	const RenderingLayerCallback& getRenderingLayerCallback() const noexcept {
		return renderingLayerCallback;
	}


	void setHasChangedCallback(HasChangedCallback cbk) {
		hasChangedCallback = std::move(cbk);
	}

	const HasChangedCallback& getHasChangedCallback() const noexcept {
		return hasChangedCallback;
	}

	void setDrawCallback(DrawCallback cbk) {
		drawCallback = std::move(cbk);
	}

	const DrawCallback& getDrawCallback() const noexcept {
		return drawCallback;
	}

	void setRenderPassCallback(RenderPassCallback cbk) {
		renderPassCallback = std::move(cbk);
	}

	const RenderPassCallback& getRenderPassCallback() const noexcept {
		return renderPassCallback;
	}

};


LayerBase::LayerBase(	const RendererBase* renderer,
						TransformCallback transformCbk,
						OpacityCallback opacityCbk,
						BlendingModeCallback blendingModeCbk,
						RenderingLayerCallback renderingLayerCbk,
						HasChangedCallback hasChangedCbk,
						DrawCallback drawCbk,
						RenderPassCallback renderPassCbk )
	: m_impl(	{}, renderer, std::move(transformCbk), 
				std::move(opacityCbk), std::move(blendingModeCbk), 
				std::move(renderingLayerCbk), std::move(hasChangedCbk), 
				std::move(drawCbk), std::move(renderPassCbk) )
{
}

LayerBase::LayerBase(LayerBase&& other) = default;

LayerBase::~LayerBase() = default;

LayerBase& LayerBase::operator=(LayerBase&& other) = default;


void LayerBase::setRenderer(const RendererBase* renderer) {
	m_impl->setRenderer(*this, renderer);
}

const RendererBase* LayerBase::getRenderer() const noexcept {
	return m_impl->getRenderer();
}


void LayerBase::setTransform(const Math::Transformf& trans) {
	m_impl->setTransform(*this, trans);
}

const Math::Transformf& LayerBase::getTransform() const noexcept {
	return m_impl->getTransform();
}


void LayerBase::setOpacity(float opa) {
	m_impl->setOpacity(*this, opa);
}

float LayerBase::getOpacity() const noexcept {
	return m_impl->getOpacity();
}


void LayerBase::setBlendingMode(BlendingMode mode) {
	m_impl->setBlendingMode(*this, mode);
}

BlendingMode LayerBase::getBlendingMode() const noexcept {
	return m_impl->getBlendingMode();
}


void LayerBase::setRenderingLayer(RenderingLayer mode) {
	m_impl->setRenderingLayer(*this, mode);
}

RenderingLayer LayerBase::getRenderingLayer() const noexcept {
	return m_impl->getRenderingLayer();
}


bool LayerBase::hasAlpha() const {
	return m_impl->hasAlpha();
}

bool LayerBase::hasChanged(const RendererBase& renderer) const {
	return m_impl->hasChanged(*this, renderer);
}

void LayerBase::draw(const RendererBase& renderer, Graphics::CommandBuffer& cmd) const {
	m_impl->draw(*this, renderer, cmd);
}


Graphics::RenderPass LayerBase::getRenderPass() const noexcept {
	return m_impl->getRenderPass();
}


void LayerBase::setTransformCallback(TransformCallback cbk) {
	m_impl->setTransformCallback(std::move(cbk));
}

const LayerBase::TransformCallback& LayerBase::getTransformCallback() const noexcept {
	return m_impl->getTransformCallback();
}


void LayerBase::setOpacityCallback(OpacityCallback cbk) {
	m_impl->setOpacityCallback(std::move(cbk));
}

const LayerBase::OpacityCallback& LayerBase::getOpacityCallback() const noexcept {
	return m_impl->getOpacityCallback();
}


void LayerBase::setBlendingModeCallback(BlendingModeCallback cbk) {
	m_impl->setBlendingModeCallback(std::move(cbk));
}

const LayerBase::BlendingModeCallback& LayerBase::getBlendingModeCallback() const noexcept {
	return m_impl->getBlendingModeCallback();
}


void LayerBase::setRenderingLayerCallback(RenderingLayerCallback cbk) {
	m_impl->setRenderingLayerCallback(std::move(cbk));
}

const LayerBase::RenderingLayerCallback&	LayerBase::getRenderingLayerCallback() const noexcept {
	return m_impl->getRenderingLayerCallback();
}


void LayerBase::setHasChangedCallback(HasChangedCallback cbk) {
	m_impl->setHasChangedCallback(std::move(cbk));
}

const LayerBase::HasChangedCallback& LayerBase::getHasChangedCallback() const noexcept {
	return m_impl->getHasChangedCallback();
}


void LayerBase::setDrawCallback(DrawCallback cbk) {
	m_impl->setDrawCallback(std::move(cbk));
}

const LayerBase::DrawCallback& LayerBase::getDrawCallback() const noexcept {
	return m_impl->getDrawCallback();
}


void LayerBase::setRenderPassCallback(RenderPassCallback cbk) {
	m_impl->setRenderPassCallback(std::move(cbk));
}

const LayerBase::RenderPassCallback& LayerBase::getRenderPassCallback() const noexcept {
	return m_impl->getRenderPassCallback();
}

}