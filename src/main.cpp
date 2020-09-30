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

	sf::Clock clock;

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
					if (mode > 2)
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
					edgeChainManager->SyncEnable();
				}

				// Space key: add new custom polygon
				if (event.key.code == sf::Keyboard::Space)
				{
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
		ImGui::Begin("Box2D Level Editor");

		/* Demo Settings */
		if (ImGui::CollapsingHeader("Editor Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Separator();

			ImGui::Text("RMB Mode:");
			if (ImGui::RadioButton("Spawn Box", &e, 1))
				EditorSettings::mode = RMBMode::BoxSpawnMode;

			ImGui::SameLine();
			if (ImGui::RadioButton("Test Point", &e, 2))
				EditorSettings::mode = RMBMode::TestPointMode;

			// ImGui::SameLine();
			// if (ImGui::RadioButton("Ray Cast", &e, 3))
			// 	EditorSettings::mode = RMBMode::RayCastMode;

			ImGui::Separator();
		}

		/* Display Settings */
		if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Separator();

			float windowWidth = ImGui::GetWindowContentRegionWidth();

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Mouse Coords");

			ImGui::SameLine();
			ImGui::HelpMarker("Display mouse coordinates next to mouse pointer.");

			ImGui::SameLine((windowWidth/2) * 0.8f);
			ImGui::Checkbox("##MouseCoords", &renderMouseCoords);

			ImGui::AlignTextToFramePadding();
			ImGui::SameLine(windowWidth/2);
            ImGui::Text("Wireframe");

			ImGui::SameLine();
			ImGui::HelpMarker("Render custom polygons in a wireframe mode.");

			ImGui::SetNextItemWidth(-1);
			ImGui::SameLine(windowWidth - 20.f);
			if(ImGui::Checkbox("##Wireframe", spriteManager->GetWireframeFlag()))
			{
				spriteManager->ToggleWireframe();
			}

			ImGui::AlignTextToFramePadding();
            ImGui::Text("Bounding Boxes");

			ImGui::SameLine();
			ImGui::HelpMarker("Display bounding boxes around the selected edge chain.");

            ImGui::SetNextItemWidth(-1);
			ImGui::SameLine((windowWidth/2) * 0.8f);
			ImGui::Checkbox("##BoundingBoxes", &edgeChainManager->GetDrawBBFlag());

			ImGui::AlignTextToFramePadding();
			ImGui::SameLine(windowWidth/2);
            ImGui::Text("Labels");

			ImGui::SameLine();
			ImGui::HelpMarker("TODO: Display edge labels.");

			bool labelBool = true;
			ImGui::SetNextItemWidth(-1);
			ImGui::SameLine(windowWidth - 20.f);
			ImGui::Checkbox("##Labels", &labelBool);

			ImGui::Separator();
		}

		/* Bodies Settings */
		if (ImGui::CollapsingHeader("Bodies", ImGuiTreeNodeFlags_DefaultOpen))
		{
			sf::Vector2f shapeStartPos(RESOLUTION.x * .25f, RESOLUTION.y * .25f);
			float windowWidth = ImGui::GetWindowContentRegionWidth();
			float btnwidth = (windowWidth*.25f) - ((windowWidth*.225f)*.05f);

			ImGui::Separator();

			// Clear all
            int i = 0;
            ImGui::PushID(i+10);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			if (ImGui::Button("Clear All", ImVec2(btnwidth, 30)))
				spriteManager->SetDestroryFlag(true);
			ImGui::PopStyleColor(3);
            ImGui::PopID();

			// Add
			i = 4;
            ImGui::PushID(i+11);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ImGui::SameLine();
			if (ImGui::Button("+Box", ImVec2(btnwidth, 30)))
				spriteManager->PushShape(ShapeType::DebugBox, shapeStartPos);
			ImGui::PopStyleColor(3);
            ImGui::PopID();

			ImGui::PushID(i+12);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ImGui::SameLine();
			if (ImGui::Button("+Circle", ImVec2(btnwidth, 30)))
				spriteManager->PushShape(ShapeType::DebugCircle, shapeStartPos);
			ImGui::PopStyleColor(3);
            ImGui::PopID();

			ImGui::PushID(i+13);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
			ImGui::SameLine();
			if (ImGui::Button("+Polygon", ImVec2(btnwidth, 30)))
				spriteManager->PushShape(ShapeType::CustomPolygon, shapeStartPos);
			ImGui::PopStyleColor(3);
            ImGui::PopID();

			ImGui::Separator();

			ImGui::SetNextTreeNodeOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::TreeNode("Level"))
			{
				ImVec4 lightBlue(.6f, .8f, 1.f, 1.f);

				ImGui::Text("Dynamic Bodies:");

				ImGui::SameLine();
				ImGui::TextColored(lightBlue, "%d", SpriteManager::DynamicBodiesCount);

				ImGui::TreePop();
			}

			ImGui::Separator();

			ImGui::SetNextTreeNodeOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::TreeNode("Debug Shapes"))
			{
				ImVec4 lightBlue(.6f, .8f, 1.f, 1.f);

				ImGui::SameLine();
				ImGui::HelpMarker("Statistics for debug shape objects.");

				ImGui::Text("Boxes:");
				ImGui::SameLine();
				ImGui::TextColored(lightBlue, "%d", DebugShape::DebugBoxCount);
				ImGui::SameLine();
				ImGui::Text(" Circles:");
				ImGui::SameLine();
				ImGui::TextColored(lightBlue, "%d", DebugShape::DebugCircleCount);

				ImGui::SameLine();
				ImGui::Text(" Polygons:");
				ImGui::SameLine();
				ImGui::TextColored(lightBlue, "%d", DebugShape::CustomPolygonCount);

				ImGui::SameLine();
				ImGui::Text("Total:");
				ImGui::SameLine();
				ImGui::TextColored(lightBlue, "%d", DebugShape::ShapeBodyCount);
				ImGui::TreePop();
			}

			ImGui::Separator();

			ImGui::SetNextTreeNodeOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::TreeNode("Edge Chains"))
			{
				ImGui::SameLine();
				ImGui::HelpMarker("Statistics for edge chain objects.");

				ImGui::Text("Chains:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%d", (int)*edgeChainManager->GetEdgeChainCount());

				ImGui::SameLine();
				ImGui::Text(" Vertices:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "%d", (int)*edgeChainManager->GetEdgeChainVertexCount());
				ImGui::TreePop();
			}

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Edge Chains", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Separator();

			float windowWidth = ImGui::GetWindowContentRegionWidth();

			// Add new edge chain
            int i = 4;
            ImGui::PushID(i);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));

			if (ImGui::Button("Add Edge Chain", ImVec2(windowWidth, 30)))
				edgeChainManager->PushChain();

			ImGui::PopStyleColor(3);
            ImGui::PopID();
			ImGui::Separator();

			// Enable edge chains
			ImGui::AlignTextToFramePadding();
            ImGui::Text("Enable");

			ImGui::SameLine();
			ImGui::HelpMarker("Toggle edge chain collisions on and off.");

            ImGui::SetNextItemWidth(-1);
			ImGui::SameLine((windowWidth/2) * 0.8f);
			if (ImGui::Checkbox("##EnableEdgeChains", &edgeChainManager->GetEnableFlag()))
				edgeChainManager->SyncEnable();

			// Select edge chain
			ImGui::AlignTextToFramePadding();
            ImGui::Text("Selected");

			ImGui::SameLine();
			ImGui::HelpMarker("Select an edge chain that exits in the level.");

            ImGui::SetNextItemWidth(-1);
			ImGui::SameLine((windowWidth/2) * 0.8f);
			if (edgeChainManager->GetChainCount() > 0) {
				if (ImGui::Combo("##SelectedEdgeChain", &edgeChainManager->GetSelectedChainIndex(), edgeChainManager->GetChainLabels())) {
					edgeChainManager->SelectCurrentChain();
				}
			}
			else {
				ImGui::Text("--");
			}

			{
				float buttonHeight = 27.5f;

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
				ImGui::BeginChild("SelectedChainChild", ImVec2(0, 94.f), true, ImGuiWindowFlags_None);

				ImVec4 subColor(.45f,.45f,.45f,1.f);
				ImGui::TextColored(subColor, "Selected Edge Chain Options");

				// Delete selected edge chain
				i = 0;
				ImGui::PushID(i+21);
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
				if (ImGui::Button("Delete Selected Chain", ImVec2(windowWidth, buttonHeight)))
					edgeChainManager->PopChain();
				ImGui::PopStyleColor(3);
				ImGui::PopID();

				// Add Vertex
				float buttonWidth = (windowWidth*0.475f);

				i = 4;
				ImGui::PushID(i+22);
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
				if (ImGui::Button("Add Vertex", ImVec2(buttonWidth, buttonHeight)))
					edgeChainManager->AddVertexToSelectedChain();
				ImGui::PopStyleColor(3);
				ImGui::PopID();

				// Remove Vertex
				ImGui::PushID(i+23);
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
				ImGui::SameLine(windowWidth*.525f);
				if (ImGui::Button("Remove Vertex", ImVec2(buttonWidth, buttonHeight)))
					edgeChainManager->RemoveVertexFromSelectedChain();
				ImGui::PopStyleColor(3);
				ImGui::PopID();

				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
		}

		if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
		{
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

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
			ImGui::BeginChild("ControlsChild", ImVec2(0, 145.f), true, ImGuiWindowFlags_None);

			for (auto& control : controls)
			{
				ImVec4 itemColor(1.f, 1.f, .4f, 1.f);
				ImGui::TextColored(itemColor, control.first.data());
				ImGui::SameLine(80);
				ImGui::Text(control.second.data());
			}

			ImGui::EndChild();
			ImGui::PopStyleVar();
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
