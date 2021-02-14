#include <zuazo/Graphics/Frame.h>

namespace Zuazo::Graphics {

Frame::Geometry::Geometry(	std::byte* data,
							size_t stride,
							size_t positionOffset,
							size_t texCoordOffset,
							ScalingMode scaling,
							Math::Vec2f targetSize ) noexcept
	: m_data(data)
	, m_stride(stride)
	, m_positionOffset(positionOffset)
	, m_texCoordOffset(texCoordOffset)
	, m_scalingMode(scaling)
	, m_targetSize(targetSize)
	, m_sourceSize(0.0f)
{
}

void Frame::Geometry::setScalingMode(ScalingMode scaling) noexcept {
	m_scalingMode = scaling;
	m_sourceSize = Math::Vec2f(0.0f); //So that it gets recalculated
}

ScalingMode Frame::Geometry::getScalingMode() const noexcept {
	return m_scalingMode;
}


void Frame::Geometry::setTargetSize(Math::Vec2f size) noexcept {
	m_targetSize = size;
	m_sourceSize = Math::Vec2f(0.0f); //So that it gets recalculated
}

const Math::Vec2f& Frame::Geometry::getTargetSize() const noexcept {
	return m_targetSize;
}


bool Frame::Geometry::useFrame(const Frame& frame) {
	const auto frameSize = frame.getDescriptor().calculateSize();

	if(m_sourceSize != frameSize) {
		m_sourceSize = frameSize;
		updateBuffer();
		return true;
	} else {
		return false;
	}
}



void Frame::Geometry::updateBuffer() { 
	//Scale accordingly and clamp its value
	auto recSize = scale(m_sourceSize, m_targetSize, m_scalingMode);
	auto texSize = m_targetSize / recSize;
	recSize = Math::min(recSize, m_targetSize);
	texSize = Math::min(texSize, Math::Vec2f(1.0f));

	constexpr std::array vertexPositions {
		Math::Vec2f(-0.5f, -0.5f),
		Math::Vec2f(-0.5f, +0.5f),
		Math::Vec2f(+0.5f, -0.5f),
		Math::Vec2f(+0.5f, +0.5f),
	};

	static_assert(vertexPositions.size() == VERTEX_COUNT, "Vertex count does not match");

	for(size_t i = 0; i < vertexPositions.size(); i++) {
		auto& position = *(reinterpret_cast<Math::Vec2f*>(m_data + (i * m_stride) + m_positionOffset));
		auto& texCoord = *(reinterpret_cast<Math::Vec2f*>(m_data + (i * m_stride) + m_texCoordOffset));

		position = recSize * vertexPositions[i];
		texCoord = texSize * vertexPositions[i] + Math::Vec2f(0.5f);
	}
}

}