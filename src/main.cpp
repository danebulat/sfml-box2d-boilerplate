#include "Platform/Platform.hpp"
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>

/* Utils */
#include "editor/mouse_utils.hpp"

/* Demos */
#include "editor/debug/demo_pulley_joint.hpp"

/* Callbacks */
#include "editor/callbacks/my_contact_listener.hpp"
#include "editor/callbacks/trigger_zone.hpp"

/* Managers */
#include "editor/managers/camera_manager.hpp"
#include "editor/managers/imgui_manager.hpp"
#include "editor/managers/drag_cache_manager.hpp"

using namespace physics;

// Set initial editor mode
RMBMode EditorSettings::mode = RMBMode::PanCameraMode;
sf::Vector2u EditorSettings::levelSize;
sf::Vector2f EditorSettings::RESOLUTION;

int main(int argc, char** argv)
{
	util::Platform platform;

	/* Seed random number generator */
	std::srand(time(0));

	/** Prepare the window */
	EditorSettings::RESOLUTION.x = VideoMode::getDesktopMode().width * .9f;
    EditorSettings::RESOLUTION.y = VideoMode::getDesktopMode().height * .75f;

	/* Resolution and level size */
	sf::Vector2f res = EditorSettings::RESOLUTION;
	sf::Vector2f levelSize = EditorSettings::RESOLUTION * 2.f;
	EditorSettings::levelSize = sf::Vector2u((uint)levelSize.x, (uint)levelSize.y);

	/* Render window */
	sf::RenderWindow window(sf::VideoMode(res.x, res.y, 32),
		"SFML - Box2D Boilerplate", sf::Style::Default);
	window.setFramerateLimit(60);

	/** Prepare the world */
	b2Vec2 gravity(0.f, 9.8f);
	std::unique_ptr<b2World> world(new b2World(gravity));

	/* Instantiate b2ContactListener */
	MyContactListener contactListener;
	world->SetContactListener(&contactListener);

	/* Instantiate b2ContactFilter */
	//MyContactFilter filter;
	//world->SetContactFilter(&filter);

	/* Instantiate b2DestructionListener */
	MyDestructionListener destructionListener;
	world->SetDestructionListener(&destructionListener);

	/* TriggerZome (b2QueryCallback) */
	TriggerZone trigger(sf::Vector2f(100.f, 100.f), sf::Vector2f(100.f,100.f));
	bool doQuery = false;

	/* Create edge chain manager */
	std::shared_ptr<EdgeChainManager> edgeChainManager(new EdgeChainManager(world.get()));

	/* Create sprite manager */
	std::shared_ptr<SpriteManager> spriteManager(new SpriteManager(world.get()));

	/* The grid */
	std::shared_ptr<Grid> grid(new Grid(res, levelSize));

	/* Camera manager */
	std::shared_ptr<CameraManager> cameraManager(new CameraManager());

	/* ImGui manager (initialise ImGui) */
	std::unique_ptr<ImGuiManager> imguiManager(
		new ImGuiManager(window, edgeChainManager, spriteManager,
			cameraManager->GetCamera(), grid));

	sf::Clock clock;

	/* TMP */
	//DebugBox box(sf::Vector2f(200.f, 200.f), &world);
	bool forceOn = false;
	bool torqueOn = false;

	/* Joint Demos */
	DemoPulleyJoint demo_pulleyJoint(world.get());

	while (window.isOpen())
	{
		sf::Time dt = clock.restart();

		sf::Event event;
		while (window.pollEvent(event))
		{
			// Process ImGui events
			imguiManager->ProcessEvent(event);

			// Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();

			if (event.type == sf::Event::KeyReleased)
            {
				// Escape key: exit
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();

				// E key: toggle static edge shape
				if (event.key.code == sf::Keyboard::E)
				{
					edgeChainManager->ToggleEnable();
					edgeChainManager->SyncEnable();
				}

				// Space key: add new custom polygon
				if (event.key.code == sf::Keyboard::Enter)
				{
					spriteManager->PushShape(ShapeType::CustomPolygon, GetMousePosition(window));
				}

				if (event.key.code == sf::Keyboard::Up) {
					forceOn = !forceOn;
				}
				if (event.key.code == sf::Keyboard::Down) {
					//box.ApplyLinearImpulse();
				}
				if (event.key.code == sf::Keyboard::Right) {
					torqueOn = !torqueOn;
				}
				if (event.key.code == sf::Keyboard::Left) {
					//box.ApplyAngularImpulse();
				}

				if (event.key.code == sf::Keyboard::Q)
				{
					/* Register b2QueryCallback */
					doQuery = true;
				}
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

			// Handle grid inputs
			grid->HandleInput(event, window);

			// Handle manager inputs
			cameraManager->HandleInput(event);
			edgeChainManager->HandleInput(event, window);
			spriteManager->HandleInput(event, window);

			// Handle triggers
			trigger.HandleInput(event);

		}// end window.poll(event)

		/* Update ImGui */
		imguiManager->Update(window, dt);

		/* Update camera */
		cameraManager->Update(window, dt);

		/* Update dragging object cache */
		DragCacheManager::UpdateCache();

		/** Update Box2D */
		world->Step(1/60.f, 8, 3);

		/* Update managers */
		edgeChainManager->Update(window);
		spriteManager->Update();

		if (doQuery)
		{
			doQuery = false;
			trigger.Query(world.get());
		}

		/* Update demos */
		demo_pulleyJoint.Update();

		/* Update trigger zones */
		trigger.Update(window);

		/*----------------------------------------------------------------------
         Draw
         ----------------------------------------------------------------------*/
		window.clear(sf::Color::White);

		/* Draw grid */
		grid->Draw(window);

		window.setView(cameraManager->GetCameraView());

		/* Render triggers */
		trigger.Draw(window);

		/* Draw objects */
		spriteManager->Draw(window);
		edgeChainManager->Draw(window);

		/* Update demos */
		demo_pulleyJoint.Draw(window);

		if (imguiManager->RenderMouseCoords())
			grid->DrawMouseLabel(window);

		// Render ImGui windows
		imguiManager->Render(window);

		window.display();
	}

	// clean up ImGui, such as deleting the internal font atlas
    imguiManager->Shutdown();

	return 0;
}
