#ifndef BOX2D_UTILS_HPP
#define BOX2D_UTILS_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/debug/custom_polygon.hpp"

using sf::Vector2f;
using sf::RectangleShape;
using sf::CircleShape;
using sf::RenderWindow;

template<class T>
void SafeDelete(T*& pVal) {
	if (pVal != nullptr) {
		delete pVal;
		pVal = nullptr;
	}
}

template<class T>
void SafeDeleteArray (T*& pVal) {
    if (pVal != nullptr) {
		delete [] pVal;
		pVal = nullptr;
	}
}

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

/** Traverse bodies
 *
 * Debugging function that counts the amount of bodies, dynamic bodies
 * and fixtures in the passed Box2D world.
 */
void TraverseWorldBodies(b2World* world);

/** b2ContactListener Example
*/
class MyContactListener : public b2ContactListener
{
	/**
	 * NOTE: Instantiate and register with b2World::SetContactListener.
	 *
	 * NOTE: Can use for sounds and game logic.
	 *
	 * NOTE: You cannot alter the physics world inside a callback as you may
	 *       delete objects Box2D is processing.
	 *
	 * NOTE: Recommended practice for processing contact points: Buffer all
	 *       contact data that you care about and process it after the time step.
	 *
	 * NOTE: Process contact points immediately after time step (some other client
	 *       code may alter the physics world)
	 */

private:
	bool m_beginContactOutput = false;
	bool m_endContactOutput = false;
	bool m_preSolveOutput = false;
	bool m_postSolveOutput = false;

	void OutputContactInfo(b2Contact* contact, const char* eventType)
	{
		std::cout << "---------------------------------------------\n"
			"b2ContactListener::" << eventType << "\n"
			"  GetFriction()    - " << contact->GetFriction() << "\n"
			"  GetRestitution() - " << contact->GetRestitution() << "\n"
			"    FixtureA->GetFriction() - " << contact->GetFixtureA()->GetFriction() << "\n"
			"    FixtureA->GetDensity()  - " << contact->GetFixtureA()->GetDensity() << "\n"
			"    FixtureB->GetFriction() - " << contact->GetFixtureB()->GetFriction() << "\n"
			"    FixtureB->GetDensity()  - " << contact->GetFixtureB()->GetDensity() << "\n\n";
	}

public:

	/** Beging event
	 * Called when two fixtures begin to overlap. Called for sensors
	 * and non-sensors.
	 */
	void BeginContact(b2Contact* contact) override
	{
		if (m_beginContactOutput)
			OutputContactInfo(contact, "BeginContact");
	}

	/** End event
	 * Called when two fixtures cease to overlap. Called for sensors
	 * and non-sensors.
	*/
	void EndContact(b2Contact* contact) override
	{
		if (m_endContactOutput)
			OutputContactInfo(contact, "EndContact");
	}

	/** Pre-solve event
	 * Called after collision detection, but before collision resolution.
	 * - Gives you a chance to disable a contact based on your current configuration.
	 * - May be fired multiple times per time step per contact due to continuous
	 *   collision detection.
	*/
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
	{
		if (m_preSolveOutput)
		{
			OutputContactInfo(contact, "PreSolve");
		}

		// Get contact point world coordinates
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		// Get states of old and current contact points
		b2PointState state1[2], state2[2];
		b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());

		// b2_addState - point was added in the update
		// https://box2d.org/documentation/b2__collision_8h.html#a0a894e3715ce8c61b7958dd6e083663d
    	if (state2[0] == b2_addState)
		{
			const b2Body* bodyA = contact->GetFixtureA()->GetBody();
        	const b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// Get first contact point
        	b2Vec2 point = worldManifold.points[0];

			// Get velocity vectors from both contact bodies
			b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
			b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);

			// Calculate approach velocity
			float approachVelocity = b2Dot(vB - vA, worldManifold.normal);

			std::cout << "> approachVelocity: " << approachVelocity << "\n";

			// Dot product > 1 means vectors point in same direction
			// https://www.mathsisfun.com/algebra/vectors-dot-product.html
			if (approachVelocity > 1.0f)
			{
				/* MyPlayCollisionSound(); */
				std::cout << "> play collision sound\n";
			}
		}
	}

	/** Post-solve event
	 * Where you can gather collision impulse results.
	 * - Implement PreSolve() if you don't need impulses.
	 */
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
	{
		if (m_postSolveOutput)
		{
			OutputContactInfo(contact, "PostSolve");
		}

		contact->SetEnabled(false);
	}
};

} // namespace physics

#endif