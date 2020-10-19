#ifndef TRIGGER_ZONE_HPP
#define TRIGGER_ZONE_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/draggable.hpp"
#include "editor/callbacks/corner_handle.hpp"

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