#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include "box2d/box2d.h"
#include "utils/custom_polygon.hpp"
#include "utils/mouse_utils.hpp"
#include "utils/edge_chain_manager.hpp"
#include "utils/sprite_manager.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "imgui/imgui_utils.hpp"
#include "imgui/imgui_demos.hpp"

using namespace physics;

// ImGui variabless
static int e = 1;
bool renderMouseCoords = true;

// Set initial editor mode
RMBMode EditorSettings::mode = RMBMode::BoxSpawnMode;

int main(int argc, char** argv)
{
	util::Platform platform;

	/** Prepare the window */
	RESOLUTION.x = VideoMode::getDesktopMode().width;
    RESOLUTION.y = VideoMode::getDesktopMode().height;

	sf::RenderWindow window(sf::VideoMode(RESOLUTION.x * .9f, RESOLUTION.y * .75f, 32),
		"SFML - Box2D Boilerplate", sf::Style::Default);
	window.setFramerateLimit(60);

	// Initialise ImGui
	ImGui::SFML::Init(window);

	// Mouse data
	sf::Font font;
	font.loadFromFile("content/DroidSansMono.ttf");
	sf::Text mouseLabel;
	mouseLabel.setFont(font);
	mouseLabel.setColor(sf::Color::Black);
	mouseLabel.setString("(X,Y)");
	mouseLabel.setCharacterSize(12);

	/** Prepare the world */
	b2Vec2 gravity(0.f, 9.8f);
	b2World world(gravity);

	/* Create edge chain manager */
	std::unique_ptr<EdgeChainManager> edgeChainManager(new EdgeChainManager(&world));

	/* Create sprite manager */
	std::unique_ptr<SpriteManager> spriteManager(new SpriteManager(&world, RESOLUTION));

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
					int mode = (static_cast<int>(EditorSettings::mode) + 1);
					if (mode > 3)
					{
						EditorSettings::mode = RMBMode::BoxSpawnMode;
						e = 1;
					}
					else
					{
						EditorSettings::mode = static_cast<RMBMode>(mode);
						e = mode;
					}
				}

				// E key: toggle static edge shape
				if (event.key.code == sf::Keyboard::E)
				{
					edgeChainManager->ToggleEnable();
				}

				// Space key: add new custom polygon
				if (event.key.code == sf::Keyboard::Space)
				{
					//CreateCustomPolygon(&world, customPolygons, wireframe, count_dynamicBodies, window);
					spriteManager->PushShape(ShapeType::CustomPolygon, GetMousePosition(window));
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
					spriteManager->PushShape(ShapeType::DebugCircle, GetMousePosition(window));
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					//holdingRMB = false;

					// Spawn a box
					if (EditorSettings::mode == RMBMode::BoxSpawnMode)
					{
						spriteManager->PushShape(ShapeType::DebugBox, GetMousePosition(window));
					}
				}
				else if (event.mouseButton.button == sf::Mouse::Left)
				{
					edgeChainManager->CheckChainClicked(window);
				}
			}

			// Handle manager inputs
			edgeChainManager->HandleInput(event, window);
			spriteManager->HandleInput(event, window);

		}// end window.poll(event)

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
				EditorSettings::mode = RMBMode::BoxSpawnMode;

			ImGui::SameLine();
			if (ImGui::RadioButton("Test Point", &e, 2))
			{
				EditorSettings::mode = RMBMode::TestPointMode;
			}


			ImGui::SameLine();
			if (ImGui::RadioButton("Ray Cast", &e, 3))
				EditorSettings::mode = RMBMode::RayCastMode;
		}

		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::AlignTextToFramePadding();
            ImGui::Text("Wireframe"); ImGui::SameLine(195);
            ImGui::SetNextItemWidth(-1);
			if(ImGui::Checkbox("##Wireframe", spriteManager->GetWireframeFlag()))
			{
				spriteManager->ToggleWireframe();
			}

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Selection Bounding Box"); ImGui::SameLine(195);
            ImGui::SetNextItemWidth(-1);
			ImGui::Checkbox("##BoundingBoxes", &edgeChainManager->GetDrawBBFlag());

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
				//clearAllBodies = true;
				spriteManager->SetDestroryFlag(true);
			}

			ImGui::PopStyleColor(3);
            ImGui::PopID();

			ImGui::Text("Dynamic Bodies: %d", SpriteManager::DynamicBodiesCount);
			ImGui::Text("Debub Shapes: %d", DebugShape::ShapeBodyCount);
			ImGui::Text("Debub Boxes: %d", DebugShape::DebugBoxCount);
			ImGui::Text("Debub Circles: %d", DebugShape::DebugCircleCount);
		}

		if (ImGui::CollapsingHeader("Static Edge Chains", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::AlignTextToFramePadding();
            ImGui::Text("Enable"); ImGui::SameLine(130);
            ImGui::SetNextItemWidth(-1);
			if (ImGui::Checkbox("##EnableEdgeChains", &edgeChainManager->GetEnableFlag()))
			{
				edgeChainManager->SyncEnable();
			}

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Selected"); ImGui::SameLine(130);
            ImGui::SetNextItemWidth(-1);

			if (edgeChainManager->GetChainCount() > 0)
			{
				if (ImGui::Combo("##SelectedEdgeChain", &edgeChainManager->GetSelectedChainIndex(), edgeChainManager->GetChainLabels()))
				{
					edgeChainManager->SelectCurrentChain();
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
				edgeChainManager->PushChain();
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
				edgeChainManager->PopChain();
			}

			ImGui::PopStyleColor(3);
            ImGui::PopID();

			ImGui::PushItemWidth((windowWidth/2) + 10.f);
			if (ImGui::Button("Add Vertex", ImVec2((windowWidth/2), 20)))
			{
				edgeChainManager->AddVertexToSelectedChain();
			}

			ImGui::SameLine((windowWidth/2)+15);
			if (ImGui::Button("Remove Vertex", ImVec2((windowWidth/2)-8.f, 20)))
			{
				edgeChainManager->RemoveVertexFromSelectedChain();
			}

			if (ImGui::Button("Add Circle"))
			{
				spriteManager->PushShape(ShapeType::DebugCircle, sf::Vector2f(100.f,100.f));
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

		/** Update Box2D */
		world.Step(1/60.f, 8, 3);

		/* Update managers */
		edgeChainManager->Update(window);
		spriteManager->Update();

		/* Draw */
		window.clear(sf::Color::White);

		/* Draw objects */
		spriteManager->Draw(window);
		edgeChainManager->Draw(window);

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
