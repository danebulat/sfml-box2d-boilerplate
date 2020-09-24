#ifndef MOUSE_UTILS_HPP
#define MOUSE_UTILS_HPP

#include <SFML/Graphics.hpp>

/** GetMousePosition
 *      sf::RenderWindow& - Window to test mouse position
 *
 * 	Get mouse position relative to the window and not the screen.
 */

sf::Vector2f GetMousePosition(const sf::RenderWindow& window);

#endif