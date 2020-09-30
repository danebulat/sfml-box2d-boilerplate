#ifndef MOUSE_UTILS_HPP
#define MOUSE_UTILS_HPP

#include <SFML/Graphics.hpp>

/** GetMousePosition
 *      sf::RenderWindow& - Window to test mouse position
 *
 * 	Get mouse position relative to the window and not the screen.
 */

sf::Vector2f GetMousePosition(const sf::RenderWindow& window);


/** Renders a label next to the mouse cursor in the SFML window.
 */
void SetMouseLabel(sf::Text& label, sf::RenderWindow& window);

/** Calculate difference vector between the current and previous
 *  mouse position
 */

sf::Vector2f GetIncrement(const sf::Vector2f& prev, const sf::Vector2f& cur);

#endif