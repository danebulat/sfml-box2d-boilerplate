#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

#include "Platform/Platform.hpp"
#include "editor/managers/imgui_manager.hpp"
#include "editor/mouse_utils.hpp"
#include <string>
#include <algorithm>

#include <ctime>
#include <cstdlib>

using namespace physics;

struct CircleSprite
{
	sf::CircleShape m_sprite;
	sf::Vector2f	m_position;

	CircleSprite()
	{
		m_sprite.setFillColor(sf::Color::White);
		m_sprite.setOutlineColor(sf::Color::Black);
		m_sprite.setOutlineThickness(2.f);
		m_sprite.setRadius(CIRCLE_RADIUS);
		m_sprite.setOrigin(CIRCLE_RADIUS/2, CIRCLE_RADIUS/2);
	}

	void SetPosition(const sf::Vector2f& position)
	{
		m_position = position;
		m_sprite.setPosition(m_position);
	}
};

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
	b2World world(gravity);

	/* Create edge chain manager */
	std::shared_ptr<EdgeChainManager> edgeChainManager(new EdgeChainManager(&world));

	/* Create sprite manager */
	std::shared_ptr<SpriteManager> spriteManager(new SpriteManager(&world));

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

	/* TMP - Distance Joint */
	b2Body* bodyA = nullptr;
	b2Body* bodyB = nullptr;
	b2Joint* joint = nullptr;
	CircleSprite circleSprite1;
	CircleSprite circleSprite2;
	joint = InitDistanceJoint(&world);

	if (joint != nullptr)
	{
		std::cout << "joint created\n";

		bodyA = joint->GetBodyA();
		bodyB = joint->GetBodyB();

		if (bodyA != nullptr && bodyB != nullptr)
		{
			std::cout << "bodyA and bodyB initialised\n";
		}
	}

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

		/* Center view on camera position */
		if (camera->IsAnimating())
			cameraTarget = camera->GetPosition();

		/** Update camera */
		camera->Update(dt.asSeconds(), cameraTarget);

		view.setCenter(camera->GetPosition());

		/** Update Box2D */
		world.Step(1/60.f, 8, 3);

		if (bodyA != nullptr && bodyB != nullptr)
		{
			b2Vec2 apos = joint->GetBodyA()->GetWorldCenter();
			b2Vec2 bpos = joint->GetBodyB()->GetWorldCenter();

			std::cout << "bodyA: (" << apos.x*SCALE << "," << apos.y*SCALE << ") ";
			std::cout << "bodyB: (" << bpos.x*SCALE << "," << bpos.y*SCALE << ")\n";

			circleSprite1.m_position.x = apos.x * SCALE;
			circleSprite1.m_position.y = apos.y * SCALE;
			circleSprite1.SetPosition(sf::Vector2f(apos.x*SCALE, apos.y*SCALE));

			circleSprite2.m_position.x = bpos.x * SCALE;
			circleSprite2.m_position.y = bpos.y * SCALE;
			circleSprite2.SetPosition(sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE));
		}

		/* Update managers */
		edgeChainManager->Update(window);
		spriteManager->Update();

		/*----------------------------------------------------------------------
         Draw
         ----------------------------------------------------------------------*/
		window.clear(sf::Color::White);

		/* Draw grid */
		grid->Draw(window);

		window.setView(view);

		/* Draw objects */
		spriteManager->Draw(window);
		edgeChainManager->Draw(window);

		/* TMP */
		//box.Draw(window);
		window.draw(circleSprite1.m_sprite);
		window.draw(circleSprite2.m_sprite);

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
