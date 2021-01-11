#include <zuazo/RendererBase.h>

#include <zuazo/Graphics/ColorTransfer.h>
#include <zuazo/Utils/StaticId.h>
#include <zuazo/LayerBase.h>

#include <algorithm>

#include <glm/gtc/matrix_access.hpp> 

namespace Zuazo {

struct RendererBase::Impl {
	enum VideoModeCallbacks {
		DSCBK_INTERNAL,
		DSCBK_EXTERNAL,
		DSCBK_COUNT
	};

	Utils::Limit<DepthStencilFormat>					depthStencilFmtLimits;
	Utils::Limit<DepthStencilFormat>					depthStencilFmtCompatibility;
	Utils::Limit<DepthStencilFormat>					depthStencilFmt;

	Camera												camera;	
	std::vector<LayerRef>								layers;

	DepthStencilFormatCallback							depthStencilFmtCompatibilityCbk;
	std::array<DepthStencilFormatCallback, DSCBK_COUNT>	depthStencilFmtCbk;
	CameraCallback										cameraCbk;
	RenderPassQueryCallback								renderPassQueryCbk;

	Math::Mat4x4f										projectionMatrix; //Precomputed for use in layerComp
	std::vector<LayerRef>								sortedLayers;
	bool												hasChanged;

	static constexpr Math::Vec2f DUMMY_SIZE = Math::Vec2f(1.0f, 1.0f);


	Impl(	Utils::Limit<DepthStencilFormat> depthStencil,
			DepthStencilFormatCallback internalDepthStencilFormatCbk, 
			CameraCallback cameraCbk, 
			RenderPassQueryCallback renderPassQueryCbk)
		: depthStencilFmtLimits(std::move(depthStencil))
		, depthStencilFmtCompatibility()
		, depthStencilFmt()
		, camera()
		, layers()
		, depthStencilFmtCompatibilityCbk()
		, depthStencilFmtCbk{ std::move(internalDepthStencilFormatCbk) }
		, cameraCbk(std::move(cameraCbk))
		, renderPassQueryCbk(std::move(renderPassQueryCbk))
		, projectionMatrix(camera.calculateProjectionMatrix(DUMMY_SIZE))
		, hasChanged(true)
	{
	}

	~Impl() = default;


	void setDepthStencilFormatCompatibilityCallback(DepthStencilFormatCallback cbk) {
		depthStencilFmtCompatibilityCbk = std::move(cbk);
	}

	const DepthStencilFormatCallback& getDepthStencilFormatCompatibilityCallback() const {
		return depthStencilFmtCompatibilityCbk;
	}


	void setDepthStencilFormatCallback(DepthStencilFormatCallback cbk) {
		depthStencilFmtCbk[DSCBK_EXTERNAL] = std::move(cbk);
	}

	const DepthStencilFormatCallback& getDepthStencilFormatCallback() const {
		return depthStencilFmtCbk[DSCBK_EXTERNAL];
	}


	void setDepthStencilFormatLimits(RendererBase& base, Utils::Limit<DepthStencilFormat> lim) {
		if(depthStencilFmtLimits != lim) {
			depthStencilFmtLimits = std::move(lim);
			updateDepthStencilFormat(base);
		}
	}

	const Utils::Limit<DepthStencilFormat>& getDepthStencilFormatLimits() const {
		return depthStencilFmtLimits;
	}

	const Utils::Limit<DepthStencilFormat>& getDepthStencilFormatCompatibility() const {
		return depthStencilFmtCompatibility;
	}

	const Utils::Limit<DepthStencilFormat>& getDepthStencilFormat() const {
		return depthStencilFmt;
	}



	void setCamera(RendererBase& base, const Camera& cam) {
		if(camera != cam) {
			camera = cam;
			projectionMatrix = camera.calculateProjectionMatrix(DUMMY_SIZE);
			Utils::invokeIf(cameraCbk, base, camera);
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

		if(renderPassQueryCbk) {
			result = renderPassQueryCbk(base);
		}

		return result;
	}



	static UniformBufferLayout getUniformBufferLayout(const Graphics::Vulkan& vulkan) {
		const auto& limits = vulkan.getPhysicalDeviceProperties().limits;

		constexpr size_t projectionMatrixOff = 0;
		constexpr size_t projectionMatrixSize = sizeof(glm::mat4);
		
		const size_t colorTansferOff = Utils::align(
			projectionMatrixOff + projectionMatrixSize, 
			limits.minUniformBufferOffsetAlignment
		);
		const size_t colorTansferSize = Graphics::OutputColorTransfer::size();

		return UniformBufferLayout {
			Utils::Area(projectionMatrixOff,	projectionMatrixSize),	//Projection matrix
			Utils::Area(colorTansferOff,		colorTansferSize )		//Color Transfer
		};
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



	void setDepthStencilFormatCompatibility(RendererBase& base, Utils::Limit<DepthStencilFormat> comp) {
		if(depthStencilFmtCompatibility != comp) {
			depthStencilFmtCompatibility = std::move(comp);
			Utils::invokeIf(depthStencilFmtCompatibilityCbk, base, depthStencilFmtCompatibility);
			updateDepthStencilFormat(base);
		}
	}


	void setInternalDepthStencilFormatCallback(DepthStencilFormatCallback cbk) {
		depthStencilFmtCbk[DSCBK_INTERNAL] = std::move(cbk);
	}
	
	const DepthStencilFormatCallback& getInternalDepthStencilFormatCallback() const {
		return depthStencilFmtCbk[DSCBK_INTERNAL];
	}
	

	void setCameraCallback(CameraCallback cbk) {
		cameraCbk = std::move(cbk);
	}

	const CameraCallback& getCameraCallback() const {
		return cameraCbk;
	}


	void setRenderPassQueryCallbackCallback(RenderPassQueryCallback cbk) {
		renderPassQueryCbk = std::move(cbk);
	}

	const RenderPassQueryCallback& getRenderPassQueryCallback() const {
		return renderPassQueryCbk;
	}
	
private:
	void updateDepthStencilFormat(RendererBase& base) noexcept {
		auto newDepthStencilFormat = depthStencilFmtCompatibility.intersect(depthStencilFmtLimits);

		if(newDepthStencilFormat != depthStencilFmt) {
			//Depth Stencil format has changed
			depthStencilFmt = std::move(newDepthStencilFormat);

			//Call the callbacks
			for(const auto& cbk : depthStencilFmtCbk) {
				Utils::invokeIf(cbk, base, depthStencilFmt);
			}
		}
	}

	bool layerComp(const LayerBase& a, const LayerBase& b) const {
		/*
			* The strategy will be the following:
			* 1. Draw the alphaless objects backwards, writing and testing depth
			* 2. Draw the transparent objscts forwards, writing and testing depth
			*/
		
		if(a.hasAlpha() != b.hasAlpha()) {
			//Prioritize alphaless drawing
			return a.hasAlpha() < b.hasAlpha();
		} else {
			//Both or neither have alpha. Depth must be taken in consideration
			assert(a.hasAlpha() == b.hasAlpha());
			const auto hasAlpha = a.hasAlpha();

			//Calculate the average depth 
			constexpr int zRowIndex = 2; //0: x, 1: y, 2: z, 3: w
			const auto zProjection = glm::row(projectionMatrix, zRowIndex);

			const auto aDepth = glm::dot(zProjection, Math::Vec4f(a.getTransform().getPosition(), 1.0f));
			const auto bDepth = glm::dot(zProjection, Math::Vec4f(b.getTransform().getPosition(), 1.0f));

			return !hasAlpha
				? aDepth < bDepth
				: aDepth > bDepth;
		}
	}

};



RendererBase::RendererBase(	Utils::Limit<DepthStencilFormat> depthStencil,
							DepthStencilFormatCallback internalDepthStencilFormatCbk,
							CameraCallback cameraCbk,
							RenderPassQueryCallback renderPassQueryCbk )
	: m_impl({}, std::move(depthStencil), std::move(internalDepthStencilFormatCbk), std::move(cameraCbk), std::move(renderPassQueryCbk))
{
}

RendererBase::RendererBase(RendererBase&& other) = default;

RendererBase::~RendererBase() = default; 

RendererBase& RendererBase::operator=(RendererBase&& other) = default;



void RendererBase::setDepthStencilFormatCompatibilityCallback(DepthStencilFormatCallback cbk) {
	m_impl->setDepthStencilFormatCompatibilityCallback(std::move(cbk));
}

const RendererBase::DepthStencilFormatCallback& RendererBase::getDepthStencilFormatCompatibilityCallback() const {
	return m_impl->getDepthStencilFormatCompatibilityCallback();
}


void RendererBase::setDepthStencilFormatCallback(DepthStencilFormatCallback cbk) {
	m_impl->setDepthStencilFormatCallback(std::move(cbk));
}

const RendererBase::DepthStencilFormatCallback& RendererBase::getDepthStencilFormatCallback() const {
	return m_impl->getDepthStencilFormatCallback();
}


void RendererBase::setDepthStencilFormatLimits(Utils::Limit<DepthStencilFormat> lim) {
	m_impl->setDepthStencilFormatLimits(*this, std::move(lim));
}

const Utils::Limit<DepthStencilFormat>&	RendererBase::getDepthStencilFormatLimits() const {
	return m_impl->getDepthStencilFormatLimits();
}

const Utils::Limit<DepthStencilFormat>& RendererBase::getDepthStencilFormatCompatibility() const {
	return m_impl->getDepthStencilFormatCompatibility();
}

const Utils::Limit<DepthStencilFormat>&	RendererBase::getDepthStencilFormat() const {
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


RendererBase::UniformBufferLayout RendererBase::getUniformBufferLayout(const Graphics::Vulkan& vulkan) {
	return Impl::getUniformBufferLayout(vulkan);
}

vk::DescriptorSetLayout	RendererBase::getDescriptorSetLayout(const Graphics::Vulkan& vulkan) {
	return Impl::getDescriptorSetLayout(vulkan);
}

vk::PipelineLayout RendererBase::getPipelineLayout(const Graphics::Vulkan& vulkan) {
	return Impl::getBasePipelineLayout(vulkan);
}


void RendererBase::setDepthStencilFormatCompatibility(Utils::Limit<DepthStencilFormat> comp) {
	m_impl->setDepthStencilFormatCompatibility(*this, std::move(comp));
}

void RendererBase::setInternalDepthStencilFormatCallback(DepthStencilFormatCallback cbk) {
	m_impl->setInternalDepthStencilFormatCallback(std::move(cbk));
}

const RendererBase::DepthStencilFormatCallback& RendererBase::getInternalDepthStencilFormatCallback() const {
	return m_impl->getInternalDepthStencilFormatCallback();
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