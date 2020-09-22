#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include "box2d/box2d.h"
#include "utils/box2d_utils.hpp"

int main(int argc, char** argv)
{
	util::Platform platform;

	// Mouse data
	bool holdingRMB = false;
	sf::Font font;
	font.loadFromFile("content/DroidSansMono.ttf");
	sf::Text mouseLabel;
	mouseLabel.setFont(font);
	mouseLabel.setColor(sf::Color::Black);
	mouseLabel.setString("(X,Y)");
	mouseLabel.setCharacterSize(12);

	// Circle data for clicking
	sf::Vector2f clickPos;
	bool drawClickPoint = false;
	sf::CircleShape circle;
	circle.setRadius(5.f);
	circle.setFillColor(sf::Color::Red);
	circle.setOrigin(sf::Vector2(5.f, 5.f));

	// Line data for ray cast
	bool rayCastDemo = false;
	sf::Vector2f rayEndPoint;
	sf::VertexArray line(sf::LinesStrip, 2);
	line[0].position = sf::Vector2f(0.f, 600.f);
	line[0].color = sf::Color::Red;
	line[1].position = sf::Vector2f(0.f, 600.f);
	line[1].color = sf::Color::Red;

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
                window.close();

			if (event.type == sf::Event::KeyReleased)
            {
				// Escape key: exit
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();

				// Enter key: toggle demo
				if (event.key.code == sf::Keyboard::Enter)
					rayCastDemo = !rayCastDemo;
			}

			// Mouse move
			if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2f position = (GetMousePosition(window) + sf::Vector2f(10.f, -15.f));
				std::string xPos = std::to_string(position.x);
				std::string yPos = std::to_string(position.y);

				xPos = xPos.substr(0, 5);
				yPos = yPos.substr(0, 5);
				std::string label = "(" + xPos + "," + yPos + ")";

				mouseLabel.setPosition(position);
				mouseLabel.setString(label);
			}

			// Left and right button release
			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					sf::Vector2f mousePos = GetMousePosition(window);
					float mouseX = mousePos.x;
					float mouseY = mousePos.y;

					CreateBox(world, mouseX, mouseY);
				}
				else if (event.mouseButton.button == sf::Mouse::Middle)
				{
					sf::Vector2f mousePos = GetMousePosition(window);
					float mouseX = mousePos.x;
					float mouseY = mousePos.y;

					CreateCircle(world, mouseX, mouseY);
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
					holdingRMB = false;
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Right)
					holdingRMB = true;
			}
			else
				drawClickPoint = false;
		}// end window.poll(event)

		if (holdingRMB && rayCastDemo)
		{
			rayEndPoint = GetMousePosition(window);

			line[1].position.x = rayEndPoint.x;
			line[1].position.y = rayEndPoint.y;
		}
		else if (holdingRMB && !rayCastDemo)
		{
			drawClickPoint = true;
			clickPos = GetMousePosition(window);
			circle.setPosition(clickPos);
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
				// Get fixture to perform point and ray cast checks
				b2Fixture* fixture = body->GetFixtureList();

				// Check if point collides with shape
				// https://stackoverflow.com/questions/5873309/box2d-get-shape-of-my-bodies
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
							sprite.setFillColor(sf::Color::White);
							++bodyCount;

							// Check if shape is clicked on
							if (drawClickPoint)
								DoTestPoint(fixture, clickPos, sprite);

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
							sprite.setFillColor(sf::Color::White);
							++bodyCount;

							// Check if shape is clicked on
							if (drawClickPoint)
								DoTestPoint(fixture, clickPos, sprite);

							window.draw(sprite);
							break;
						}
					}

					// Draw raycast
					if (rayCastDemo)
						DoRayCast(fixture, rayEndPoint, window);

					fixture = fixture->GetNext();
				}// while (fixture != NULL)
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

		if (drawClickPoint)
			window.draw(circle);

		if (rayCastDemo)
			window.draw(line);

		window.draw(mouseLabel);

		window.display();
	}

	return 0;
}
