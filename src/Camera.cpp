#include <zuazo/RendererBase.h>

#include <zuazo/Math/Transformations.h>
#include <zuazo/Math/Trigonometry.h>

namespace Zuazo {

RendererBase::Camera::Camera(	const Math::Transformf& trf,
								Projection projection,
								float nearClip,
								float farClip,
								float fov )
	: m_transform(trf)
	, m_projection(projection)
	, m_nearClip(nearClip)
	, m_farClip(farClip)
	, m_fieldOfView(fov)
{
}

RendererBase::Camera::Camera(const Camera& other) = default;

RendererBase::Camera::~Camera() = default;

RendererBase::Camera& RendererBase::Camera::operator=(const Camera& other) = default;



bool RendererBase::Camera::operator==(const Camera& other) const {
	bool result = 	m_transform == other.m_transform &&
					m_projection == other.m_projection &&
					m_nearClip == other.m_nearClip &&
					m_farClip == other.m_farClip ;

	if(m_projection == Projection::frustum) {
		result = result && m_fieldOfView == other.m_fieldOfView; //Field of view is only used for frustum
	}

	return result;
}

bool RendererBase::Camera::operator!=(const Camera& other) const {
	return !operator==(other);
}



void RendererBase::Camera::setTransform(const Math::Transformf& trf) {
	m_transform = trf;
}

const Math::Transformf& RendererBase::Camera::getTransform() const {
	return m_transform;
}


void RendererBase::Camera::setProjection(Projection proj) {
	m_projection = proj;
}

RendererBase::Camera::Projection RendererBase::Camera::getProjection() const {
	return m_projection;
}


void RendererBase::Camera::setNearClip(float near) {
	m_nearClip = near;
}

float RendererBase::Camera::getNearClip() const {
	return m_nearClip;
}


void RendererBase::Camera::setFarClip(float far) {
	m_farClip = far;
}

float RendererBase::Camera::getFarClip() const {
	return m_farClip;
}


void RendererBase::Camera::setFieldOfView(float fov) {
	m_fieldOfView = fov;
}

float RendererBase::Camera::getFieldOfView() const {
	return m_fieldOfView;
}



Math::Mat4x4f RendererBase::Camera::calculateMatrix(const Math::Vec2f& size) const {
	const auto projectionMtx = calculateProjectionMatrix(size);
	const auto viewMtx = calculateViewMatrix();
	return projectionMtx * viewMtx;
}

Math::Mat4x4f RendererBase::Camera::calculateViewMatrix() const {
	return Math::inv(getTransform().calculateMatrix());
}

Math::Mat4x4f RendererBase::Camera::calculateProjectionMatrix(const Math::Vec2f& size) const {
	Math::Mat4x4f result;

	switch(m_projection) {
	case Projection::orthogonal:
		result = Math::ortho(
			-size.x / 2.0f, +size.x / 2.0f,	//Left, Right
			-size.y / 2.0f, +size.y / 2.0f,	//Bottom, Top
			m_nearClip, m_farClip			//Clipping planes
		);

		break;

	case Projection::frustum:
		if(std::isinf(m_farClip)) {
			result = Math::perspective(
				getFieldOfView(),				//Vertical FOV
				size.x / size.y,				//Aspect ratio
				m_nearClip						//Clipping plane
			);
		} else {
			result = Math::perspective(
				getFieldOfView(),				//Vertical FOV
				size.x / size.y,				//Aspect ratio
				m_nearClip, m_farClip			//Clipping planes
			);
		}

		break;

	}

	return result;
}

}