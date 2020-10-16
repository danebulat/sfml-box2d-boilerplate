#ifndef TRIGGER_ZONE_HPP
#define TRIGGER_ZONE_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/mouse_utils.hpp"

/** AABB Query
 *
 * Sometimes you want to determine all shapes in a region - you can provide
 * an AABB in world coordinates and an implementation of b2QueryCallback.
 *
 * The world calls your class with each fixture whose AABB overlaps the
 * query AABB.
 */

class MyQueryCallback : public b2QueryCallback
{
public:

	/** ReportFixture
	 *
	 * Return true to continue the query, otherwise return false.
	 * Call with world->Query(&callback, aabb)
	 */
	bool ReportFixture(b2Fixture* fixture) override
	{
		std::cout << "MyQueryCallback::ReportFixture - Entered AABB\n";
		b2Body* body = fixture->GetBody();
		body->SetAwake(true);
		return true;
	}
};

/** TriggerZone
 *
 * A visual representation of a Box2D query AABB. A b2QueryCallback derived
 * class is associated with the visual rectangle.
*/

class TriggerZone
{
private:
	sf::RectangleShape	m_sprite;
	sf::Color			m_color;
	sf::Vector2f		m_position;
	sf::Vector2f		m_size;

	b2Vec2				m_lower;
	b2Vec2				m_upper;

	// TODO: Make this a polymorphic pointer to assign custom callback objects
	MyQueryCallback		m_callback;

	// Dragging data members
	bool    		m_leftMouseDown;
	bool 			m_mouseMoved;
	bool			m_hoveringOnZone;
	sf::Vector2f	m_currMousePos;
	sf::Vector2f	m_prevMousePos;

public:
	TriggerZone(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		/* Initialise mouse-dragging data members */
		m_leftMouseDown = false;
		m_mouseMoved = false;
		m_hoveringOnZone = false;

		/* Initialise zone data members */
		m_position = position;
		m_size = size;
		m_color = sf::Color(0.f, 0.f, 255.f, 64.f);

		m_sprite.setSize(size);
		m_sprite.setPosition(position);
		m_sprite.setFillColor(m_color);

		m_lower.Set(m_position.x/SCALE, m_position.y/SCALE);
		m_upper.Set((m_position.x + m_size.x)/SCALE,
					(m_position.y + m_size.y)/SCALE);
	}

	bool MouseHoveringOnZone(const sf::Vector2f& mousePos)
	{
		sf::FloatRect r(m_position.x, m_position.y, m_size.x, m_size.y);

		if ((mousePos.x > r.left && mousePos.x < r.left + r.width) &&
			(mousePos.y > r.top && mousePos.y < r.top + r.height))
		{
			return true;
		}

		return false;
	}

	void Query(b2World* world)
	{
		b2AABB aabb;
		aabb.lowerBound = m_lower;
		aabb.upperBound = m_upper;

		// Query b2World
		world->QueryAABB(&m_callback, aabb);
	}

	void HandleInput(const sf::Event& event)
	{
		// Mouse Button Pressed
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_leftMouseDown = true;
			}
		}

		// Mouse move
		if (event.type == sf::Event::MouseMoved)
		{
			m_mouseMoved = true;
		}
		else
		{
			m_mouseMoved = false;
		}

		// Mouse Button Released
		if (event.type == sf::Event::MouseButtonReleased)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_leftMouseDown = false;
			}
		}
	}

	void Update(sf::RenderWindow& window)
	{
		auto mousePos = GetMousePosition(window);

		// Handle dragging the zone with the mouse
		if (m_hoveringOnZone && m_leftMouseDown)
		{
			// TODO: Set hover color

			// Get move increment
			sf::Vector2f moveIncrement = GetIncrement(m_prevMousePos, mousePos);

			// Update AABB data
			m_position += moveIncrement;

			m_lower.Set(m_position.x/SCALE, m_position.y/SCALE);
			m_upper.Set((m_position.x + m_size.x)/SCALE, (m_position.y + m_size.y)/SCALE);

			// Update sprite position
			m_sprite.setPosition(m_position);
		}

		// Detect if mouse is hovering over the trigger zone
		if (m_mouseMoved)
		{
			if (MouseHoveringOnZone(mousePos))
			{
				m_hoveringOnZone = true;
			}
			else
			{
				m_hoveringOnZone = false;
			}
		}

		m_prevMousePos = mousePos;
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_sprite);
	}
};

#endif