#include "utils/sprite_manager.hpp"
#include "utils/constants.hpp"

using sf::Vector2f;
using sf::Event;
using sf::Keyboard;
using sf::Mouse;
using sf::RenderWindow;

unsigned int SpriteManager::DynamicBodiesCount = 0;

SpriteManager::SpriteManager(b2World* world, const Vector2f& resolution)
{
	m_world = world;
	m_resolution = resolution;
	m_destroyFlag = false;
	m_wireframeMode = false;
	m_rmbPressed = false;
}

SpriteManager::~SpriteManager()
{
	DestroyAllShapes();
}

void SpriteManager::PushShape(const ShapeType type, const Vector2f& position)
{
	switch (type)
	{
	case ShapeType::DebugBox:
		m_debugShapes.push_back(
			dynamic_cast<DebugShape*>(new DebugBox(position, m_world)));
		++DynamicBodiesCount;
		break;

	case ShapeType::DebugCircle:
		m_debugShapes.push_back(
			dynamic_cast<DebugShape*>(new DebugCircle(position, m_world))
		);
		++DynamicBodiesCount;
		break;
	case ShapeType::CustomPolygon:
		m_debugShapes.push_back(
			dynamic_cast<DebugShape*>(new CustomPolygon(position,
				demo_data::customPolygonCoords, m_world)));
		++DynamicBodiesCount;
		break;
	default:
		break;
	}
}

void SpriteManager::HandleInput(const Event& event, RenderWindow& window)
{
	// Handle wireframe toggle for custom polygons
	if (event.type == Event::KeyReleased)
    {
		// W key: toggle custom polygon wireframe
		if (event.key.code == Keyboard::W)
		{
			m_wireframeMode = !m_wireframeMode;

			for (auto& shape : m_debugShapes)
			{
				if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
					polygon->SetWireframe(m_wireframeMode);
			}
		}
	}

	// Handle test point
	if (event.type == Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == Mouse::Right &&
			EditorSettings::mode == RMBMode::TestPointMode)
		{
			m_rmbPressed = true;
			DoTestPoint(window);
		}
	}

	if (event.type == Event::MouseMoved)
	{
		if (m_rmbPressed && EditorSettings::mode == RMBMode::TestPointMode)
		{
			DoTestPoint(window);
		}
	}

	if (event.type == Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == Mouse::Right &&
			EditorSettings::mode == RMBMode::TestPointMode)
		{
			ResetTestPoint();
		}
	}

	// TODO: Handle ray cast
	// I.e., Update line and do ray cast if RMB is being held and
	// editor is in RayCastMode
}

void SpriteManager::DoTestPoint(RenderWindow& window)
{
	for (auto& shape : m_debugShapes)
	{
		if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
		{
			box->DoTestPoint(GetMousePosition(window));
		}
		else if (DebugCircle* circle = dynamic_cast<DebugCircle*>(shape))
		{
			circle->DoTestPoint(GetMousePosition(window));
		}
		else if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
		{
			polygon->DoTestPoint(GetMousePosition(window));
		}
	}
}

void SpriteManager::ResetTestPoint()
{
	m_rmbPressed = false;

	for (auto& shape : m_debugShapes)
	{
		if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
		{
			box->ResetTestPoint();
		}
		else if (DebugCircle* circle = dynamic_cast<DebugCircle*>(shape))
		{
			circle->ResetTestPoint();
		}
		else if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
		{
			polygon->ResetTestPoint();
		}
	}
}

bool* SpriteManager::GetWireframeFlag()
{
	return &m_wireframeMode;
}

void SpriteManager::ToggleWireframe()
{
	for (auto& shape : m_debugShapes)
	{
		if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
			polygon->SetWireframe(m_wireframeMode);
	}
}

void SpriteManager::Update()
{
	/* Check destroy flag */
	if (m_destroyFlag)
	{
		m_destroyFlag = false;
		DestroyAllShapes();
		return;
	}

	/* Update debug shapes */
	for (auto& shape : m_debugShapes)
	{
		if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
		{
			box->Update();
		}
		else if (DebugCircle* circle = dynamic_cast<DebugCircle*>(shape))
		{
			circle->Update();
		}
		else if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
		{
			polygon->Update();
		}

		// Mark shapes for removal
		float offset = 50.f;
		Vector2f pos;

		// Custom polygon requires b2Body to get its world position
		if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
			pos = polygon->GetBodyPosition();
		else
			pos = shape->GetPosition();

		if (pos.x < 0.f || pos.x > (m_resolution.x + offset) ||
			pos.y < 0.f || pos.y > (m_resolution.y + offset))
		{
			shape->MarkForDelete(true);
		}
	}

	/* De-allocate shapes marked for remove */
	m_debugShapes.erase(
		std::remove_if(
			m_debugShapes.begin(),
			m_debugShapes.end(),
			[](DebugShape* shape)
			{
				if (shape->IsMarkedForDelete())
				{
					--DynamicBodiesCount;

					if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
					{
						SafeDelete(box);
						return true;
					}
					else if (DebugCircle* circle = dynamic_cast<DebugCircle*>(shape))
					{
						SafeDelete(circle);
						return true;
					}
					else if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
					{
						SafeDelete(polygon);
						return true;
					}
					else {
						SafeDelete(shape);
						return true;
					}
				}

				return false;
			}),
		m_debugShapes.end());
}

void SpriteManager::Draw(RenderWindow& window)
{
	for (auto& shape : m_debugShapes)
	{
		if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
		{
			box->Draw(window);
		}
		else if (DebugCircle* box = dynamic_cast<DebugCircle*>(shape))
		{
			box->Draw(window);
		}
		else if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
		{
			polygon->Draw(window);
		}
	}
}

void SpriteManager::DestroyAllShapes()
{
	for (auto& shape : m_debugShapes)
	{
		--DynamicBodiesCount;

		if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
		{
			SafeDelete(box);
		}
		else if (DebugCircle* circle = dynamic_cast<DebugCircle*>(shape))
		{
			SafeDelete(circle);
		}
		else if (CustomPolygon* polygon = dynamic_cast<CustomPolygon*>(shape))
		{
			SafeDelete(polygon);
		}
		else
		{
			SafeDelete(shape);
		}
	}

	m_debugShapes.clear();
}

void SpriteManager::SetDestroryFlag(bool flag)
{
	m_destroyFlag = flag;
}