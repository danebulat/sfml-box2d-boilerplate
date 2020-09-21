#ifndef BOX2D_UTILS_HPP
#define BOX2D_UTILS_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

static const float SCALE = 30.f;
static const float SQUARE_SIZE = 32.f;
static const float CIRCLE_RADIUS = 18.f;

/** GetMousePosition
 *      sf::RenderWindow& - Window to test mouse position
 *
 * 	Get mouse position relative to the window and not the screen.
 */

sf::Vector2f GetMousePosition(const sf::RenderWindow& window)
{
	sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(), window.getView());
	sf::Vector2i windowOffset = window.getPosition();

	return sf::Vector2f(mousePosition.x - (float)windowOffset.x,
						mousePosition.y - (float)windowOffset.y);
}

/** DoTestPoint
 *      b2Fixture*    - The shape to test against
 *      sf::Vector2f& - The position where mouse was clicked
 *      sf::Shape&    - The shape to change color
 *
 * 	Checks if the passed point is in contact with a shape, and
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
 *      b2Fixture*        - The shape to test against
 *      sf::Vector2f&     - The ray's end point
 *      sf::RenderWindow& - The window to draw collision points
 *
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

/** CreateBox
 *      b2World& - The world to add box to
 *      int		 - Box starting X position
 *      int      - Box starting Y position
 *
 * 	Create a dynamic body with an associated box polgyon shape and add
 *  it to the b2World.
 */

void CreateBox(b2World& world, float mouseX, float mouseY)
{
	// Create a b2Body via a b2BodyDef
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(mouseX/SCALE, mouseY/SCALE);
	bodyDef.type = b2_dynamicBody;
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
 *      b2World& - The world to add box to
 *      int		 - Circle starting X position
 *      int      - Circle starting Y position
 *
 * 	Create a dynamic body with an associated circle shape and add
 *  it to the b2World.
 */

void CreateCircle(b2World& world, float mouseX, float mouseY)
{
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(mouseX/SCALE, mouseY/SCALE);
	bodyDef.type = b2_dynamicBody;
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
 *      b2World& - The world to add ground to
 *      int		 - Ground X position
 *      int      - Ground Y position
 *
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

#endif