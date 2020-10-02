#include "editor/animation/camera.hpp"
#include "editor/box2d_utils.hpp"

//#include "engine/utils.hpp"

using namespace sf;

/** Initialises a camera with default values.
*
* Disables clamping to the background size and provides
* default interpolation and duration values for the tweens.
*/
Camera::Camera() {
	InitDefault();
}

/** Initialises a camera for clamping to a background.
*
* Initialises data members to enable clamping the camera's
* position to the background container. Default interpolation
* and duration values are initialised.
*/
Camera::Camera(const Vector2f& position,
			   const Vector2u& backgroundSize,
			   const Vector2f& resolution,
			   bool clamp=true)
			   	: m_position(position)
				, m_backgroundSize(backgroundSize)
				, m_clampToBackground(clamp)
				, m_tweenX(nullptr)
				, m_tweenY(nullptr) {

	CalculateMinMaxPos(backgroundSize, resolution);

	m_interpolation = InterpFunc::QuartEaseOut;
	m_duration = 1.f;
	m_tweenXActive = false;
	m_tweenYActive = false;
}

/** Deallocates camera's tween objects.
*/
Camera::~Camera() {
	SafeDelete(m_tweenX);
	SafeDelete(m_tweenY);
}

void Camera::InitDefault() {
	m_position.x = 0.f;
	m_position.y = 0.f;
	m_backgroundSize.x = 0.f;
	m_backgroundSize.y = 0.f;

	m_clampToBackground = false;
	m_minX = 0.f;
	m_minY = 0.f;
	m_maxX = 0.f;
	m_maxY = 0.f;

	m_interpolation = InterpFunc::QuartEaseOut;
	m_duration = 1.f;
	m_tweenX = nullptr;
	m_tweenY = nullptr;
	m_tweenXActive = false;
	m_tweenYActive = false;
}

void Camera::ClampPosition(const Vector2f& pos) {
	if (pos.x < m_minX)
		m_position.x = m_minX;
	else if (pos.x > m_maxX)
		m_position.x = m_maxX;
	else
		m_position.x = pos.x;

	if (pos.y < m_minY)
		m_position.y = m_minY;
	else if (pos.y < m_maxY)
		m_position.y = m_maxY;
	else
		m_position.y = pos.y;
}

void Camera::CalculateMinMaxPos(const sf::Vector2u& backgroundSize,
						const sf::Vector2f& resolution) {

	m_minX = resolution.x * .5f;
	m_minY = resolution.y * .5f;

	m_maxX = ((float)backgroundSize.x) - (resolution.x * .5f);
	m_maxY = ((float)backgroundSize.y) - (resolution.y * .5f);
}

void Camera::AnimateTo(const Vector2f& target) {

	// Animate only if there isn't an animation already playing
	if (m_tweenXActive || m_tweenYActive)
		return;

	SpawnTweenX(target.x);
	SpawnTweenY(target.y);
}

void Camera::SpawnTweenX(float targetX) {
	if (targetX < m_minX) targetX = m_minX;
	if (targetX > m_maxX) targetX = m_maxX;

	m_tweenX = new Tween(&m_position.x, m_position.x, targetX, m_duration, m_interpolation);
	m_tweenX->Start();
	m_tweenXActive = true;
}

void Camera::SpawnTweenY(float targetY) {
	if (targetY < m_minY) targetY = m_minY;
	if (targetY > m_maxY) targetY = m_maxY;

	m_tweenY = new Tween(&m_position.y, m_position.y, targetY, m_duration, m_interpolation);
	m_tweenY->Start();
	m_tweenYActive = true;
}

void Camera::ClampTo(const sf::Vector2u& backgroundSize,
		     const sf::Vector2f& resolution) {

	m_clampToBackground = true;
	CalculateMinMaxPos(backgroundSize, resolution);
}

void Camera::IncrementDuration(float i) {
	m_duration += i;
	if (m_duration < .2f)
		m_duration = .2f;
}

// ----------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------

void Camera::SetInterpolation(InterpFunc interp) {
	m_interpolation = interp;
}

InterpFunc Camera::GetInterpolation() const {
	return m_interpolation;
}

Vector2f Camera::GetPosition() const {
	return m_position;
}

void Camera::SetPosition(const Vector2f& pos) {
	if (m_clampToBackground) {
		ClampPosition(pos);
	}
	else {
		m_position = pos;
	}
}

float Camera::GetDuration() const {
	return m_duration;
}

void Camera::SetDuration(float duration) {
	m_duration = duration;
}

// ----------------------------------------------------------------------
// Update
// ----------------------------------------------------------------------

bool Camera::IsAnimating() const {
	return (m_tweenXActive || m_tweenYActive);
}

void Camera::Update(float dt, sf::Vector2f& target) {

	// Deallocate tweens if it has finished animating, otherwise call update()
	if (m_tweenXActive && (!m_tweenX->IsAnimating())) {
		m_tweenXActive = false;
		SafeDelete(m_tweenX);
	}
	else if (m_tweenXActive) {
		m_tweenX->Update(dt);
	}

	if (m_tweenYActive && (!m_tweenY->IsAnimating())) {
		m_tweenYActive = false;
		SafeDelete(m_tweenY);
	}
	else if (m_tweenYActive) {
		m_tweenY->Update(dt);
	}

	// Camera position may be out of bounds of the background
	if (m_clampToBackground) {
		if (m_tweenXActive || m_tweenYActive) {

			float cameraX = 0.f;
            float cameraY = 0.f;

			// Clamp X if interpolated X position is outside of background
			if (m_position.x < m_minX)
				cameraX = m_minX;
			else if (m_position.x > m_maxX)
				cameraX = m_maxX;
			else
				cameraX = m_position.x;

			// Clamp Y if interpolated Y position is outside of background
			if (m_position.y < m_minY)
				cameraY = m_minY;
			else if (m_position.y > m_maxY)
				cameraY = m_maxY;
			else
				cameraY = m_position.y;

			m_position.x = cameraX;
			m_position.y = cameraY;
		}
	}

	// Update camera position based on the player if it's not animating
	if (!m_tweenXActive && !m_tweenYActive) {
		// float playerX = player.getCenter().x;
		// float playerY = player.getCenter().y;

		float playerX = target.x;
		float playerY = target.y;

		bool clampOnMinX = false;
		bool clampOnMaxX = false;
		bool clampOnMinY = false;
		bool clampOnMaxY = false;

		if (playerX < m_minX)      clampOnMinX = true;
		else if (playerX > m_maxX) clampOnMaxX = true;

		if (playerY < m_minY)	   clampOnMinY = true;
		else if (playerY > m_maxY) clampOnMaxY = true;

		if (clampOnMinX)
		{
			m_position.x = m_minX;
			target.x = m_minX;
		}
		else if (clampOnMaxX)
		{
			m_position.x = m_maxX;
			target.x = m_maxX;

		}
		else
			m_position.x = playerX;

		if (clampOnMinY) {
			m_position.y = m_minY;
			target.y = m_minY;
		}
		else if (clampOnMaxY)
		{
			m_position.y = m_maxY;
			target.y = m_maxY;
		}
		else
			m_position.y = playerY;
	}
}