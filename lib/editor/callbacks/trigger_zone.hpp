#ifndef TRIGGER_ZONE_HPP
#define TRIGGER_ZONE_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/draggable.hpp"

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
	bool ReportFixture(b2Fixture* fixture) override;
};

/** CornerHandle
 *
 * A handle displayed on the bottom-right hand corner of a trigger zone.
 * The handle is dragged to resize the trigger zone.
*/

class CornerHandle
{
private:
	sf::RectangleShape	m_sprite;
	sf::Vector2f		m_position;

	/* Static data */
	static constexpr float LENGTH = 17.5f;

public:
	CornerHandle(const sf::FloatRect& zoneBounds)
	{
		/* Initialise sprite size and color */
		m_sprite.setSize(sf::Vector2f(LENGTH, LENGTH));
		m_sprite.setFillColor(sf::Color(0.f, 0.f, 255.f, 96.f));

		/* Initialise sprite position */
		m_position.x = (zoneBounds.left + zoneBounds.width) - LENGTH;
		m_position.y = (zoneBounds.top + zoneBounds.height) - LENGTH;
		m_sprite.setPosition(m_position);
	}

	virtual ~CornerHandle()
	{}

	void Update(const sf::FloatRect& zoneBounds)
	{
		m_position.x = (zoneBounds.left + zoneBounds.width) - LENGTH;
		m_position.y = (zoneBounds.top + zoneBounds.height) - LENGTH;
		m_sprite.setPosition(m_position);
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_sprite);
	}

	bool MouseHoveringOnHandle(const sf::Vector2f& mousePos)
	{
		sf::FloatRect r(m_position.x, m_position.y, LENGTH, LENGTH);

		if ((mousePos.x > r.left && mousePos.x < r.left + r.width) &&
			(mousePos.y > r.top && mousePos.y < r.top + r.height))
		{
			return true;
		}

		return false;
	}

	void IncrementPosition(const sf::Vector2f& increment)
	{
		m_position += increment;
		m_sprite.setPosition(m_position);
	}
};

/** TriggerZone
 *
 * A visual representation of a Box2D query AABB. A b2QueryCallback derived
 * class is associated with the visual rectangle.
*/

class TriggerZone : public Draggable
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
	bool			m_hoveringOnCornerHandle;
	sf::Vector2f	m_currMousePos;
	sf::Vector2f	m_prevMousePos;

	/* Corner handle to resize zone */
	std::unique_ptr<CornerHandle> m_cornerHandle;

	static constexpr float MIN_LENGTH = 35.f;

public:
	TriggerZone(const sf::Vector2f& position, const sf::Vector2f& size);
	virtual ~TriggerZone();

	bool MouseHoveringOnZone(const sf::Vector2f& mousePos);
	bool ZoneBeingDraggedByMouse();

	void Query(b2World* world);

	void HandleInput(const sf::Event& event);
	void Update(sf::RenderWindow& window);
	void Draw(sf::RenderWindow& window);

	/** Implement Draggable Interface
	 */

	virtual void UpdateDragCache() override;

private:
	/* Private accessors for CornerHandle class */
	sf::FloatRect GetFloatRect() const
	{
		return m_sprite.getGlobalBounds();
	}
};

#endif