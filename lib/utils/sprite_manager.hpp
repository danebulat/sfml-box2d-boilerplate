#ifndef SPRITE_MANAGER_HPP
#define SPRITE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include "utils/debug_box.hpp"
#include "utils/debug_circle.hpp"
#include "utils/box2d_utils.hpp"

enum class ShapeType
{
	DebugBox = 1,
	DebugCircle
};

class SpriteManager
{
private:
	std::vector<DebugShape*>	m_debugShapes;
	b2World* 					m_world;
	bool						m_destroyFlag;

public:
	static unsigned int DynamicBodiesCount;

	SpriteManager(b2World* world)
	{
		m_world = world;
		m_destroyFlag = false;
	}

	~SpriteManager()
	{
		DestroyAllShapes();
	}

	SpriteManager(const SpriteManager&) = delete;
	SpriteManager& operator= (const SpriteManager&) = delete;

	void PushShape(const ShapeType type, const sf::Vector2f& position)
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
		default:
			break;
		}
	}

	void Update()
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

			// Mark shapes for removal
			float offset = 50.f;
			sf::Vector2f pos = shape->GetPosition();
			if (pos.x < 0.f || pos.x > (RESOLUTION.x + offset) ||
				pos.y < 0.f || pos.y > (RESOLUTION.y + offset))
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
					// TODO: SafeDelete() util
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
						else {
							SafeDelete(shape);
							return true;
						}
					}

					return false;
				}),
			m_debugShapes.end());
	}

	void Draw(sf::RenderWindow& window)
	{
		for (auto& shape : m_debugShapes)
		{
			if (DebugBox* box = dynamic_cast<DebugBox*>(shape))
			{
				box->Draw(window);
			}
			if (DebugCircle* box = dynamic_cast<DebugCircle*>(shape))
			{
				box->Draw(window);
			}
		}
	}

	void DestroyAllShapes()
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
			else
			{
				SafeDelete(shape);
			}
		}

		m_debugShapes.clear();
	}

	void SetDestroryFlag(bool flag)
	{
		m_destroyFlag = flag;
	}
};

unsigned int SpriteManager::DynamicBodiesCount = 0;

#endif