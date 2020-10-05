#ifndef DEBUG_BOX_HPP
#define DEBUG_BOX_HPP

#include <box2d/box2d.h>
#include "editor/debug/debug_shape.hpp"

class DebugBox : public DebugShape
{
private:
	sf::RectangleShape	m_sprite;
	float 				m_size;

	b2Body*				m_body;
	b2World*			m_world;

	bool 				m_testPointActive;

private:
	void CreateBody();

public:
	DebugBox(const sf::Vector2f& position, b2World* world);
	virtual ~DebugBox();

	void DeleteBody();

	virtual void Update() override;
	virtual void Draw(sf::RenderWindow& window) override;

	std::string* GetUserData();
	sf::Vector2f GetPosition() const;

	void DoTestPoint(const sf::Vector2f& point);
	void ResetTestPoint();

	// TMP
	void ApplyForce();
	void ApplyLinearImpulse();
	void ApplyTorque();
	void ApplyAngularImpulse();
	// END TMP
};

#endif