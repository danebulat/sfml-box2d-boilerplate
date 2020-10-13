#ifndef DEMO_STRUCTS_HPP
#define DEMO_STRUCTS_HPP

#include "editor/constants.hpp"

/** BoxSprite Struct
 */

struct BoxSprite
{
	sf::RectangleShape	m_sprite;
	sf::Vector2f 		m_position;

	BoxSprite()
	{
		m_sprite.setFillColor(sf::Color::White);
		m_sprite.setOutlineColor(sf::Color::Black);
		m_sprite.setOutlineThickness(2.f);
		m_sprite.setSize(sf::Vector2f(20.f, 20.f));
		m_sprite.setOrigin(10.f, 10.f);
	}

	void SetPosition(const sf::Vector2f& position)
	{
		m_position = position;
		m_sprite.setPosition(m_position);
	}
};

/** CircleSprite Struct
 */

struct CircleSprite
{
	sf::CircleShape m_sprite;
	sf::Vector2f	m_position;

	CircleSprite()
	{
		m_sprite.setFillColor(sf::Color::White);
		m_sprite.setOutlineColor(sf::Color::Black);
		m_sprite.setOutlineThickness(2.f);
		m_sprite.setRadius(CIRCLE_RADIUS);
		m_sprite.setOrigin(CIRCLE_RADIUS, CIRCLE_RADIUS);
	}

	void SetPosition(const sf::Vector2f& position)
	{
		m_position = position;
		m_sprite.setPosition(m_position);
	}

	bool intersects(const sf::Vector2f& point)
	{
		auto bb = m_sprite.getGlobalBounds();

		if ((point.x > bb.left) && (point.x < bb.left + bb.width) &&
			(point.y > bb.top)  && (point.y < bb.top + bb.height))
		{
			return true;
		}
		return false;
	}
};

#endif