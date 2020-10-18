#include "editor/managers/camera_manager.hpp"

CameraManager::CameraManager()
{
	m_rmbPressed = false;
	m_panCamera = false;

	sf::Vector2f res = EditorSettings::RESOLUTION;

	m_view.setSize(res.x, res.y);
	m_cameraTarget.x = res.x * 0.5f;
	m_cameraTarget.y = res.y * 0.5f;

	m_camera.reset(new Camera(
		m_cameraTarget, EditorSettings::levelSize, res, true));
	m_camera->SetDuration(0.5f);
	m_camera->SetInterpolation(InterpFunc::ExpoEaseOut);
}

CameraManager::~CameraManager()
{}

void CameraManager::HandleInput(const sf::Event& event)
{
	// Mouse Button Pressed
	if (event.type == sf::Event::MouseButtonPressed)
	{
		// RMB Pressed
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			if (EditorSettings::mode == RMBMode::PanCameraMode)
				m_rmbPressed = true;
		}
	}

	// Mouse move
	if (event.type == sf::Event::MouseMoved)
	{
		// Handle camera pan
		if (EditorSettings::mode == RMBMode::PanCameraMode && m_rmbPressed)
		{
			m_panCamera = true;
		}
	}

	// Right button release
	if (event.type == sf::Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == sf::Mouse::Right)
		{
			m_rmbPressed = false;
			m_panCamera = false;
		}
	}
}

void CameraManager::Update(sf::RenderWindow& window, sf::Time& dt)
{
	/* WEIRD CODE ****************************************************************/
	// By simply calculating the increment of the current/previous mouse position
	// each frame results in strange behavior due to the way the view is centered
	// every frame.
	// The following solution updates the current mouse position WITH THE INCREMENT
	// value BEFORE copying it over to the previous frame mouse position.
	sf::Vector2f mousePos = GetMousePosition(window);

	if (m_panCamera)
	{
		float incX = abs(mousePos.x - m_prevMousePos.x);
		float incY = abs(mousePos.y - m_prevMousePos.y);

		if (mousePos.x < m_prevMousePos.x) {
			m_cameraTarget.x += incX;
			mousePos.x += incX;
		}
		else {
			// move negative x
			m_cameraTarget.x -= incX;
			mousePos.x -= incX;
		}

		if (mousePos.y < m_prevMousePos.y) {
			m_cameraTarget.y += incY;
			mousePos.y += incY;
		}
		else {
			// move negative y
			m_cameraTarget.y -= incY;
			mousePos.y -= incY;
		}
	}

	m_prevMousePos = mousePos;
	/* END WEIRD CODE ************************************************************/

	/* Center view on camera position */
	if (m_camera->IsAnimating())
	{
		m_cameraTarget = m_camera->GetPosition();
	}

	/** Update camera */
	m_camera->Update(dt.asSeconds(), m_cameraTarget);

	/* Sync center of view with camera position */
	m_view.setCenter(m_camera->GetPosition());
}

sf::View& CameraManager::GetCameraView()
{
	return m_view;
}

std::shared_ptr<Camera> CameraManager::GetCamera()
{
	return m_camera;
}