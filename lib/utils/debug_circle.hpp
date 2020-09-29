#ifndef DEBUG_CIRCLE_HPP
#define DEBUG_CIRCLE_HPP

#include <box2d/box2d.h>
#include "utils/debug_shape.hpp"
#include "utils/constants.hpp"
#include "utils/box2d_utils.hpp"

class DebugCircle : public DebugShape
{
private:
	sf::CircleShape m_sprite;
	float 			m_radius;

	b2Body*			m_body;
	b2World* 		m_world;

private:
	void CreateBody();

public:
	DebugCircle(const sf::Vector2f& position,
		b2World* world);

	virtual ~DebugCircle();
	void DeleteBody();

	virtual void Update() override;
	virtual void Draw(sf::RenderWindow& window) override;

	std::string* GetUserData();

	void DoTestPoint(const sf::Vector2f& point);
	void ResetTestPoint();
};

#endif