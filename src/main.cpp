#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

#include "Platform/Platform.hpp"
#include "editor/mouse_utils.hpp"
#include <string>
#include <algorithm>

#include <ctime>
#include <cstdlib>

/* Demos */
#include "editor/debug/demo_distance_joint.hpp"
#include "editor/debug/demo_revolute_joint.hpp"
#include "editor/debug/demo_prismatic_joint.hpp"
#include "editor/debug/demo_pulley_joint.hpp"

/* Callbacks */
#include "editor/callbacks/my_contact_listener.hpp"
#include "editor/callbacks/trigger_zone.hpp"

/* Managers */
#include "editor/managers/imgui_manager.hpp"
#include "editor/managers/drag_cache_manager.hpp"

using namespace physics;

sf::Vector2f prevMousePos;
bool rmbPressed = false;
bool panCamera = false;

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

	/* Mouse data */
	sf::Text mouseLabel;
	mouseLabel.setFont(FontStore::GetInstance()->GetFont("DroidSansMono.ttf"));
	mouseLabel.setFillColor(sf::Color::Black);
	mouseLabel.setString("(X,Y)");
	mouseLabel.setCharacterSize(12);

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

	/* The camera */
	sf::View view(sf::FloatRect(0, 0, res.x, res.y));
	sf::Vector2f cameraTarget(res.x/2, res.y/2);

	std::shared_ptr<Camera> camera(new Camera(cameraTarget, EditorSettings::levelSize, res, true));
	camera->SetDuration(.5f);
	camera->SetInterpolation(InterpFunc::ExpoEaseOut);

	/* ImGui manager (initialise ImGui) */
	std::unique_ptr<ImGuiManager> imguiManager(
		new ImGuiManager(window, edgeChainManager, spriteManager, camera, grid));

	sf::Clock clock;

	/* TMP */
	//DebugBox box(sf::Vector2f(200.f, 200.f), &world);
	bool forceOn = false;
	bool torqueOn = false;

	/* Joint Demos */
	DemoDistanceJoint demo_distanceJoint(world.get());
	//DemoRevoluteJoint demo_revoluteJoint(&world);
	//DemoPrismaticJoint demo_prismaticJoint(&world);
	//DemoPulleyJoint demo_pulleyJoint(world.get());

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

			// Mouse Button Pressed
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					if (EditorSettings::mode == RMBMode::PanCameraMode)
						rmbPressed = true;
				}
			}

			// Mouse move
			if (event.type == sf::Event::MouseMoved)
			{
				SetMouseLabel(mouseLabel, window);

				// Handle camera pan
				if (EditorSettings::mode == RMBMode::PanCameraMode && rmbPressed)
				{
					panCamera = true;
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
					rmbPressed = false;
					panCamera = false;

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

			// Handle demo inputs
			demo_distanceJoint.HandleInput(event);
			//demo_prismaticJoint.HandleInput(event);

			// Handle triggers
			trigger.HandleInput(event);

		}// end window.poll(event)

		/* Update ImGui */
		imguiManager->Update(window, dt);

		/*----------------------------------------------------------------------
         Update
         ----------------------------------------------------------------------*/
		// if (forceOn)
		// 	box.ApplyForce();
		// if (torqueOn)
		// 	box.ApplyTorque();
		// box.Update();

		/* WEIRD CODE ****************************************************************/
		// By simply calculating the increment of the current/previous mouse position
		// each frame results in strange behavior due to the way the view is centered
		// every frame.
		// The following solution updates the current mouse position WITH THE INCREMENT
		// value BEFORE copying it over to the previous frame mouse position.
		sf::Vector2f mousePos = GetMousePosition(window);

		if (panCamera)
		{
			float incX = abs(mousePos.x - prevMousePos.x);
			float incY = abs(mousePos.y - prevMousePos.y);

			if (mousePos.x < prevMousePos.x) {
				cameraTarget.x += incX;
				mousePos.x += incX;
			}
			else {
				// move negative x
				cameraTarget.x -= incX;
				mousePos.x -= incX;
			}

			if (mousePos.y < prevMousePos.y) {
				cameraTarget.y += incY;
				mousePos.y += incY;
			}
			else {
				// move negative y
				cameraTarget.y -= incY;
				mousePos.y -= incY;
			}
		}

		prevMousePos = mousePos;
		/* END WEIRD CODE ************************************************************/

		/* Update dragging object cache */
		DragCacheManager::UpdateCache();

		/* Center view on camera position */
		if (camera->IsAnimating())
			cameraTarget = camera->GetPosition();

		/** Update camera */
		camera->Update(dt.asSeconds(), cameraTarget);

		view.setCenter(camera->GetPosition());

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
		demo_distanceJoint.Update(window);
		//demo_revoluteJoint.Update();
		//demo_prismaticJoint.Update(window);
		//demo_pulleyJoint.Update();

		/* Update trigger zones */
		trigger.Update(window);

		/*----------------------------------------------------------------------
         Draw
         ----------------------------------------------------------------------*/
		window.clear(sf::Color::White);

		/* Draw grid */
		grid->Draw(window);

		window.setView(view);

		/* Render triggers */
		trigger.Draw(window);

		/* Draw objects */
		spriteManager->Draw(window);
		edgeChainManager->Draw(window);

		/* Update demos */
		demo_distanceJoint.Draw(window);
		//demo_revoluteJoint.Draw(window);
		//demo_prismaticJoint.Draw(window);
		//demo_pulleyJoint.Draw(window);

		if (imguiManager->RenderMouseCoords())
			window.draw(mouseLabel);

		// Render ImGui windows
		imguiManager->Render(window);

		window.display();
	}

	// clean up ImGui, such as deleting the internal font atlas
    imguiManager->Shutdown();

	return 0;
}
