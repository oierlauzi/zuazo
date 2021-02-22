#include <zuazo/Graphics/Frame.h>

namespace Zuazo::Graphics {

Frame::Geometry::Geometry(	ScalingMode scaling,
							Math::Vec2f targetSize ) noexcept
	: m_scalingMode(scaling)
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


std::pair<Math::Vec2f, Math::Vec2f> Frame::Geometry::calculateSurfaceSize() const noexcept {
		//Scale according to the mode
		auto recSize = scale(m_sourceSize, m_targetSize, m_scalingMode);

		//Obtain the texture size
		auto texSize = m_targetSize / recSize;

		//Clamp the values
		recSize = Math::min(recSize, m_targetSize);
		texSize = Math::min(texSize, Math::Vec2f(1.0f));

		return std::make_pair(recSize, texSize);
}


bool Frame::Geometry::useFrame(const Frame& frame) {
	const auto frameSize = frame.getDescriptor().calculateSize();

	if(m_sourceSize != frameSize) {
		m_sourceSize = frameSize;

		return true;
	} else {
		return false;
	}
}

void Frame::Geometry::writeQuadVertices(Math::Vec2f* position,
										Math::Vec2f* texCoord,
										size_t positionStride,
										size_t texCoordStride ) const noexcept
{
	const auto surfaceSize = calculateSurfaceSize();
	constexpr std::array vertexPositions {
		Math::Vec2f(-0.5f, -0.5f),
		Math::Vec2f(-0.5f, +0.5f),
		Math::Vec2f(+0.5f, -0.5f),
		Math::Vec2f(+0.5f, +0.5f),
	};
	static_assert(vertexPositions.size() == VERTEX_COUNT, "Vertex count does not match");

	for(size_t i = 0; i < vertexPositions.size(); ++i) {
		//Write the values
		*position = surfaceSize.first * vertexPositions[i];
		*texCoord = surfaceSize.second * vertexPositions[i] + Math::Vec2f(0.5f);
		
		//Advance write pointers
		reinterpret_cast<std::byte*&>(position) += positionStride;
		reinterpret_cast<std::byte*&>(texCoord) += texCoordStride;
	}

}

}