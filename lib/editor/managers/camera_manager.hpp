#ifndef CAMERA_MANAGER_HPP
#define CAMERA_MANAGER_HPP

#include "editor/animation/camera.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/constants.hpp"

class CameraManager
{
private:
	std::shared_ptr<Camera> m_camera;

	sf::View 		m_view;
	sf::Vector2f	m_cameraTarget;

	sf::Vector2f	m_prevMousePos;
	bool			m_rmbPressed;
	bool 			m_panCamera;

public:
	CameraManager();
	~CameraManager();

	CameraManager(const CameraManager&) = delete;
	CameraManager& operator= (const CameraManager&) = delete;

	void HandleInput(const sf::Event& event);
	void Update(sf::RenderWindow& window, sf::Time& dt);

	sf::View& GetCameraView();
	std::shared_ptr<Camera> GetCamera();
};

#endif