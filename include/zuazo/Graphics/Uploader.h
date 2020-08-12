#pragma once

#include "Vulkan.h"
#include "StagedFrame.h"
#include "ColorTransfer.h"
#include "Buffer.h"

#include "../Utils/Pool.h"

#include <memory>
#include <utility>

namespace Zuazo::Graphics {

class Uploader {
public:
	Uploader(	const Vulkan& vulkan, 
				const Frame::Descriptor& conf );
	Uploader(const Uploader& other) = delete;
	Uploader(Uploader&& other) = default;
	~Uploader() = default;

	Uploader& 										operator=(const Uploader& other) = delete;

		void										setMaxSpares(size_t max);
	size_t											getMaxSpares() const;

	std::shared_ptr<StagedFrame>					acquireFrame() const;
	void											clear();

private:
	class Allocator {
	public:
		explicit Allocator(const Uploader& uploader);
		Allocator(const Allocator& other) = default;
		~Allocator() = default;

		Allocator& 										operator=(const Allocator& other) = default;

		StagedFrame*									operator()() const;

	private:
		std::reference_wrapper<const Uploader>			m_uploader;

	};

	const Vulkan&									m_vulkan;
	std::shared_ptr<Frame::Descriptor>				m_frameDescriptor;
	ColorTransfer									m_colorTransfer;
	std::vector<Frame::PlaneDescriptor>				m_planeDescriptors;

	std::shared_ptr<vk::UniqueCommandPool>			m_commandPool;
	std::shared_ptr<Buffer>							m_colorTransferBuffer;

	mutable Utils::Pool<StagedFrame, Allocator>		m_pool;

	static std::vector<Frame::PlaneDescriptor>		createPlaneDescriptors(	const Vulkan& vulkan, 
																			const Frame::Descriptor& desc,
																			ColorTransfer& colorTransfer );
	static std::shared_ptr<vk::UniqueCommandPool> 	createCommandPool(const Vulkan& vulkan);
};



}