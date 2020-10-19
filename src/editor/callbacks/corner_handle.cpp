#include "editor/callbacks/corner_handle.hpp"

/** Constructor **/
CornerHandle::CornerHandle(const sf::FloatRect& zoneBounds)
{
	/* Initialise sprite size and color */
	m_sprite.setSize(sf::Vector2f(LENGTH, LENGTH));
	m_sprite.setFillColor(sf::Color(0.f, 0.f, 255.f, 96.f));

	/* Initialise sprite position */
	m_position.x = (zoneBounds.left + zoneBounds.width) - LENGTH;
	m_position.y = (zoneBounds.top + zoneBounds.height) - LENGTH;
	m_sprite.setPosition(m_position);
}

/** Destructor **/
CornerHandle::~CornerHandle()
{}

/** Update **/
void CornerHandle::Update(const sf::FloatRect& zoneBounds)
{
	m_position.x = (zoneBounds.left + zoneBounds.width) - LENGTH;
	m_position.y = (zoneBounds.top + zoneBounds.height) - LENGTH;
	m_sprite.setPosition(m_position);
}

/** Draw **/
void CornerHandle::Draw(sf::RenderWindow& window)
{
	window.draw(m_sprite);
}

/** MouseHoveringOnHandle **/
bool CornerHandle::MouseHoveringOnHandle(const sf::Vector2f& mousePos)
{
	sf::FloatRect r(m_position.x, m_position.y, LENGTH, LENGTH);

	if ((mousePos.x > r.left && mousePos.x < r.left + r.width) &&
		(mousePos.y > r.top && mousePos.y < r.top + r.height))
	{
		return true;
	}

	return false;
}

/** IncrementPosition **/
void CornerHandle::IncrementPosition(const sf::Vector2f& increment)
{
	m_position += increment;
	m_sprite.setPosition(m_position);
}