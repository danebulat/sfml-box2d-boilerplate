#ifndef IMGUI_MANAGER_HPP
#define IMGUI_MANAGER_HPP

#include "imgui.h"
#include "imgui-SFML.h"

#include "editor/managers/edge_chain_manager.hpp"
#include "editor/managers/sprite_manager.hpp"
#include "editor/animation/camera.hpp"
#include "editor/grid.hpp"
#include <vector>
#include <string>

class ImGuiManager
{
private:
	std::shared_ptr<EdgeChainManager> p_edgeChainManager;
	std::shared_ptr<SpriteManager> 	  p_spriteManager;
	std::shared_ptr<Camera> 	      p_camera;
	std::shared_ptr<Grid>			  p_grid;

	/** General settings */
	static int e;
	static bool render_mouse_coords;

	/** Grid settings */
	static int   grid_style;
	static bool  show_grid_settings;
	static float grid_unit_size;
	static float grid_color[4];
	static bool  show_grid;

	/** Camera settings */
	static bool  show_camera_settings;
	static bool  enable_clamp;
	static float tween_duration;
	static int   combo_index;
	static std::vector<std::string> easing_labels;

private:
	static void InitEasingLabels(std::vector<std::string>& vec);

public:
	ImGuiManager(sf::RenderWindow& window,
		std::shared_ptr<EdgeChainManager> edgeChainManager,
		std::shared_ptr<SpriteManager> spriteManager,
		std::shared_ptr<Camera> camera,
		std::shared_ptr<Grid> grid);

	void ProcessEvent(sf::Event& event);
	void Update(sf::RenderWindow& window, sf::Time& dt);
	void Render(sf::RenderWindow& window);
	void Shutdown();

	bool RenderMouseCoords();
};

#endif