#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "imgui.h"
#include "imgui-SFML.h"

#include "Platform/Platform.hpp"
#include "editor/managers/edge_chain_manager.hpp"
#include "editor/managers/sprite_manager.hpp"
#include "editor/debug/custom_polygon.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/grid.hpp"
#include <string>
#include <algorithm>

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
	RESOLUTION.x = VideoMode::getDesktopMode().width * .9f;
    RESOLUTION.y = VideoMode::getDesktopMode().height * .75f;

	sf::RenderWindow window(sf::VideoMode(RESOLUTION.x, RESOLUTION.y, 32),
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

	/* The grid */
	std::unique_ptr<Grid> grid(new Grid(RESOLUTION));

	sf::Clock clock;
	sf::View view;
	view.setSize(RESOLUTION.x, RESOLUTION.y);
	view.setCenter(RESOLUTION.x * .5f, RESOLUTION.y * .5f);

	std::cout << "(" << RESOLUTION.x  << "," << RESOLUTION.y << ")\n";

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

				// TMP - GRID
				if (event.key.code == sf::Keyboard::Up)
				{
					grid->IncrementUnitSize(2.f);
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					grid->IncrementUnitSize(-2.f);
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
			ImGui::HalfWidthLabelCheckoxLeft("Mouse Coords", "##MouseCoords",
				"Display mouse coordinates next to mouse pointer.", &renderMouseCoords);

			if (ImGui::HalfWidthLabelCheckoxRight("Wireframe", "##Wireframe",
				"Render custom polygons in a wireframe mode.", spriteManager->GetWireframeFlag())) {
				spriteManager->ToggleWireframe();
			}

			ImGui::HalfWidthLabelCheckoxLeft("Bounding Boxes",
				"##BoundingBoxes", "Display bounding boxes around the selected edge chain.", edgeChainManager->GetDrawBBFlag());

			bool tmp = true;
			ImGui::HalfWidthLabelCheckoxRight("Labels", "##Labels",
				"TODO: Display edge labels.", &tmp);
			ImGui::Separator();
		}

		/* Bodies Settings */
		if (ImGui::CollapsingHeader("Bodies", ImGuiTreeNodeFlags_DefaultOpen))
		{
			sf::Vector2f      shapeStartPos(RESOLUTION.x * .25f, RESOLUTION.y * .25f);
			float windowWidth = ImGui::GetWindowContentRegionWidth();
			float btnwidth    = (windowWidth*.25f) - ((windowWidth*.225f)*.05f);

			ImGui::Separator();

			// Clear all (red)
			if (ImGui::StartColorButton(1, 0, "Clear All", btnwidth, 30.f, false))
				spriteManager->SetDestroryFlag(true);
			ImGui::StopColorButton();

			// +Box (blue)
			if (ImGui::StartColorButton(2, 4, "+Box", btnwidth, 30.f, true))
				spriteManager->PushShape(ShapeType::DebugBox, shapeStartPos);
			ImGui::StopColorButton();

			// +Circle (blue)
			if (ImGui::StartColorButton(3, 4, "+Circle", btnwidth, 30.f, true))
				spriteManager->PushShape(ShapeType::DebugCircle, shapeStartPos);
			ImGui::StopColorButton();

			// +Polygon (blue)
			if (ImGui::StartColorButton(4, 4, "+Polygon", btnwidth, 30.f, true))
				spriteManager->PushShape(ShapeType::CustomPolygon, shapeStartPos);
			ImGui::StopColorButton();

			/* Dynamic bodies count */
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

			/* Debug bodies count */
			ImGui::Separator();
			ImGui::SetNextTreeNodeOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::TreeNode("Debug Shapes"))
			{
				ImVec4 lightBlue(.6f, .8f, 1.f, 1.f);
				ImGui::SameLine();
				ImGui::HelpMarker("Statistics for debug shape objects.");

				ImGui::LabelWithColoredFloat("Boxes:", lightBlue, DebugShape::DebugBoxCount, false);
				ImGui::LabelWithColoredFloat(" Circles:", lightBlue, DebugShape::DebugCircleCount, true);
				ImGui::LabelWithColoredFloat(" Polygons:", lightBlue, DebugShape::CustomPolygonCount, true);
				ImGui::LabelWithColoredFloat("Total:", lightBlue, DebugShape::ShapeBodyCount, false);
				ImGui::TreePop();
			}

			/* Edge chain and vertex count */
			ImGui::Separator();
			ImGui::SetNextTreeNodeOpen(true, ImGuiTreeNodeFlags_DefaultOpen);
			if (ImGui::TreeNode("Edge Chains"))
			{
				ImVec4 lightBlue(.6f, .8f, 1.f, 1.f);
				ImGui::SameLine();
				ImGui::HelpMarker("Statistics for edge chain objects.");

				ImGui::LabelWithColoredFloat("Chains:", lightBlue, (int)*edgeChainManager->GetEdgeChainCount(), false);
				ImGui::LabelWithColoredFloat(" Vertices:", lightBlue, (int)*edgeChainManager->GetEdgeChainVertexCount(), true);
				ImGui::TreePop();
			}
			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Edge Chains", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Separator();
			float windowWidth = ImGui::GetWindowContentRegionWidth();

			// Add new edge chain
			if (ImGui::StartColorButton(11, 4, "Add Edge Chain", windowWidth, 30.f, false))
				edgeChainManager->PushChain();
			ImGui::StopColorButton();

			ImGui::Separator();

			// Enable edge chains
			if (ImGui::HalfWidthLabelCheckoxLeft("Enable", "##EnableEdgeChains",
				"Toggle edge chain collisions on and off.", edgeChainManager->GetEnableFlag())) {
				edgeChainManager->SyncEnable();
			}

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
				if (ImGui::StartColorButton(21, 0, "Delete Selected Chain", windowWidth, buttonHeight, false))
					edgeChainManager->PopChain();
				ImGui::StopColorButton();

				float buttonWidth = (windowWidth*0.475f);

				// Add Vertex
				if (ImGui::StartColorButton(22, 4, "Add Vertex", buttonWidth, buttonHeight, false))
					edgeChainManager->AddVertexToSelectedChain();
				ImGui::StopColorButton();

				// Remove Vertex
				if (ImGui::StartColorButton(23, 4, "Remove Vertex", buttonWidth, buttonHeight, true))
					edgeChainManager->RemoveVertexFromSelectedChain();
				ImGui::StopColorButton();

				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
		}

		if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::RenderEditorControls();
        }

		ImGui::End();

		/*----------------------------------------------------------------------
         Update
         ----------------------------------------------------------------------*/

		/** Update Box2D */
		world.Step(1/60.f, 8, 3);

		/* Update managers */
		edgeChainManager->Update(window);
		spriteManager->Update();

		/*----------------------------------------------------------------------
         Draw
         ----------------------------------------------------------------------*/

		window.clear(sf::Color::White);

		/* Draw grid */
		grid->Draw(window);

		/* Draw objects */
		window.setView(view);
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
