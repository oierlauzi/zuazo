#include <zuazo/LayerBase.h>

#include <zuazo/Utils/StaticId.h>

#include <utility>

namespace Zuazo {

struct LayerBase::Impl {
	Math::Transformf								transform;
	float											opacity;
	BlendingMode									blendingMode;
	RenderingLayer									renderingLayer;

	vk::RenderPass 									renderPass;

	TransformCallback								transformCallback;
	OpacityCallback									opacityCallback;
	BlendingModeCallback							blendingModeCallback;
	RenderingLayerCallback							renderingLayerCallback;
	HasChangedCallback								hasChangedCallback;
	HasAlphaCallback								hasAlphaCallback;
	DrawCallback									drawCallback;
	RenderPassCallback								renderPassCallback;


	Impl(	TransformCallback transformCbk,
			OpacityCallback opacityCbk,
			BlendingModeCallback blendingModeCbk,
			RenderingLayerCallback renderingLayerCbk,
			HasChangedCallback hasChangedCbk,
			HasAlphaCallback hasAlphaCbk,
			DrawCallback drawCbk,
			RenderPassCallback renderPassCbk )
		: transform()
		, opacity(1.0f)
		, blendingMode(BlendingMode::opacity)
		, renderingLayer(RenderingLayer::scene)
		, renderPass()
		, transformCallback(std::move(transformCbk))
		, opacityCallback(std::move(opacityCbk))
		, blendingModeCallback(std::move(blendingModeCbk))
		, renderingLayerCallback(std::move(renderingLayerCbk))
		, hasChangedCallback(std::move(hasChangedCbk))
		, hasAlphaCallback(std::move(hasAlphaCbk))
		, drawCallback(std::move(drawCbk))
		, renderPassCallback(std::move(renderPassCbk))
	{
	}

	~Impl() = default;


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



	bool hasBlending(const LayerBase& base) const {
		bool result;

		if(blendingMode == BlendingMode::none) {
			//This blending mode does not write anything, so it can be considered as alphaless
			result = false;
		} else if(blendingMode == BlendingMode::write) {
			//This blending mode overwrites the framebuffer, so it can be considered as alphaless
			result = false;
		} else if(blendingMode == BlendingMode::opacity) {
			//This blending mode will overwrite the framebuffer for alpha == 1
			if(opacity != 1) {
				result = true;
			} else if(hasAlphaCallback) {
				//Expensive call!
				result = hasAlphaCallback(base);
			} else {
				result = false;
			}
		} else {
			//Default to true
			result = true;
		}

		return result;
	}

	bool hasChanged(const LayerBase& base, const RendererBase& renderer) const {
		return hasChangedCallback ? hasChangedCallback(base, renderer) : true;
	}

	void draw(const LayerBase& base, const RendererBase& renderer, Graphics::CommandBuffer& cmd) const {
		if(hasEffect()) {
			Utils::invokeIf(drawCallback, base, renderer, cmd);
		}
	}



	void setRenderPass(LayerBase& base, vk::RenderPass pass) {
		if(renderPass != pass) {
			renderPass = pass;
			Utils::invokeIf(renderPassCallback, base, renderPass);
		} 
	}

	vk::RenderPass getRenderPass() const noexcept {
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


	void setHasAlphaCallback(HasAlphaCallback cbk) {
		hasAlphaCallback = std::move(cbk);
	}

	const HasAlphaCallback& getHasAlphaCallback() const noexcept {
		return hasAlphaCallback;
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

private:
	bool hasEffect() const noexcept {
		bool result;

		//When one of these blending modes is used in conjunction
		//with an alpha value of zero, the framebuffer will not be
		//altered. Demonstration of each format on its comment.
		constexpr std::array<BlendingMode, 4> A0_NOPS = {
			BlendingMode::opacity,			//C = C_src * 0 + C_dst * (1 - 0) = C_dst
			BlendingMode::add,				//C = C_src * 0 + C_dst = C_dst
			BlendingMode::differenceInv,	//C = C_dst - C_src * 0 = C_dst
			BlendingMode::screen			//C = C_src * 0 + C_dst * (1 - C_src * 0) = C_dst
		};

		//For binary search:
		assert(std::is_sorted(A0_NOPS.cbegin(), A0_NOPS.cend()));

		if(blendingMode == BlendingMode::none) {
			//Write is disabled
			result = false;
		} else if(opacity == 0.0f && std::binary_search(A0_NOPS.cbegin(), A0_NOPS.cend(), blendingMode)) {
			//These blending modes have no effect with zero opacity
			result = false;
		} else {
			//By default, it will be visible
			result = true;
		}

		return result;
	}

};



LayerBase::LayerBase(	TransformCallback transformCbk,
						OpacityCallback opacityCbk,
						BlendingModeCallback blendingModeCbk,
						RenderingLayerCallback renderingLayerCbk,
						HasChangedCallback hasChangedCbk,
						HasAlphaCallback hasAlphaCbk,
						DrawCallback drawCbk,
						RenderPassCallback renderPassCbk )
	: m_impl(	{}, 
				std::move(transformCbk), 
				std::move(opacityCbk), 
				std::move(blendingModeCbk), 
				std::move(renderingLayerCbk), 
				std::move(hasChangedCbk), 
				std::move(hasAlphaCbk),
				std::move(drawCbk),
				std::move(renderPassCbk) )
{
}

LayerBase::LayerBase(LayerBase&& other) = default;

LayerBase::~LayerBase() = default;

LayerBase& LayerBase::operator=(LayerBase&& other) = default;



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


bool LayerBase::hasBlending() const {
	return m_impl->hasBlending(*this);
}

bool LayerBase::hasChanged(const RendererBase& renderer) const {
	return m_impl->hasChanged(*this, renderer);
}

void LayerBase::draw(const RendererBase& renderer, Graphics::CommandBuffer& cmd) const {
	m_impl->draw(*this, renderer, cmd);
}


void LayerBase::setRenderPass(vk::RenderPass pass) {
	m_impl->setRenderPass(*this, pass);
}

vk::RenderPass LayerBase::getRenderPass() const noexcept {
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