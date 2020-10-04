#include "editor/managers/imgui_manager.hpp"
#include "imgui/imgui_utils.hpp"

using std::vector;
using std::string;
using std::shared_ptr;

using sf::RenderWindow;
using sf::Color;
using sf::Vector2f;
using sf::Event;
using sf::Keyboard;
using sf::Time;

/** General settings */
int  ImGuiManager::mode_index            = 1;
bool ImGuiManager::render_mouse_coords   = true;

/** Grid settings */
int   ImGuiManager::grid_style 			 = 0;
bool  ImGuiManager::show_grid_settings 	 = false;
float ImGuiManager::grid_unit_size 		 = 2.5f;
float ImGuiManager::grid_color[4] 		 = { 1.f,0.f,0.f,0.f };
bool  ImGuiManager::show_grid 			 = true;

/** Camera settings */
vector<string> ImGuiManager::easing_labels;
bool  ImGuiManager::show_camera_settings = true;
bool  ImGuiManager::enable_clamp         = true;
float ImGuiManager::tween_duration       = .5f;
int   ImGuiManager::combo_index          = 1;

ImGuiManager::ImGuiManager(RenderWindow& window,
	shared_ptr<EdgeChainManager> edgeChainManager,
	shared_ptr<SpriteManager> spriteManager,
	shared_ptr<Camera> camera,
	shared_ptr<Grid> grid)
{
	ImGui::SFML::Init(window);

	p_edgeChainManager = edgeChainManager;
	p_spriteManager = spriteManager;
	p_camera = camera;
	p_grid = grid;

	/** Initialise grid data */
	grid_unit_size = p_grid->GetUnitSize();
	Color col = p_grid->GetLineColor();
	grid_color[0] = col.r / 255.f;
	grid_color[1] = col.g / 255.f;
	grid_color[2] = col.b / 255.f;
	grid_color[3] = col.a / 255.f;
	show_grid = p_grid->IsVisible();

	/** Initialise camera data */
	enable_clamp   = p_camera->ClampEnabled();
	combo_index    = static_cast<int>(p_camera->GetInterpolation());
	tween_duration = p_camera->GetDuration();
	InitEasingLabels(easing_labels);
}

void ImGuiManager::ProcessEvent(sf::Event& event)
{
	ImGui::SFML::ProcessEvent(event);

	if (event.type == Event::KeyReleased)
	{
		// Space key: Traverse RMB mode
		if (event.key.code == Keyboard::Space)
		{
			int mode = (static_cast<int>(EditorSettings::mode) + 1);
			if (mode > 3)
			{
				EditorSettings::mode = RMBMode::PanCameraMode;
				mode_index = 1;
			}
			else
			{
				EditorSettings::mode = static_cast<RMBMode>(mode);
				mode_index = mode;
			}
		}
	}
}

void ImGuiManager::Update(RenderWindow& window, Time& dt)
{
	ImGui::SFML::Update(window, dt);

	ImGui::Begin("Box2D Level Editor");
	UpdateMainWindow();
	UpdateGridWindow();
	UpdateCameraWindow();
	ImGui::End();
}

/** Grid Settings Panel
 */
void ImGuiManager::UpdateGridWindow()
{
	if (show_grid_settings) // window toggle flag
	{
		if (!ImGui::Begin("Grid Settings", &show_grid_settings)) {
			ImGui::End();
		}
		else
		{
			ImGui::Separator();
			ImGui::SetWindowSize(ImVec2(300.f, 205.f));
			if (ImGui::FullWidthLabelCheckox("Display",
				"##GridDisplay", "Toggle visibility of the grid", &show_grid)) {
				p_grid->IsVisible(show_grid);
			}

			ImGui::Separator();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Grid Style:   ");

			ImGui::SameLine();
			if (ImGui::RadioButton("Standard", &grid_style, 0))
				p_grid->SetType(static_cast<GridType>(grid_style));

			ImGui::SameLine();
			if (ImGui::RadioButton("Crosshair", &grid_style, 1))
				p_grid->SetType(static_cast<GridType>(grid_style));

			ImGui::Separator();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Unit Size:    ");
			ImGui::SameLine();
			if (ImGui::SliderFloat("float", &grid_unit_size, 10.f, 250.f))
				p_grid->SetUnitSize(grid_unit_size);

			ImGui::Separator();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Line Color:   ");

			ImGui::SetNextItemWidth(-1);
			if (ImGui::ColorEdit3("##GridLineColor", grid_color, ImGuiColorEditFlags_PickerHueWheel))
			{
					Color c;
					c.r = static_cast<sf::Uint8>(grid_color[0] * 255.f);
					c.g = static_cast<sf::Uint8>(grid_color[1] * 255.f);
					c.b = static_cast<sf::Uint8>(grid_color[2] * 255.f);
					c.a = 96.f;
					p_grid->SetLineColor(c);
			}
			ImGui::Separator();

			// Reset grid settings
			float width = ImGui::GetWindowContentRegionWidth();
			if (ImGui::StartColorButton(31, 4, "Reset Settings", width, 30.f, false)) {
				p_grid->Reset();
				Color c = p_grid->GetLineColor();
				grid_color[0] = c.r / 255.f;
				grid_color[1] = c.g / 255.f;
				grid_color[2] = c.b / 255.f;
				grid_color[3] = c.a / 255.f;
			}
			ImGui::StopColorButton();
			ImGui::Separator();
			ImGui::End();
		}
	}
}

/** Camera Settings Panel
*/
void ImGuiManager::UpdateCameraWindow()
{
	if (show_camera_settings)
	{
		if (!ImGui::Begin("Camera Settings", &show_camera_settings)) {
			ImGui::End();
		}
		else
		{
			ImGui::Separator();
			ImGui::SetWindowSize(ImVec2(300.f, 190.f));

			if (ImGui::FullWidthLabelCheckox("Clamp to Level Size",
				"##CameraClamping", "If enabled, the camera will not scroll past the level size", &enable_clamp)) {
				p_camera->ClampEnabled(enable_clamp);
			}

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Easing Function"); ImGui::SameLine();
			ImGui::HelpMarker("Easing function applied to the camera animation when moving to a newly selected object.");
			ImGui::SetNextItemWidth(-1);
			if (ImGui::Combo("##CameraEasingFunction", &combo_index, easing_labels)) {
				p_camera->SetInterpolation(static_cast<InterpFunc>(combo_index));
			}

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Tween Duration"); ImGui::SameLine();
			ImGui::HelpMarker("How long in seconds camera will animate to a newly selected object.");
			ImGui::SetNextItemWidth(-1);
			if (ImGui::SliderFloat("##CameraTweenDuration", &tween_duration, 0.2f, 15.f, "%.1f secs")) {
				p_camera->SetDuration(tween_duration);
			}

			// Reset grid settings
			float width = ImGui::GetWindowContentRegionWidth();
			if (ImGui::StartColorButton(31, 4, "Reset Settings", width, 30.f, false)) {
				tween_duration = .5f;
				p_camera->SetDuration(tween_duration);

				enable_clamp = true;
				p_camera->ClampEnabled(enable_clamp);

				combo_index = static_cast<int>(InterpFunc::ExpoEaseOut);
				p_camera->SetInterpolation(InterpFunc::ExpoEaseOut);
			}
			ImGui::StopColorButton();
			ImGui::Separator();
			ImGui::End();
		}
	}
}

/** Main Window
*/
void ImGuiManager::UpdateMainWindow()
{
	/* Demo Settings */
	if (ImGui::CollapsingHeader("Editor Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Separator();
		ImGui::Text("RMB Mode:");
		if (ImGui::RadioButton("Pan Camera", &mode_index, 1))
			EditorSettings::mode = RMBMode::PanCameraMode;

		ImGui::SameLine();
		if (ImGui::RadioButton("Spawn Box", &mode_index, 2))
			EditorSettings::mode = RMBMode::BoxSpawnMode;

		ImGui::SameLine();
		if (ImGui::RadioButton("Test Point", &mode_index, 3))
			EditorSettings::mode = RMBMode::TestPointMode;
		ImGui::Separator();

		// Grid Options (blue)
		float width = ImGui::GetWindowContentRegionWidth()-6.f;
		if (ImGui::StartColorButton(41, 4, "Grid Options", width/2, 30.f, false))
			show_grid_settings = !show_grid_settings;
		ImGui::StopColorButton();

		// Camera Options (blue)
		if (ImGui::StartColorButton(42, 4, "Camera Options", width/2, 30.f, true))
			show_camera_settings = !show_camera_settings;
		ImGui::StopColorButton();
		ImGui::Separator();
	}

	/* Display Settings */
	if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Separator();
		ImGui::HalfWidthLabelCheckoxLeft("Mouse Coords", "##MouseCoords",
			"Display mouse coordinates next to mouse pointer.", &render_mouse_coords);

		if (ImGui::HalfWidthLabelCheckoxRight("Wireframe", "##Wireframe",
			"Render custom polygons in a wireframe mode.", p_spriteManager->GetWireframeFlag())) {
			p_spriteManager->ToggleWireframe();
		}

		ImGui::HalfWidthLabelCheckoxLeft("Bounding Boxes",
			"##BoundingBoxes", "Display bounding boxes around the selected edge chain.", p_edgeChainManager->GetDrawBBFlag());

		bool tmp = true;
		ImGui::HalfWidthLabelCheckoxRight("Labels", "##Labels", "TODO: Display edge labels.", &tmp);
		ImGui::Separator();
	}

	/* Bodies Settings */
	if (ImGui::CollapsingHeader("Bodies", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector2f      shapeStartPos(EditorSettings::RESOLUTION.x * .25f, EditorSettings::RESOLUTION.y * .25f);
		float windowWidth = ImGui::GetWindowContentRegionWidth();
		float btnwidth    = (windowWidth*.25f) - ((windowWidth*.225f)*.05f);

		ImGui::Separator();

		// Clear all (red)
		if (ImGui::StartColorButton(1, 0, "Clear All", btnwidth, 30.f, false))
			p_spriteManager->SetDestroryFlag(true);
		ImGui::StopColorButton();

		// +Box (blue)
		if (ImGui::StartColorButton(2, 4, "+Box", btnwidth, 30.f, true))
			p_spriteManager->PushShape(ShapeType::DebugBox, shapeStartPos);
		ImGui::StopColorButton();

		// +Circle (blue)
		if (ImGui::StartColorButton(3, 4, "+Circle", btnwidth, 30.f, true))
			p_spriteManager->PushShape(ShapeType::DebugCircle, shapeStartPos);
		ImGui::StopColorButton();

		// +Polygon (blue)
		if (ImGui::StartColorButton(4, 4, "+Polygon", btnwidth, 30.f, true))
			p_spriteManager->PushShape(ShapeType::CustomPolygon, shapeStartPos);
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

			ImGui::LabelWithColoredFloat("Chains:", lightBlue, (int)*p_edgeChainManager->GetEdgeChainCount(), false);
			ImGui::LabelWithColoredFloat(" Vertices:", lightBlue, (int)*p_edgeChainManager->GetEdgeChainVertexCount(), true);
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
		{
			Vector2f worldPos = p_camera->GetPosition();
			p_edgeChainManager->PushChain(worldPos);
		}

		ImGui::StopColorButton();
		ImGui::Separator();

		// Enable edge chains
		if (ImGui::HalfWidthLabelCheckoxLeft("Enable", "##EnableEdgeChains",
			"Toggle edge chain collisions on and off.", p_edgeChainManager->GetEnableFlag())) {
			p_edgeChainManager->SyncEnable();
		}

		// Select edge chain
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Selected");

		ImGui::SameLine();
		ImGui::HelpMarker("Select an edge chain that exits in the level.");

		ImGui::SetNextItemWidth(-1);
		ImGui::SameLine((windowWidth/2) * 0.8f);
		if (p_edgeChainManager->GetChainCount() > 0) {
			if (ImGui::Combo("##SelectedEdgeChain", &p_edgeChainManager->GetSelectedChainIndex(), p_edgeChainManager->GetChainLabels())) {
				p_edgeChainManager->SelectCurrentChain();

				/* Animate camera towards selected chain */
				Vector2f pos = p_edgeChainManager->GetSelectedEdgeChain().GetPosition();
				p_camera->AnimateTo(pos);
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
				p_edgeChainManager->PopChain();
			ImGui::StopColorButton();

			float buttonWidth = (windowWidth*0.475f);

			// Add Vertex
			if (ImGui::StartColorButton(22, 4, "Add Vertex", buttonWidth, buttonHeight, false))
				p_edgeChainManager->AddVertexToSelectedChain();
			ImGui::StopColorButton();

			// Remove Vertex
			if (ImGui::StartColorButton(23, 4, "Remove Vertex", buttonWidth, buttonHeight, true))
				p_edgeChainManager->RemoveVertexFromSelectedChain();
			ImGui::StopColorButton();

			ImGui::EndChild();
			ImGui::PopStyleVar();
		}
	}

	if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::RenderEditorControls();
	}
}

void ImGuiManager::Render(RenderWindow& window)
{
	ImGui::SFML::Render(window);
}

void ImGuiManager::Shutdown()
{
	ImGui::SFML::Shutdown();
}

bool ImGuiManager::RenderMouseCoords()
{
	return render_mouse_coords;
}

void ImGuiManager::InitEasingLabels(std::vector<std::string>& vec) {
	vec = {
		"Linear",
		"Quad Ease In",
		"Quad Ease Out",
		"Quad Ease In Out",
		"Cubic Ease In",
		"Cubic Ease Out",
		"Cubic Ease In Out",
		"Quart Ease In",
		"Quart Ease Out",
		"Quart Ease In Out",
		"Quint Ease In",
		"Quint Ease Out",
		"Quint Ease In Out",
		"Sine Ease In",
		"Sine Ease Out",
		"Sine Ease In Out",
		"Expo Ease In",
		"Expo Ease Out",
		"Expo Ease In Out",
		"Circ Ease In",
		"Circ Ease Out",
		"Circ Ease In Out",
		"Back Ease In",
		"Back Ease Out",
		"Back Ease In Out",
		"Elastic Ease In",
		"Elastic Ease Out",
		"Elastic Ease In Out",
		"Bounce Ease In",
		"Bounce Ease Out",
		"Bounce Ease In Out"
	}; // end vector
}