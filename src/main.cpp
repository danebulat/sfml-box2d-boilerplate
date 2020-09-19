#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

/** Easily convert between pixel and real-world coordinates */
static const float SCALE = 30.f;

/** Create the base for the boxes to land */
void CreateGround(b2World& world, float x, float y);

/** Create the boxes */
void CreateBox(b2World& world, int mouseX, int mouseY);

/** Hello world simulation */
void helloWorld();

int main(int argc, char** argv)
{
	util::Platform platform;

	helloWorld();

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

			if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Escape)
				{
                    window.close();
                }
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(), window.getView());
				sf::Vector2i mouseOffset = window.getPosition();

				int mouseX = (int)mousePosition.x - mouseOffset.x;
				int mouseY = (int)mousePosition.y - mouseOffset.y;

				CreateBox(world, mouseX, mouseY);
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
				sf::RectangleShape sprite;
				sprite.setSize(sf::Vector2f(32.f, 32.f));
				sprite.setPosition(SCALE * body->GetPosition().x, SCALE * body->GetPosition().y);
				sprite.setRotation(body->GetAngle() * 180/b2_pi);
				sprite.setOrigin(16.f, 16.f);
				sprite.setFillColor(sf::Color::White);
				sprite.setOutlineColor(sf::Color::Black);
				sprite.setOutlineThickness(2.f);
				window.draw(sprite);
				++bodyCount;
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
	shape.SetAsBox((32.f/2)/SCALE, (32.f/2)/SCALE);	// pass half of width and height
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .7f;
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