#include "editor/callbacks/trigger_zone.hpp"

/** MyQueryCallback::ReportFixture
 *
 * Return true to continue the query, otherwise return false.
 * Call with world->Query(&callback, aabb)
 */

bool MyQueryCallback::ReportFixture(b2Fixture* fixture)
{
	std::cout << "MyQueryCallback::ReportFixture - Entered AABB\n";
	b2Body* body = fixture->GetBody();
	body->SetAwake(true);
	return true;
}

/** TriggerZone
 *
 * A visual representation of a Box2D query AABB. A b2QueryCallback derived
 * class is associated with the visual rectangle.
*/

TriggerZone::TriggerZone(const sf::Vector2f& position, const sf::Vector2f& size)
{
	/* Initialise mouse-dragging data members */
	m_leftMouseDown = false;
	m_mouseMoved = false;
	m_hoveringOnZone = false;
	m_hoveringOnCornerHandle = false;

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

	/* Create corner handle based on zone's position */
	m_cornerHandle.reset(new CornerHandle(m_sprite.getGlobalBounds()));
}

TriggerZone::~TriggerZone()
{}

bool TriggerZone::MouseHoveringOnZone(const sf::Vector2f& mousePos)
{
	sf::FloatRect r(m_position.x, m_position.y, m_size.x, m_size.y);

	if ((mousePos.x > r.left && mousePos.x < r.left + r.width) &&
		(mousePos.y > r.top && mousePos.y < r.top + r.height))
	{
		return true;
	}

	return false;
}

void TriggerZone::Query(b2World* world)
{
	b2AABB aabb;
	aabb.lowerBound = m_lower;
	aabb.upperBound = m_upper;

	// Query b2World
	world->QueryAABB(&m_callback, aabb);
}

void TriggerZone::HandleInput(const sf::Event& event)
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

void TriggerZone::Update(sf::RenderWindow& window)
{
	auto mousePos = GetMousePosition(window);

	// Set hover color
	if (m_hoveringOnZone)
	{
		m_color.a = 48.f;
		m_sprite.setFillColor(m_color);
	}
	else
	{
		m_color.a = 64.f;
		m_sprite.setFillColor(m_color);
	}

	// First handle the corner handle being dragged
	if (m_hoveringOnCornerHandle && m_leftMouseDown)
	{
		// Cache object pointer as the object getting dragged
		s_ObjectBeingDragged = this;

		// Get move increment
		sf::Vector2f moveIncrement = GetIncrement(m_prevMousePos, mousePos);

		// TODO: Resize lower and upper bounds
		m_size += moveIncrement;
		m_lower.Set(m_position.x/SCALE, m_position.y/SCALE);
		m_upper.Set((m_position.x + m_size.x)/SCALE, (m_position.y + m_size.y)/SCALE);

		// Resize sprite
		m_sprite.setSize(m_sprite.getSize() + moveIncrement);

		// Move corner handle
		m_cornerHandle->IncrementPosition(moveIncrement);
	}

	// Handle dragging the zone with the mouse
	else if (ZoneBeingDraggedByMouse() && Draggable::CanDrag(this))
	{
		// Cache object pointer as the object getting dragged
		s_ObjectBeingDragged = this;

		// Get move increment
		sf::Vector2f moveIncrement = GetIncrement(m_prevMousePos, mousePos);

		// Update AABB data
		m_position += moveIncrement;

		m_lower.Set(m_position.x/SCALE, m_position.y/SCALE);
		m_upper.Set((m_position.x + m_size.x)/SCALE, (m_position.y + m_size.y)/SCALE);

		// Update sprite position
		m_sprite.setPosition(m_position);

		// Update CornerHandle position
		m_cornerHandle->Update(m_sprite.getGlobalBounds());
	}

	// Detect if mouse is hovering over the trigger zone
	if (m_mouseMoved)
	{
		if (m_cornerHandle->MouseHoveringOnHandle(mousePos))
			m_hoveringOnCornerHandle = true;
		else
			m_hoveringOnCornerHandle = false;

		if (MouseHoveringOnZone(mousePos))
			m_hoveringOnZone = true;
		else
			m_hoveringOnZone = false;
	}

	m_prevMousePos = mousePos;
}

bool TriggerZone::ZoneBeingDraggedByMouse()
{
	return (m_hoveringOnZone && m_leftMouseDown);
}

void TriggerZone::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
	m_cornerHandle->Draw(window);
}

/** Implement Draggable Interface
 */

void TriggerZone::UpdateDragCache()
{
	if (!ZoneBeingDraggedByMouse())
	{
		s_ObjectBeingDragged = nullptr;
	}
}