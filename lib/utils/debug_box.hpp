#ifndef DEBUG_BOX_HPP
#define DEBUG_BOX_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "utils/constants.hpp"

class DebugBox
{
private:
	sf::RectangleShape	m_sprite;
	sf::Vector2f 		m_position;
	std::string* 		m_tag;

	float 				m_size;
	bool 				m_markedForDelete;

	b2Body*				m_body;
	b2World*			m_world;

private:
	void CreateBody();

public:
	static unsigned int BodyCount;

	DebugBox(const sf::Vector2f& position, b2World* world);
	~DebugBox();

	void DeleteBody();

	void Update();
	void Draw(sf::RenderWindow& window);

	std::string* GetUserData();
	sf::Vector2f GetPosition() const;

	void MarkForDelete(bool flag);
	bool IsMarkedForDelete() const;
};

#endif