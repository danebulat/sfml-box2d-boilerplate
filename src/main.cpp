#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include "box2d/box2d.h"
#include "utils/box2d_utils.hpp"
#include "utils/static_edge_chain.hpp"
#include "utils/custom_polygon.hpp"
#include "utils/mouse_utils.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui/imgui_utils.hpp"
#include "imgui/imgui_demos.hpp"

using namespace physics;

// ImGui variabless
static int e = 1;
bool edgeChainsEnabled = true;
bool clearAllBodies = false;
bool renderMouseCoords = true;
bool renderBoundingBoxes = true;

int selectedStaticEdgeChainIndex = 0;
int prevSelectedStaticEdgeChainIndex = 0;
std::vector<std::string> staticEdgeChainLabels;

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
	std::vector<StaticEdgeChain> staticEdgeChains;
	staticEdgeChains.push_back(StaticEdgeChain(demo_data::coords, "EC1", &world));
	staticEdgeChains.push_back(StaticEdgeChain(demo_data::coordsLeft, "EC2", &world));
	staticEdgeChains.push_back(StaticEdgeChain(demo_data::coordsRight, "EC3", &world));

	staticEdgeChains[0].SetEditable(true);
	staticEdgeChains[0].DrawBoundingBox(true);

	for (auto& chain : staticEdgeChains)
		staticEdgeChainLabels.push_back(chain.GetTag()); // for ImGui

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

					for (auto& chain : staticEdgeChains)
						chain.SetEnabled(edgeChainsEnabled);
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
					CreateCustomPolygon(&world, customPolygons, wireframe, count_dynamicBodies, window);
				}
			}

			// Mouse move
			if (event.type == sf::Event::MouseMoved)
			{
				SetMouseLabel(mouseLabel, window);
			}

			// Left and right button release
			if (event.type == sf::Event::MouseButtonReleased)
			{
				// Spawn a circle
				if (event.mouseButton.button == sf::Mouse::Middle)
				{
					sf::Vector2f mousePos = GetMousePosition(window);
					CreateCircle(world, mousePos.x, mousePos.y);
					++count_dynamicBodies;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					holdingRMB = false;

					// Spawn a box
					if (rmbMode == RMBMode::BoxSpawnMode)
					{
						sf::Vector2f mousePos = GetMousePosition(window);
						CreateBox(world, mousePos.x, mousePos.y);
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

			// Handle edge chain input
			for (auto& chain : staticEdgeChains)
				chain.HandleInput(event, window);

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
		}

		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::AlignTextToFramePadding();
            ImGui::Text("Wireframe"); ImGui::SameLine(195);
            ImGui::SetNextItemWidth(-1);
			if (ImGui::Checkbox("##Wireframe", &wireframe))
			{
				for (auto& polygon : customPolygons)
					polygon.SetWireframe(wireframe);
			}

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Selection Bounding Box"); ImGui::SameLine(195);
            ImGui::SetNextItemWidth(-1);
			ImGui::Checkbox("##BoundingBoxes", &renderBoundingBoxes);

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Mouse Coords"); ImGui::SameLine(195);
            ImGui::SetNextItemWidth(-1);
			ImGui::Checkbox("##MouseCoords", &renderMouseCoords);
		}

		if (ImGui::CollapsingHeader("Box2D Bodies", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float windowWidth = ImGui::GetWindowContentRegionWidth();
            int i = 0;
            ImGui::PushID(i);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));

			if (ImGui::Button("Clear All Dynamic Bodies", ImVec2(windowWidth, 30)))
			{
				clearAllBodies = true;
			}

			ImGui::PopStyleColor(3);
            ImGui::PopID();

			ImGui::Text("Dynamic Bodies: %d", count_dynamicBodies);
		}

		if (ImGui::CollapsingHeader("Static Edge Chains", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::AlignTextToFramePadding();
            ImGui::Text("Enable"); ImGui::SameLine(130);
            ImGui::SetNextItemWidth(-1);
			if (ImGui::Checkbox("##EnableEdgeChains", &edgeChainsEnabled))
			{
				for (auto& chain : staticEdgeChains)
					chain.SetEnabled(edgeChainsEnabled);
			}

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Selected"); ImGui::SameLine(130);
            ImGui::SetNextItemWidth(-1);

			if (staticEdgeChains.size() > 0)
			{
				if (ImGui::Combo("##SelectedEdgeChain", &selectedStaticEdgeChainIndex, staticEdgeChainLabels))
				{
					staticEdgeChains[prevSelectedStaticEdgeChainIndex].SetEditable(false);
					staticEdgeChains[prevSelectedStaticEdgeChainIndex].DrawBoundingBox(false);
					staticEdgeChains[selectedStaticEdgeChainIndex].SetEditable(true);
					staticEdgeChains[selectedStaticEdgeChainIndex].DrawBoundingBox(true);
					prevSelectedStaticEdgeChainIndex = selectedStaticEdgeChainIndex;
				}
			}
			else
			{
				ImGui::Text("--");
			}

			/* Add new edge chain */
			float windowWidth = ImGui::GetWindowContentRegionWidth();
            int i = 3;
            ImGui::PushID(i);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));

			if (ImGui::Button("Add Edge Chain", ImVec2(windowWidth, 30)))
			{
				sf::Vector2f startPos(400.f, 300.f);
				std::string tag = "EC" + std::to_string(staticEdgeChains.size() + 1);
				staticEdgeChains.push_back(StaticEdgeChain(demo_data::newChainCoords, tag, &world));
				staticEdgeChainLabels.push_back(tag);

				selectedStaticEdgeChainIndex = staticEdgeChains.size()-1;
				staticEdgeChains[prevSelectedStaticEdgeChainIndex].SetEditable(false);
				staticEdgeChains[prevSelectedStaticEdgeChainIndex].DrawBoundingBox(false);
				staticEdgeChains[selectedStaticEdgeChainIndex].SetEditable(true);
				staticEdgeChains[selectedStaticEdgeChainIndex].DrawBoundingBox(true);
				prevSelectedStaticEdgeChainIndex = selectedStaticEdgeChainIndex;
			}

			ImGui::PopStyleColor(3);
            ImGui::PopID();

			/* Delete selected edge chain */
			i = 3;
            ImGui::PushID(i+1);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));

			if (ImGui::Button("Delete Selected Chain", ImVec2(windowWidth, 30)))
			{
				if (staticEdgeChains.size() > 0)
				{
					// Remove label
					std::string tag = staticEdgeChains[selectedStaticEdgeChainIndex].GetTag();
					staticEdgeChainLabels.erase(
						find(staticEdgeChainLabels.begin(), staticEdgeChainLabels.end(), tag));

					// Remove chain
					std::vector<StaticEdgeChain>::iterator chain = staticEdgeChains.begin() + selectedStaticEdgeChainIndex;
					chain->SetEditable(false);
					chain->DeleteBody(&world);
					staticEdgeChains.erase(chain);

					// Update indexes
					selectedStaticEdgeChainIndex = staticEdgeChains.size() - 1;
					staticEdgeChains[selectedStaticEdgeChainIndex].SetEditable(true);
					prevSelectedStaticEdgeChainIndex = selectedStaticEdgeChainIndex;
				}
			}

			ImGui::PopStyleColor(3);
            ImGui::PopID();

			if (ImGui::Button("Add Vertex"))
			{
				if (staticEdgeChains.size() != 0)
					staticEdgeChains[selectedStaticEdgeChainIndex].m_addVertex = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove Vertex"))
			{
				if (staticEdgeChains.size() != 0)
					staticEdgeChains[selectedStaticEdgeChainIndex].m_removeVertex = true;
			}
		}

		std::vector<std::pair<std::string, std::string>> controls =
		{
			std::make_pair("Space", "Spawn custom polygon"),
			std::make_pair("Enter", "Cycle RMB modes"),
			std::make_pair("E", 	"Toggle edge chain active state"),
			std::make_pair("W", 	"Toggle wireframe rendering mode"),
			std::make_pair("Esc", 	"Close window"),
			std::make_pair("\nMMB", "\nSpawn circle rigid body"),
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
		/* Update edge chains */
		for (auto& chain : staticEdgeChains)
			chain.Update(window, &world);

		/** Update Box2D */
		world.Step(1/60.f, 8, 3);

		/* Draw */
		window.clear(sf::Color::White);

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

				// Get fixture to perform point and ray cast checks
				b2Fixture* fixture = body->GetFixtureList();

				// Get fixture type
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

		// Delete bodies that are off screen
		RemoveOffScreenDynamicBodies(&world, count_dynamicBodies);

		// Update polygons and draw if not marked as expired
		for (auto& polygon : customPolygons)
		{
			polygon.Update(&world);
			polygon.Draw(window);
		}

		// Remove marked expired polygons and resize vector
		RemoveExpiredCustomPolygons(customPolygons, &world, count_dynamicBodies);

		// Remove all bodies if flag set
		if (clearAllBodies && count_dynamicBodies > 0)
		{
			clearAllBodies = false;
			RemoveDynamicBodies(&world, count_dynamicBodies, customPolygons);
		}
		else if (clearAllBodies && count_dynamicBodies == 0)
		{
			clearAllBodies = false;
		}

		for (auto& chain : staticEdgeChains)
		{
			if (chain.IsEditable())
				chain.DrawBoundingBox(renderBoundingBoxes);
			chain.Draw(window);
		}

		if (drawClickPoint)
			window.draw(circle);

		if (rmbMode == RMBMode::RayCastMode)
			window.draw(line);

		if (renderMouseCoords)
			window.draw(mouseLabel);

		// Render ImGui windows
		ImGui::SFML::Render(window);

		window.display();
	}

	// clean up ImGui, such as deleting the internal font atlas
    ImGui::SFML::Shutdown();

	return 0;
}
