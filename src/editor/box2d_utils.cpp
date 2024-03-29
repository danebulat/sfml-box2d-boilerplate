#include "editor/box2d_utils.hpp"

namespace demo_data
{
	// new edge chain
	std::vector<sf::Vector2f> newChainCoords = {
		sf::Vector2f(-100.f, 0.f),
		sf::Vector2f(-50.f,  0.f),
		sf::Vector2f( 0.f,   0.f),
		sf::Vector2f( 50.f,  0.f),
		sf::Vector2f( 100.f, 0.f)
	};

	// edge chain central
	std::vector<sf::Vector2f> coords =  {
		sf::Vector2f(70.f,  590.f),
		sf::Vector2f(200.f, 520.f),
		sf::Vector2f(410.f, 557.f),
		sf::Vector2f(670.f, 485.f),
		sf::Vector2f(800.f, 590.f)
	};

	// edge chain left
	std::vector<sf::Vector2f> coordsLeft = {
		sf::Vector2f(72.f,  85.f),
		sf::Vector2f(105.f, 280.f),
		sf::Vector2f(70.f, 590.f)
	};

	// edge chain right
	std::vector<sf::Vector2f> coordsRight = {
		sf::Vector2f(800.f, 590.f),
		sf::Vector2f(772.f, 280.f),
		sf::Vector2f(828.f, 85.f)
	};

	// custom polygon
	std::vector<sf::Vector2f> customPolygonCoords = {
		sf::Vector2f(10.f, 10.f),
		sf::Vector2f(10.f, 0.f),
		sf::Vector2f(0.f, -30.f),
		sf::Vector2f(-10.f, 0.f),
		sf::Vector2f(-10.f, 20.f)
	};
}

namespace physics
{

using namespace ::sf;
using namespace ::demo_data;

/** DoTestPoint
 *  Checks if the passed point is in contact with a shape, and
 *  changes the passed sprite's color of there is a collision.
 */

void DoTestPoint(b2Fixture* fixture, const sf::Vector2f& clickPos,
		sf::RectangleShape& sprite)
{
	b2Vec2 scaledPoint(clickPos.x/SCALE, clickPos.y/SCALE);

	if (fixture->TestPoint(scaledPoint))
	{
		sprite.setFillColor(sf::Color::Green);
	}
}

void DoTestPoint(b2Fixture* fixture, const sf::Vector2f& clickPos,
		sf::CircleShape& sprite)
{
	b2Vec2 scaledPoint(clickPos.x/SCALE, clickPos.y/SCALE);

	if (fixture->TestPoint(scaledPoint))
	{
		sprite.setFillColor(sf::Color::Blue);
	}
}

/** DoRayCast
 * 	Cast a ray from the bottom left hand corner of the window to the passed
 *  end point. Draw the first point of contact on the passed fixture.
 */

void DoRayCast(b2Fixture* fixture, sf::Vector2f& rayEndPoint, sf::RenderWindow& window)
{
	// Get the child shape class associated with the fixture
	b2Shape* shape = nullptr;

	if (fixture->GetType() == b2Shape::e_polygon)
	{
		shape = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
	}
	else
	{
		shape = dynamic_cast<b2CircleShape*>(fixture->GetShape());
	}

	// Set up ray cast data
	b2Transform transform = fixture->GetBody()->GetTransform();
	b2RayCastInput input;
	input.p1.Set(0.f/SCALE, 600.f/SCALE);
	input.p2.Set(rayEndPoint.x/SCALE, rayEndPoint.y/SCALE);
	input.maxFraction = 1.f;
	int32 childIndex = 0;

	b2RayCastOutput output;

	bool hit = shape->RayCast(&output, input, transform, childIndex);

	// Handle the hit
	if (hit)
	{
		b2Vec2 hitPoint = input.p1 + output.fraction * (input.p2 - input.p1);

		sf::CircleShape point;
		point.setRadius(5.f);
		point.setFillColor(sf::Color::Red);
		point.setOrigin(sf::Vector2(5.f, 5.f));
		point.setPosition(hitPoint.x * SCALE, hitPoint.y * SCALE);

		window.draw(point);
	}
}

/** (Deprecated) CreateCustomPolygon
 * 	Create a CustomPolygon object and add to vector.
 */

// void CreateCustomPolygon(b2World* world,
// 	std::vector<CustomPolygon>& collection,
// 	bool wireframe,
// 	unsigned int& counter,
// 	sf::RenderWindow& window)
// {
// 	sf::Vector2f mousePos = GetMousePosition(window);
// 	CustomPolygon polygon;
// 	polygon.SetTag("custom");
// 	polygon.SetWireframe(wireframe);
// 	polygon.Init(demo_data::customPolygonCoords, mousePos, world);
// 	collection.push_back(polygon);
// 	++counter;
// }

/** CreateBox
 * 	Create a dynamic body with an associated box polgyon shape and add
 *  it to the b2World.
 */


void CreateBox(b2World& world, float mouseX, float mouseY)
{
	// Create a b2Body via a b2BodyDef
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(mouseX/SCALE, mouseY/SCALE);
	bodyDef.type = b2_dynamicBody;
	bodyDef.angularDamping = 0.1f;
	bodyDef.linearDamping = 0.f;
	bodyDef.allowSleep = true;
	bodyDef.awake = true;
	bodyDef.fixedRotation = false;
	bodyDef.enabled = true;
	b2Body* body = world.CreateBody(&bodyDef);

	// Define shape and body physical properties with b2FixtureDef
	b2PolygonShape shape;
	shape.SetAsBox((SQUARE_SIZE/2)/SCALE, (SQUARE_SIZE/2)/SCALE);

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .7f;
	fixtureDef.shape = &shape;

	body->CreateFixture(&fixtureDef);
}

/** CreateCircle
 * 	Create a dynamic body with an associated circle shape and add
 *  it to the b2World.
 */

void CreateCircle(b2World& world, float mouseX, float mouseY)
{
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(mouseX/SCALE, mouseY/SCALE);
	bodyDef.type = b2_dynamicBody;
	bodyDef.angularDamping = 0.1f;
	bodyDef.linearDamping = 0.f;
	bodyDef.allowSleep = true;
	bodyDef.awake = true;
	bodyDef.fixedRotation = false;
	bodyDef.enabled = true;
	b2Body* body = world.CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = CIRCLE_RADIUS/SCALE;

	b2FixtureDef fixtureDef;
	fixtureDef.density = .4f;
	fixtureDef.friction = .3f;
	fixtureDef.shape = &shape;

	body->CreateFixture(&fixtureDef);
}

/** CreateGround
 * 	Create a static body with an associated box shape and add
 *  it to the b2World.
 */

void CreateGround(b2World& world, float x, float y)
{
	// Create a b2Body via a b2BodyDef
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(x/SCALE, y/SCALE);
	bodyDef.type = b2_staticBody;
	b2Body* body = world.CreateBody(&bodyDef);

	// Define shape and body physical properties with b2FixtureDef
	b2PolygonShape shape;
	shape.SetAsBox((800.f/2)/SCALE, (16.f/2)/SCALE);
	b2FixtureDef fixtureDef;
	fixtureDef.density = 0.f;
	fixtureDef.shape = &shape;

	body->CreateFixture(&fixtureDef);
}

/** (Deprecated) RemoveExpiredCustomPolygons
 * 	Deletes expired CustomPolygon objects and destroys their
 *  associated Box2D object.
 */

// void RemoveExpiredCustomPolygons(
// 	std::vector<CustomPolygon>& polygons, b2World* world, unsigned int& counter)
// {
// 	// Remove expired polygons
// 	auto newEnd = std::remove_if(polygons.begin(), polygons.end(),
// 		[&counter](CustomPolygon& polygon) -> bool {
// 			if (polygon.IsExpired())
// 				--counter;
// 			return polygon.IsExpired();
// 		});

// 	// Resize vector appropriately
// 	polygons.erase(newEnd, polygons.end());
// }

/** (Deprecated) RemoveDynamicBodies
 *
 * 	Destroys all dynamic bodies in the b2World, including
 *  CustomPolygon objects.
 */

void RemoveDynamicBodies(
		b2World* world,
		unsigned int& counter,
		std::vector<CustomPolygon>& customPolygons)
{
	std::vector<b2Body*> bodiesToDelete;

	// Get pointers to all dynamic bodies that contain no user data
	for (b2Body* body = world->GetBodyList(); body != 0; body = body->GetNext())
	{
		if (body->GetType() == b2_dynamicBody && body->GetUserData().pointer == 0)
		{
			bodiesToDelete.push_back(body);
		}
	}

	// Delete bodies
	if (bodiesToDelete.size() > 0)
	{
		for (auto iter = bodiesToDelete.begin(); iter != bodiesToDelete.end(); ++iter)
		{
			b2Body* body = *iter;
			world->DestroyBody(body);
			*iter = nullptr;
		}
	}

	// Delete custom polygons
	// if (customPolygons.size() > 0)
	// {
	// 	for (auto& polygon : customPolygons)
	// 		polygon.Delete(world);

	// 	customPolygons.clear();
	// }

	// Reset dynamic body count
	counter = 0;
}

/** RemoveOffScreenDynamicBodies
 * 	Destroys all dynamic bodies in the b2World, including
 *  CustomPolygon objects.
 */

void RemoveOffScreenDynamicBodies(b2World* world, unsigned int& counter)
{
	std::vector<b2Body*> bodiesToDelete;

	for (b2Body* body = world->GetBodyList(); body != 0; body = body->GetNext())
	{
		// Skip bodies with user data
		if (body->GetUserData().pointer != 0)
			continue;

		// Get body world position
		b2Vec2 scaledWorldPosition = body->GetWorldCenter();
		sf::Vector2f worldPosition;
		worldPosition.x = scaledWorldPosition.x * SCALE;
		worldPosition.y = scaledWorldPosition.y * SCALE;

		// Add body pointer to vector if world position out of bounds
		if (worldPosition.x < -50.f || worldPosition.x > 1250.f ||
			worldPosition.y < -50.f || worldPosition.y > 650.f)
		{
			bodiesToDelete.push_back(body);
		}
	}

	// Delete bodies that are off screen
	for (auto iter = bodiesToDelete.begin(); iter != bodiesToDelete.end(); ++iter)
	{
		b2Body* body = *iter;
		world->DestroyBody(body);
		*iter = nullptr;

		// Decrement counter
		--counter;
	}
}

/** Traverse bodies
 *
 * Debugging function that counts the amount of bodies, dynamic bodies
 * and fixtures in the passed Box2D world.
 */

void TraverseWorldBodies(b2World* world)
{
	int bodyCount = 0;
	int dynamicBodyCount = 0;
	int fixtureCount = 0;

	for (b2Body* body = world->GetBodyList(); body != 0; body = body->GetNext())
	{
		++bodyCount;

		if (body->GetType() == b2_dynamicBody)
		{
			++dynamicBodyCount;

			b2Fixture* fixture = body->GetFixtureList();

			while (fixture != NULL)
			{
				++fixtureCount;

				switch (fixture->GetType())
				{
					case b2Shape::e_polygon:
						break;
					case b2Shape::e_circle:
						break;
					case b2Shape::e_edge:
						break;
					case b2Shape::e_chain:
						break;
					case b2Shape::e_typeCount:
						break;
					default:
						break;
				}
			}
		}
	}
}

/** Traverse contacts
 *
 * Function that demonstrates how to traverse all the contacts in the
 * world, and the contacts in each body.
 */
void TraverseContacts(b2World* world)
{
	/*
	CAUTION: Accessing contacts from the world and bodies may miss some
	transient contacts that occur in the middle of a time step.
	Use b2ContactListener to get the most accurate results.
	*/

	// Traversing contacts using the world:
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		// Process c

		// Getting fixtures and bodies from a contact
		b2Fixture* fixtureA = c->GetFixtureA();
		b2Fixture* fixtureB = c->GetFixtureB();

		b2Body* bodyA = fixtureA->GetBody();
		b2Body* bodyB = fixtureB->GetBody();

		// Can access associated actor if pointer set as body's user data:
		// MyActor* actorA = (MyActor*)bodyA->GetUserData();

		if (bodyA->GetUserData().pointer != 0)
			;
		if (bodyB->GetUserData().pointer != 0)
			;
	}

	// Traversing contacts on each body in the world:
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		// Contacts on a body are stored in a graph using a
		// contact edge structure
		for (b2ContactEdge* ce = b->GetContactList(); ce; ce = ce->next)
		{
			b2Contact* c = ce->contact;

			if (c->IsEnabled())
				;

			// process c
		}
	}
}

}; // namespace physics
