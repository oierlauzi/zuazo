#include <zuazo/RendererBase.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/LayerBase.h>

#include <algorithm>

namespace Zuazo {

struct RendererBase::Impl {
	Math::Vec2f											viewportSize;
	DepthStencilFormat									depthStencilFormat;
	Camera												camera;	
	std::vector<LayerRef>								layers;

	ViewportSizeCallback								viewportSizeCallback;
	DepthStencilFormatCallback							depthStencilFormatCallback;
	CameraCallback										cameraCallback;
	RenderPassQueryCallback								renderPassQueryCallback;

	Math::Mat4x4f										projectionMatrix; //Precomputed for use in layerComp
	std::vector<LayerRef>								sortedLayers;
	bool												hasChanged;

	static constexpr Math::Vec2f DUMMY_SIZE = Math::Vec2f(1.0f, 1.0f);


	Impl(	DepthStencilFormatCallback depthStencilFormatCbk, 
			CameraCallback cameraCbk, 
			RenderPassQueryCallback renderPassQueryCbk)
		: viewportSize()
		, depthStencilFormat(DepthStencilFormat::D16)
		, camera()
		, layers()
		, viewportSizeCallback()
		, depthStencilFormatCallback(std::move(depthStencilFormatCbk))
		, cameraCallback(std::move(cameraCbk))
		, renderPassQueryCallback(std::move(renderPassQueryCbk))
		, projectionMatrix(camera.calculateProjectionMatrix(DUMMY_SIZE))
		, hasChanged(true)
	{
	}

	~Impl() = default;



	void setViewportSize(RendererBase& base, Math::Vec2f size) {
		if(viewportSize != size) {
			viewportSize = size;
			Utils::invokeIf(viewportSizeCallback, base, size);
		}		
	}

	Math::Vec2f getViewportSize() const noexcept {
		return viewportSize;
	}

	void setViewportSizeCallback(ViewportSizeCallback cbk) {
		viewportSizeCallback = std::move(cbk);
	}

	const ViewportSizeCallback& getViewportSizeCallback() const noexcept {
		return viewportSizeCallback;
	}



	void setDepthStencilFormat(RendererBase& base, DepthStencilFormat fmt) {
		if(depthStencilFormat != fmt) {
			depthStencilFormat = fmt;
			Utils::invokeIf(depthStencilFormatCallback, base, depthStencilFormat);
		}
	}

	DepthStencilFormat getDepthStencilFormat() const noexcept {
		return depthStencilFormat;
	}



	void setCamera(RendererBase& base, const Camera& cam) {
		if(camera != cam) {
			camera = cam;
			projectionMatrix = camera.calculateProjectionMatrix(DUMMY_SIZE);
			Utils::invokeIf(cameraCallback, base, camera);
		}
	}

	const Camera& getCamera() const {
		return camera;
	}


	void setLayers(Utils::BufferView<const LayerRef> l) {
		const bool areEqual = std::equal(
			layers.cbegin(), layers.cend(),
			l.cbegin(), l.cend(),
			[] (LayerRef a, LayerRef b) -> bool {
				return &(a.get()) == &(b.get());
			}
		);
		
		if(!areEqual) {
			layers.clear();
			layers.insert(layers.cend(), l.cbegin(), l.cend());
			hasChanged = true;
		}
	}

	Utils::BufferView<const LayerRef> getLayers() const {
		return Utils::BufferView<const LayerRef>(layers.data(), layers.size());
	}


	bool layersHaveChanged(const RendererBase& renderer) const {
		if(hasChanged) {
			return true;
		}

		return std::any_of(
			layers.cbegin(), layers.cend(),
			[&renderer] (const LayerBase& layer) -> bool {
				return layer.hasChanged(renderer);
			}
		);
	}

	void draw(const RendererBase& renderer, Graphics::CommandBuffer& cmd) {
		assert(sortedLayers.empty());

		//Insert all the layers
		sortedLayers.insert(sortedLayers.cend(), layers.cbegin(), layers.cend());

		//Sort the layers based on their alpha and depth. Stable sort is used to preserve order
		std::stable_sort(
			sortedLayers.begin(), sortedLayers.end(),
			std::bind(&Impl::layerComp, std::cref(*this), std::placeholders::_1, std::placeholders::_2)
		);

		//Draw all the layers
		for(const LayerBase& layer : sortedLayers) {
			layer.draw(renderer, cmd);
		}

		//Empty the sorted layers array. This should not deallocate it
		sortedLayers.clear();
		hasChanged = false;
	}


	Graphics::RenderPass getRenderPass(const RendererBase& base) const {
		Graphics::RenderPass result;

		if(renderPassQueryCallback) {
			result = renderPassQueryCallback(base);
		}

		return result;
	}



	static UniformBufferSizes getUniformBufferSizes() noexcept {
		static const std::array uniformBufferSizes = {
			std::make_pair(static_cast<uint32_t>(DESCRIPTOR_BINDING_PROJECTION_MATRIX), 	sizeof(Math::Mat4x4f)),
			std::make_pair(static_cast<uint32_t>(DESCRIPTOR_BINDING_OUTPUT_COLOR_TRANSFER),	Graphics::OutputColorTransfer::size())
		};

		return uniformBufferSizes;
	}

	static DescriptorPoolSizes getDescriptorPoolSizes() noexcept {
		static const std::array descriptorPoolSizes = {
			vk::DescriptorPoolSize(
				vk::DescriptorType::eUniformBuffer,
				getUniformBufferSizes().size()
			)
		};

		return descriptorPoolSizes;
	}

	static vk::DescriptorSetLayout getDescriptorSetLayout(const Graphics::Vulkan& vulkan) {
		static const Utils::StaticId id;

		auto result = vulkan.createDescriptorSetLayout(id);

		if(!result) {
			//Create the bindings
			const std::array bindings = {
				vk::DescriptorSetLayoutBinding(	//UBO binding
					DESCRIPTOR_BINDING_PROJECTION_MATRIX,			//Binding
					vk::DescriptorType::eUniformBuffer,				//Type
					1,												//Count
					vk::ShaderStageFlagBits::eVertex,				//Shader stage
					nullptr											//Immutable samplers
				), 
				vk::DescriptorSetLayoutBinding(	//UBO binding
					DESCRIPTOR_BINDING_OUTPUT_COLOR_TRANSFER,		//Binding
					vk::DescriptorType::eUniformBuffer,				//Type
					1,												//Count
					vk::ShaderStageFlagBits::eFragment,				//Shader stage
					nullptr											//Immutable samplers
				), 
			};

			const vk::DescriptorSetLayoutCreateInfo createInfo(
				{},
				bindings.size(), bindings.data()
			);

			result = vulkan.createDescriptorSetLayout(id, createInfo);
		}

		assert(result);
		return result;
	}

	static vk::PipelineLayout getBasePipelineLayout(const Graphics::Vulkan& vulkan) {
		//This pipeline layout won't be used to create any pipeline, but it must be compatible with the
		// 1st descriptor set of all the pipelines, so that the color transfer and projection-view matrices
		// are bound.
		static const Utils::StaticId id;

		auto result = vulkan.createPipelineLayout(id);
		if(!result) {
			const std::array layouts {
				getDescriptorSetLayout(vulkan)
			};

			const vk::PipelineLayoutCreateInfo createInfo(
				{},													//Flags
				layouts.size(), layouts.data(),						//Descriptor set layouts
				0, nullptr											//Push constants
			);

			result = vulkan.createPipelineLayout(id, createInfo);
		}

		assert(result);
		return result;
	}



	void setDepthStencilFormatCallback(DepthStencilFormatCallback cbk) {
		depthStencilFormatCallback= std::move(cbk);
	}
	
	const DepthStencilFormatCallback& getDepthStencilFormatCallback() const {
		return depthStencilFormatCallback;
	}
	

	void setCameraCallback(CameraCallback cbk) {
		cameraCallback = std::move(cbk);
	}

	const CameraCallback& getCameraCallback() const {
		return cameraCallback;
	}


	void setRenderPassQueryCallbackCallback(RenderPassQueryCallback cbk) {
		renderPassQueryCallback = std::move(cbk);
	}

	const RenderPassQueryCallback& getRenderPassQueryCallback() const {
		return renderPassQueryCallback;
	}
	
private:
	bool layerComp(const LayerBase& a, const LayerBase& b) const {
		/*
		 * The strategy will be the following:
		 * 1. Draw all the background objects in order, disabling depth tests
		 * 2. Draw the blend-less scene objects backwards, writing and testing depth
		 * 3. Draw the transparent scene objects forwards, writing and testing depth
		 * 4. Draw all the foreground objects in order, disabling depth tests
		 */
		bool result;

		if(a.getRenderingLayer() != b.getRenderingLayer()) {
			//Prioritize background, then scene and finally foreground
			result = static_cast<int>(a.getRenderingLayer()) < static_cast<int>(b.getRenderingLayer());

		} else if(a.getRenderingLayer() == RenderingLayer::SCENE) {
			const auto aHasBlending = a.hasBlending();
			const auto bHasBlending = b.hasBlending();

			if(aHasBlending != bHasBlending) {
				//Prioritize alphaless drawing
				result = aHasBlending < bHasBlending;
			} else {
				//Both use the same 3D rendering mode and both or neither have alpha. 
				//Depth must be taken in consideration in order to decide which comes first
				//Calculate the average depth 
				constexpr size_t zRowIndex = 2; //0: x, 1: y, 2: z, 3: w
				const auto zProjection = projectionMatrix.getRow(zRowIndex);

				const auto aDepth = Math::dot(zProjection, Math::Vec4f(a.getTransform().getPosition(), 1.0f));
				const auto bDepth = Math::dot(zProjection, Math::Vec4f(b.getTransform().getPosition(), 1.0f));

				if(aHasBlending) {
					//Both layers have alpha. Render the furthest one first
					result = aDepth > bDepth;
				} else {
					//Neither of the layers alpha. Render the closest one first
					result = aDepth < bDepth;
				}
			}

		} else {
			//Both layers are using the same 2D rendering mode. Render them in order
			result = false;

		}

		return result;
	}

};



RendererBase::RendererBase(	DepthStencilFormatCallback depthStencilFormatCbk,
							CameraCallback cameraCbk,
							RenderPassQueryCallback renderPassQueryCbk )
	: m_impl({}, std::move(depthStencilFormatCbk), std::move(cameraCbk), std::move(renderPassQueryCbk))
{
}

RendererBase::RendererBase(RendererBase&& other) = default;

RendererBase::~RendererBase() = default; 

RendererBase& RendererBase::operator=(RendererBase&& other) = default;



Math::Vec2f RendererBase::getViewportSize() const noexcept {
	return m_impl->getViewportSize();
}

void RendererBase::setViewportSizeCallback(ViewportSizeCallback cbk) {
	m_impl->setViewportSizeCallback(std::move(cbk));
}

const RendererBase::ViewportSizeCallback& RendererBase::getViewportSizeCallback() const noexcept {
	return m_impl->getViewportSizeCallback();
}



void RendererBase::setDepthStencilFormat(DepthStencilFormat fmt) {
	m_impl->setDepthStencilFormat(*this, fmt);
}

DepthStencilFormat RendererBase::getDepthStencilFormat() const noexcept {
	return m_impl->getDepthStencilFormat();
}



void RendererBase::setCamera(const Camera& cam) {
	m_impl->setCamera(*this, cam);
}

const RendererBase::Camera& RendererBase::getCamera() const {
	return m_impl->getCamera();
}


bool RendererBase::layersHaveChanged() const {
	return m_impl->layersHaveChanged(*this);
}

void RendererBase::draw(Graphics::CommandBuffer& cmd) {
	m_impl->draw(*this, cmd);
}


void RendererBase::setLayers(Utils::BufferView<const LayerRef> layers) {
	m_impl->setLayers(layers);
}

Utils::BufferView<const RendererBase::LayerRef> RendererBase::getLayers() const {
	return m_impl->getLayers();
}


Graphics::RenderPass RendererBase::getRenderPass() const {
	return m_impl->getRenderPass(*this);
}



RendererBase::UniformBufferSizes RendererBase::getUniformBufferSizes() noexcept {
	return Impl::getUniformBufferSizes();
}

RendererBase::DescriptorPoolSizes RendererBase::getDescriptorPoolSizes() noexcept {
	return Impl::getDescriptorPoolSizes();
}

vk::DescriptorSetLayout RendererBase::getDescriptorSetLayout(const Graphics::Vulkan& vulkan) {
	return Impl::getDescriptorSetLayout(vulkan);
}

vk::PipelineLayout RendererBase::getBasePipelineLayout(const Graphics::Vulkan& vulkan) {
	return Impl::getBasePipelineLayout(vulkan);
}



void RendererBase::setViewportSize(Math::Vec2f size) {
	m_impl->setViewportSize(*this, size);
}



void RendererBase::setDepthStencilFormatCallback(DepthStencilFormatCallback cbk) {
	m_impl->setDepthStencilFormatCallback(std::move(cbk));
}

const RendererBase::DepthStencilFormatCallback& RendererBase::getDepthStencilFormatCallback() const {
	return m_impl->getDepthStencilFormatCallback();
}


void RendererBase::setCameraCallback(CameraCallback cbk) {
	m_impl->setCameraCallback(std::move(cbk));
}

const RendererBase::CameraCallback&	RendererBase::getCameraCallback() const {
	return m_impl->getCameraCallback();
}


void RendererBase::setRenderPassQueryCallbackCallback(RenderPassQueryCallback cbk) {
	m_impl->setRenderPassQueryCallbackCallback(std::move(cbk));
}

const RendererBase::RenderPassQueryCallback& RendererBase::getRenderPassQueryCallback() const {
	return m_impl->getRenderPassQueryCallback();
}

}