#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include "box2d/box2d.h"
#include "utils/box2d_utils.hpp"
#include "utils/static_edge_chain.hpp"
#include "utils/custom_polygon.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui/imgui_utils.hpp"
#include "imgui/imgui_demos.hpp"

// ImGui variabless
static int e = 1;
bool edgeChainsEnabled = true;
bool clearAllBodies = false;

enum class RMBMode
{
	BoxSpawnMode = 1,
	TestPointMode,
	RayCastMode
};

int main(int argc, char** argv)
{
	util::Platform platform;
	RMBMode rmbMode = RMBMode::BoxSpawnMode;

	/** Prepare the window */
	sf::RenderWindow window(sf::VideoMode(1150.f, 600.f, 32), "SFML - Box2D Boilerplate", sf::Style::Default);
	window.setFramerateLimit(60);

	// Initialise ImGui
	ImGui::SFML::Init(window);

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
	sf::Vector2f rayEndPoint;
	sf::VertexArray line(sf::LinesStrip, 2);
	line[0].position = sf::Vector2f(0.f, 600.f);
	line[0].color = sf::Color::Red;
	line[1].position = sf::Vector2f(0.f, 600.f);
	line[1].color = sf::Color::Red;

	/** Prepare the world */
	b2Vec2 gravity(0.f, 9.8f);
	b2World world(gravity);
	CreateGround(world, 400.f, 575.f);

	/* Create static edge chain */
	StaticEdgeChain edgeChain;
	edgeChain.Init(demo_data::coords, &world);

	StaticEdgeChain edgeChainLeft;
	edgeChainLeft.Init(demo_data::coordsLeft, &world);

	StaticEdgeChain edgeChainRight;
	edgeChainRight.Init(demo_data::coordsRight, &world);

	/* Vector for custom polygon objects */
	std::vector<CustomPolygon> customPolygons;

	bool wireframe = false;
	sf::Clock clock;

	unsigned int count_dynamicBodies = 0;

	while (window.isOpen())
	{
		sf::Time dt = clock.restart();

		sf::Event event;
		while (window.pollEvent(event))
		{
			// Process ImGui events
			ImGui::SFML::ProcessEvent(event);

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
				{
					int mode = (static_cast<int>(rmbMode) + 1);
					if (mode > 3)
					{
						rmbMode = RMBMode::BoxSpawnMode;
						e = 1;
					}
					else
					{
						rmbMode = static_cast<RMBMode>(mode);
						e = mode;
					}
				}

				// E key: toggle static edge shape
				if (event.key.code == sf::Keyboard::E)
				{
					edgeChainsEnabled = !edgeChainsEnabled;

					edgeChain.SetEnabled(edgeChainsEnabled);
					edgeChainLeft.SetEnabled(edgeChainsEnabled);
					edgeChainRight.SetEnabled(edgeChainsEnabled);
				}

				// W key: toggle custom polygon wireframe
				if (event.key.code == sf::Keyboard::W)
				{
					wireframe = !wireframe;
					for (auto& polygon : customPolygons)
						polygon.SetWireframe(wireframe);
				}

				// Space key: add new custom polygon
				if (event.key.code == sf::Keyboard::Space)
				{
					sf::Vector2f mousePos = GetMousePosition(window);
					CustomPolygon custom;
					custom.SetTag("custom");
					custom.SetWireframe(wireframe);
					custom.Init(demo_data::customPolygonCoords, mousePos, &world);
					customPolygons.push_back(custom);
					++count_dynamicBodies;
				}
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
				if (event.mouseButton.button == sf::Mouse::Middle)
				{
					sf::Vector2f mousePos = GetMousePosition(window);
					float mouseX = mousePos.x;
					float mouseY = mousePos.y;

					CreateCircle(world, mouseX, mouseY);
					++count_dynamicBodies;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					holdingRMB = false;

					if (rmbMode == RMBMode::BoxSpawnMode)
					{
						// Spawn a box
						sf::Vector2f mousePos = GetMousePosition(window);
						float mouseX = mousePos.x;
						float mouseY = mousePos.y;

						CreateBox(world, mouseX, mouseY);
						++count_dynamicBodies;
					}
				}
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					holdingRMB = true;
				}
			}
			else
				drawClickPoint = false;
		}// end window.poll(event)

		if (holdingRMB && rmbMode == RMBMode::RayCastMode)
		{
			rayEndPoint = GetMousePosition(window);

			line[1].position.x = rayEndPoint.x;
			line[1].position.y = rayEndPoint.y;
		}
		else if (holdingRMB && rmbMode == RMBMode::TestPointMode)
		{
			drawClickPoint = true;
			clickPos = GetMousePosition(window);
			circle.setPosition(clickPos);
		}

		/** Update Box2D */
		world.Step(1/60.f, 8, 3);

		/* Update ImGui */
		ImGui::SFML::Update(window, dt);

		/*----------------------------------------------------------------------
         Start ImGui
         ----------------------------------------------------------------------*/
		ImGui::Begin("Box2D Boilerplate");

		if (ImGui::CollapsingHeader("Demo Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("RMB Mode:");
			if (ImGui::RadioButton("Spawn Box", &e, 1))
				rmbMode = RMBMode::BoxSpawnMode;

			ImGui::SameLine();
			if (ImGui::RadioButton("Test Point", &e, 2))
				rmbMode = RMBMode::TestPointMode;

			ImGui::SameLine();
			if (ImGui::RadioButton("Ray Cast", &e, 3))
				rmbMode = RMBMode::RayCastMode;

			ImGui::Text("Rendering:");
			if (ImGui::Checkbox(" Wireframe", &wireframe))
			{
				for (auto& polygon : customPolygons)
					polygon.SetWireframe(wireframe);
			}
		}

		if (ImGui::CollapsingHeader("Box2D Bodies", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Dynamic Bodies: %d\n\n", count_dynamicBodies);

			ImGui::Text("Enable:");
			if (ImGui::Checkbox(" Edge Chains", &edgeChainsEnabled))
			{
				edgeChain.SetEnabled(edgeChainsEnabled);
				edgeChainLeft.SetEnabled(edgeChainsEnabled);
				edgeChainRight.SetEnabled(edgeChainsEnabled);
			}

			float windowWidth = ImGui::GetWindowContentRegionWidth();
            int i = 0;
            ImGui::PushID(i);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));

			if (ImGui::Button("Clear All Dynamic Bodies", ImVec2(windowWidth, 45)))
			{
				clearAllBodies = true;
			}

			ImGui::PopStyleColor(3);
            ImGui::PopID();
		}

		std::vector<std::pair<std::string, std::string>> controls =
		{
			std::make_pair("Space", "Spawn custom polygon"),
			std::make_pair("Enter", "Cycle RMB modes"),
			std::make_pair("E", 	"Toggle edge chain active state"),
			std::make_pair("W", 	"Toggle wireframe rendering mode"),
			std::make_pair("Esc", 	"Close window"),
			std::make_pair("\nLMB", "\nSpawn box rigid body"),
			std::make_pair("MMB", 	"Spawn circle rigid body"),
			std::make_pair("RMB", 	"Perform selected RMB mode"),
		};

		if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto& control : controls)
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), control.first.data());
				ImGui::SameLine(80);
				ImGui::Text(control.second.data());
			}
        }

		ImGui::End();
		/*----------------------------------------------------------------------
         End ImGui
         ----------------------------------------------------------------------*/

		/* Draw */
		window.clear(sf::Color::White);

		std::vector<b2Body*> bodiesToDelete;

		int bodyCount = 0;
		for (b2Body* body = world.GetBodyList(); body != 0; body = body->GetNext())
		{
			if (body->GetType() == b2_dynamicBody)
			{
				// Skip rendering body if it has custom data
				b2BodyUserData ud = body->GetUserData();
				if (ud.pointer != 0)
				{
					continue; // draws independently
				}

				// Delete body if it's off screen
				b2Vec2 scaledWorldPosition = body->GetWorldCenter();
				sf::Vector2f worldPosition;
				worldPosition.x = scaledWorldPosition.x * SCALE;
				worldPosition.y = scaledWorldPosition.y * SCALE;

				if (worldPosition.x < -50.f || worldPosition.x > 1250.f ||
					worldPosition.y < -50.f || worldPosition.y > 650.f)
				{
					bodiesToDelete.push_back(body);
					continue;
				}

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
						default:
							continue;
					}

					// Draw raycast
					if (rmbMode == RMBMode::RayCastMode)
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

		if (rmbMode == RMBMode::RayCastMode)
			window.draw(line);

		// Draw mouse label
		window.draw(mouseLabel);

		// Delete bodies that are off screen
		for (auto iter = bodiesToDelete.begin(); iter != bodiesToDelete.end(); ++iter)
		{
			b2Body* body = *iter;
			world.DestroyBody(body);
			*iter = nullptr;
			--count_dynamicBodies;
		}
		bodiesToDelete.clear();

		// Update polygons and draw if not marked as expired
		for (auto& polygon : customPolygons)
		{
			polygon.Update(&world);
			if (!polygon.IsExpired())
				polygon.Draw(window);
		}

		// Remove polygons and resize vector
		RemoveExpiredCustomPolygons(customPolygons, &world, count_dynamicBodies);

		// Remove all bodies if flag set
		if (clearAllBodies && count_dynamicBodies > 0)
		{
			clearAllBodies = false;

			// Get pointers to all dynamic bodies that contain no user data
			for (b2Body* body = world.GetBodyList(); body != 0; body = body->GetNext())
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
					world.DestroyBody(body);
					*iter = nullptr;
				}
			}

			// Delete custom polygons
			if (customPolygons.size() > 0)
			{
				for (auto& polygon : customPolygons)
					polygon.Delete(&world);
				customPolygons.clear();
			}

			// Reset dynamic body count
			count_dynamicBodies = 0;
		}
		else if (clearAllBodies && count_dynamicBodies == 0)
		{
			clearAllBodies = false;
		}

		edgeChain.Draw(window);
		edgeChainLeft.Draw(window);
		edgeChainRight.Draw(window);

		// Render ImGui windows
		ImGui::SFML::Render(window);

		window.display();
	}

	// clean up ImGui, such as deleting the internal font atlas
    ImGui::SFML::Shutdown();

	return 0;
}
