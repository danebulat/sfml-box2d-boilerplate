#ifndef BOX2D_UTILS_HPP
#define BOX2D_UTILS_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "box2d/box2d.h"
#include "utils/constants.hpp"
#include "utils/mouse_utils.hpp"
#include "utils/custom_polygon.hpp"

using sf::Vector2f;
using sf::RectangleShape;
using sf::CircleShape;
using sf::RenderWindow;

namespace demo_data
{
	// edge chains and custom polygon coordinates
	extern std::vector<Vector2f> newChainCoords;
	extern std::vector<Vector2f> coords;
	extern std::vector<Vector2f> coordsLeft;
	extern std::vector<Vector2f> coordsRight;
	extern std::vector<Vector2f> customPolygonCoords;
}

namespace physics
{
using namespace ::sf;
using namespace ::demo_data;

/** DoTestPoint
 *      b2Fixture*    - The shape to test against
 *      sf::Vector2f& - The position where mouse was clicked
 *      sf::Shape&    - The shape to change color
 *
 * 	Checks if the passed point is in contact with a shape, and
 *  changes the passed sprite's color of there is a collision.
 */

void DoTestPoint(b2Fixture* fixture, const Vector2f& clickPos,
		RectangleShape& sprite);

void DoTestPoint(b2Fixture* fixture, const Vector2f& clickPos,
		CircleShape& sprite);

/** DoRayCast
 *      b2Fixture*        - The shape to test against
 *      sf::Vector2f&     - The ray's end point
 *      sf::RenderWindow& - The window to draw collision points
 *
 * 	Cast a ray from the bottom left hand corner of the window to the passed
 *  end point. Draw the first point of contact on the passed fixture.
 */

void DoRayCast(b2Fixture* fixture, Vector2f& rayEndPoint,
	RenderWindow& window);

/** CreateCustomPolygon
 *
 * 	Create a CustomPolygon object and add to vector.
 */

void CreateCustomPolygon(b2World* world,
	std::vector<CustomPolygon>& collection,
	bool wireframe,
	unsigned int& counter,
	RenderWindow& window);

/** CreateBox
 *      b2World& - The world to add box to
 *      int		 - Box starting X position
 *      int      - Box starting Y position
 *
 * 	Create a dynamic body with an associated box polgyon shape and add
 *  it to the b2World.
 */

void CreateBox(b2World& world, float mouseX, float mouseY);

/** CreateCircle
 *      b2World& - The world to add box to
 *      int		 - Circle starting X position
 *      int      - Circle starting Y position
 *
 * 	Create a dynamic body with an associated circle shape and add
 *  it to the b2World.
 */

void CreateCircle(b2World& world, float mouseX, float mouseY);

/** CreateGround
 *      b2World& - The world to add ground to
 *      int		 - Ground X position
 *      int      - Ground Y position
 *
 * 	Create a static body with an associated box shape and add
 *  it to the b2World.
 */

void CreateGround(b2World& world, float x, float y);

/** RemoveExpiredCustomPolygons
 * 		std::vector<CustomPolygon> - The CustomPolygon collection
 *      b2World* 				   - Box2D world
 *
 * 	Deletes expired CustomPolygon objects and destroys their
 *  associated Box2D object.
 */

void RemoveExpiredCustomPolygons(std::vector<CustomPolygon>& polygons,
	b2World* world, unsigned int& counter);

/** RemoveDynamicBodies
 *
 * 	Destroys all dynamic bodies in the b2World, including
 *  CustomPolygon objects.
 */

void RemoveDynamicBodies(b2World* world, unsigned int& counter,
	std::vector<CustomPolygon>& customPolygons);

/** RemoveOffScreenDynamicBodies
 *
 * 	Destroys all dynamic bodies in the b2World, including
 *  CustomPolygon objects.
 */
void RemoveOffScreenDynamicBodies(b2World* world, unsigned int& counter);

} // namespace physics

#endif