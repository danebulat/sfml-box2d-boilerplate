#include "utils/mouse_utils.hpp"

sf::Vector2f GetMousePosition(const sf::RenderWindow& window)
{
	sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(), window.getView());
	sf::Vector2i windowOffset = window.getPosition();

	return sf::Vector2f(mousePosition.x - (float)windowOffset.x,
						mousePosition.y - (float)windowOffset.y);
}