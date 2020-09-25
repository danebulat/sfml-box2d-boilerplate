#include "utils/mouse_utils.hpp"

using sf::Mouse;
using sf::RenderWindow;
using sf::Vector2f;
using sf::Vector2i;
using sf::Text;
using std::string;

/** Get mouse coordinates relative to SFML window.
 */
sf::Vector2f GetMousePosition(const RenderWindow& window)
{
	Vector2f mousePosition = window.mapPixelToCoords(Mouse::getPosition(), window.getView());
	Vector2i windowOffset = window.getPosition();

	return Vector2f(mousePosition.x - (float)windowOffset.x,
					mousePosition.y - (float)windowOffset.y);
}

/** Renders a label next to the mouse cursor in the SFML window.
 */
void SetMouseLabel(Text& label, RenderWindow& window)
{
	Vector2f pos = (GetMousePosition(window) + sf::Vector2f(10.f, -15.f));
	string xPos = std::to_string(pos.x);
	string yPos = std::to_string(pos.y);

	xPos = xPos.substr(0, 5);
	yPos = yPos.substr(0, 5);
	string str = "(" + xPos + "," + yPos + ")";

	label.setPosition(pos);
	label.setString(str);
}

/** Calculate difference vector between the current and previous
 *  mouse position
 */

Vector2f GetIncrement(const Vector2f& prev, const Vector2f& cur)
{
	return Vector2f(cur.x - prev.x, cur.y - prev.y);
}