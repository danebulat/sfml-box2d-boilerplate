#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

/** Easily convert between pixel and real-world coordinates */
static const float SCALE = 30.f;

/** Create the base for the boxes to land */
void CreateGround(b2World& world, float x, float y);

/** Create boxes */
void CreateBox(b2World& world, int mouseX, int mouseY);

/** Create circles */
void CreateCircle(b2World& world, int mouseX, int mouseY);

/** Hello world simulation */
void helloWorld();

const float CIRCLE_RADIUS = 18.f;
const float SQUARE_SIZE = 32.f;

int main(int argc, char** argv)
{
	util::Platform platform;
	//helloWorld();

	// Circle data for clicking
	sf::Vector2f circlePos;
	bool drawCircle = false;
	sf::CircleShape circle;
	circle.setRadius(5.f);
	circle.setFillColor(sf::Color::Red);
	circle.setOrigin(sf::Vector2(5.f, 5.f));

#if defined(_DEBUG)
	std::cout << "Hello World!" << std::endl;
#endif

	/** Prepare the window */
	sf::RenderWindow window(sf::VideoMode(800.f, 600.f, 32), "SFML - Box2D Boilerplate", sf::Style::Default);
	window.setFramerateLimit(60);

	/** Prepare the world */
	b2Vec2 gravity(0.f, 9.8f);
	b2World world(gravity);
	CreateGround(world, 400.f, 500.f);

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			// Close window: exit
            if (event.type == sf::Event::Closed)
			{
                window.close();
            }

			// Escape key: exit
			if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Escape)
				{
                    window.close();
                }
			}

			// Left and right button release
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(), window.getView());
					sf::Vector2i mouseOffset = window.getPosition();

					int mouseX = (int)mousePosition.x - mouseOffset.x;
					int mouseY = (int)mousePosition.y - mouseOffset.y;

					CreateBox(world, mouseX, mouseY);
				}
				else if (event.mouseButton.button == sf::Mouse::Middle)
				{
					sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(), window.getView());
					sf::Vector2i mouseOffset = window.getPosition();

					int mouseX = (int)mousePosition.x - mouseOffset.x;
					int mouseY = (int)mousePosition.y - mouseOffset.y;

					CreateCircle(world, mouseX, mouseY);
				}
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					drawCircle = true;

					sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(), window.getView());
					sf::Vector2i mouseOffset = window.getPosition();

					circlePos.x = (float)mousePosition.x - (float)mouseOffset.x;
					circlePos.y = (float)mousePosition.y - (float)mouseOffset.y;

					circle.setPosition(circlePos);
					//std::cout << "right mouse button pressed\n";
				}
			}
			else
			{
				drawCircle = false;
			}
		}

		/** Update */
		world.Step(1/60.f, 8, 3);

		/* Draw */
		window.clear(sf::Color::White);

		int bodyCount = 0;
		for (b2Body* body = world.GetBodyList(); body != 0; body = body->GetNext())
		{
			if (body->GetType() == b2_dynamicBody)
			{
				// Check if point collides with shape
				// https://stackoverflow.com/questions/5873309/box2d-get-shape-of-my-bodies
				b2Fixture* fixture = body->GetFixtureList();

				while (fixture != NULL)
				{
					switch (fixture->GetType())
					{
						case b2Shape::e_polygon:
						{
							sf::RectangleShape sprite;
							sprite.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
							sprite.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
							sprite.setRotation(body->GetAngle() * 180/b2_pi);
							sprite.setOrigin(16.f, 16.f);
							sprite.setOutlineColor(sf::Color::Black);
							sprite.setOutlineThickness(2.f);
							++bodyCount;

							if (drawCircle)
							{
								b2Vec2 mousePoint(circlePos.x/SCALE, circlePos.y/SCALE);

								if (fixture->TestPoint(mousePoint))
								{
									sprite.setFillColor(sf::Color::Green);
								}
							}
							else
							{
								sprite.setFillColor(sf::Color::White);
							}

							window.draw(sprite);
							break;
						}
						case b2Shape::e_circle:
						{
							sf::CircleShape sprite;
							sprite.setRadius(CIRCLE_RADIUS);
							sprite.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
							sprite.setRotation(body->GetAngle() * 180/b2_pi);
							sprite.setOrigin(CIRCLE_RADIUS, CIRCLE_RADIUS);
							sprite.setOutlineColor(sf::Color::Black);
							sprite.setOutlineThickness(2.f);
							++bodyCount;

							if (drawCircle)
							{
								b2Vec2 mousePoint(circlePos.x/SCALE, circlePos.y/SCALE);

								if (fixture->TestPoint(mousePoint))
								{
									sprite.setFillColor(sf::Color::Blue);
								}
							}
							else
							{
								sprite.setFillColor(sf::Color::White);
							}

							window.draw(sprite);
							break;
						}
					}

					fixture = fixture->GetNext();
				}
			}
			else
			{
				sf::RectangleShape ground;
				ground.setSize(sf::Vector2f(800.f, 4.f));
				ground.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
				ground.setRotation(body->GetAngle() * 180/b2_pi);
				ground.setOrigin(400.f, 8.f);
				ground.setFillColor(sf::Color::White);
				ground.setOutlineColor(sf::Color::Black);
				ground.setOutlineThickness(2.f);
				window.draw(ground);
			}
		}

		if (drawCircle)
		{
			window.draw(circle);
			//std::cout << "draw circle at (" << circle.getPosition().x << "," << circle.getPosition().y << ")\n";
		}

		window.display();
	}

	return 0;
}

void CreateBox(b2World& world, int mouseX, int mouseY)
{
	/** create a b2Body via a b2BodyDef */
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(mouseX/SCALE, mouseY/SCALE);
	bodyDef.type = b2_dynamicBody;
	b2Body* body = world.CreateBody(&bodyDef);

	/** define shape and body physical properties with b2FixtureDef */
	b2PolygonShape shape;
	shape.SetAsBox((SQUARE_SIZE/2)/SCALE, (SQUARE_SIZE/2)/SCALE);	// pass half of width and height

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .7f;
	fixtureDef.shape = &shape;
	body->CreateFixture(&fixtureDef);
}

void CreateCircle(b2World& world, int mouseX, int mouseY)
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

void CreateGround(b2World& world, float x, float y)
{
	/** create a b2Body via a b2BodyDef */
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(x/SCALE, y/SCALE);
	bodyDef.type = b2_staticBody;
	b2Body* body = world.CreateBody(&bodyDef);

	/** define shape and body physical properties with b2FixtureDef */
	b2PolygonShape shape;
	shape.SetAsBox((800.f/2)/SCALE, (16.f/2)/SCALE); // pass half of width and height
	b2FixtureDef fixtureDef;
	fixtureDef.density = 0.f;
	fixtureDef.shape = &shape;
	body->CreateFixture(&fixtureDef);
}

void helloWorld()
{
	/** Creating a b2World
	*/

	// First define a gravity vector
	b2Vec2 gravity(0.f, -10.f);

	// Create the world object (on stack in this case)
	b2World world(gravity);


	/** Creating a Ground Box
	*/

	// Body definition defines initial position (static body by default)
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.f, -10.f);

	// Create the ground body
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	// Create a ground polygon (box(hx, hy))
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.f, 10.f);

	// Create a shape fixture using shortcut (shape density kg p/meter squared, 0 for static bodies)
	groundBody->CreateFixture(&groundBox, 0.0f);


	/** Creating a Dynamic Body
	*/

	// Create the body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.f, 4.f);
	b2Body* body = world.CreateBody(&bodyDef);

	// Create and attach polgon shape using a fixture definition
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.f, 1.f);


	/** TMP **/

	/** Circle shapes
	*/
	b2CircleShape circle;
	circle.m_p.Set(2.f, 3.f);
	circle.m_radius = .5f;

	/** Convex polygons
	*/

	// Define a triangle in CCW order
	b2Vec2 vertices[3];
	vertices[0].Set(0.f, 0.f);
	vertices[1].Set(1.f, 0.f);
	vertices[2].Set(0.f, 1.f);

	int32 count = 3;
	b2PolygonShape polygon;
	polygon.Set(vertices, count);

	/** Edge shapes
	*/

	// Define an edge with ghost vertices on either side of edge
	b2Vec2 v0(1.7f, 0.f);
	b2Vec2 v1(1.f, .25f);
	b2Vec2 v2(0.f, 0.f);
	b2Vec2 v3(-1.7f, .4f);
	b2EdgeShape edge;
	edge.SetOneSided(v0, v1, v2, v3);

	/** Chain shapes
	*/

	// Chain shapes - loop (CCW = outward normal, CW = inward normal)
	b2Vec2 vs[4];
	vs[0].Set(1.7f, 0.f);
	vs[1].Set(1.f, .25f);
	vs[2].Set(0.f, 0.f);
	vs[2].Set(-1.7f, .4f);
	b2ChainShape chain;
	chain.CreateLoop(vs, 4);

	// To connect several chains together using ghost vertices (I.e., in scrolling game worlds)
	b2ChainShape chain2;
	b2Vec2 prev(1.9f, 0.f);
	b2Vec2 next(-1.8f, .4f);
	chain2.CreateChain(vs, 4, prev, next);

	// Visit each child edge in chain
	for (int32 i = 0; i < chain.GetChildCount(); ++i) {
		b2EdgeShape edge;
		chain.GetChildEdge(&edge, i);
	}

	/** Shape point test
	*/
	b2Transform transform;
	transform.SetIdentity();
	b2Vec2 point(5.f, 2.f);
	bool hit = polygon.TestPoint(transform, point);

	/** Shape Ray Cast
	*/
	b2Transform transform2;
	transform2.SetIdentity();

	b2RayCastInput input;
	input.p1.Set(0.f, 0.f);
	input.p2.Set(1.f, 0.f);
	input.maxFraction = 1.f;

	int32 childIndex; // for chain shapes
	b2RayCastOutput output;

	bool hit2 = chain.RayCast(&output, input, transform2, childIndex);
	if (hit2) {
		b2Vec2 hitPoint = input.p1 + output.fraction * (input.p2 - input.p1);
		std::cout << "hit (" << hitPoint.x << "," << hitPoint.y << ")\n";
	}

	/** END TMP **/


	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .3f;

	// Create fixture using definition
	body->CreateFixture(&fixtureDef);


	/** Simulating the world
	*/

	// Create a time step
	float timeStep = 1.f/60.f;

	int32 velocityIterations = 6;		// 8 default
	int32 positionInterations = 2;		// 3 default

	// Call b2World::Step to process the simulation loop; one call usually enough per frame
	for (int32 i = 0; i < 60; ++i)
	{
		world.Step(timeStep, velocityIterations, positionInterations);
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		std::cout << "position: (" << position.x << "," << position.y << ")\tangle: " << angle << "radians\n";
	}
}