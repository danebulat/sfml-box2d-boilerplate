#ifndef CORNER_HANDLE_HPP
#define CORNER_HANDLE_HPP

#include <SFML/Graphics.hpp>

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
	CornerHandle(const sf::FloatRect& zoneBounds);
	~CornerHandle();

	void Update(const sf::FloatRect& zoneBounds);
	void Draw(sf::RenderWindow& window);

	bool MouseHoveringOnHandle(const sf::Vector2f& mousePos);
	void IncrementPosition(const sf::Vector2f& increment);
};

#endif