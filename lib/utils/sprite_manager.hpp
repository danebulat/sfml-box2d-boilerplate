#ifndef SPRITE_MANAGER_HPP
#define SPRITE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include "utils/debug_box.hpp"
#include "utils/debug_circle.hpp"
#include "utils/custom_polygon.hpp"
#include "utils/box2d_utils.hpp"

enum class ShapeType
{
	DebugBox = 1,
	DebugCircle,
	CustomPolygon
};

class SpriteManager
{
private:
	std::vector<DebugShape*>	m_debugShapes;
	sf::Vector2f 				m_resolution;
	b2World* 					m_world;
	bool						m_destroyFlag;

public:
	static unsigned int DynamicBodiesCount;

	SpriteManager(b2World* world, const sf::Vector2f& resolution);
	~SpriteManager();

	SpriteManager(const SpriteManager&) = delete;
	SpriteManager& operator= (const SpriteManager&) = delete;

	void PushShape(const ShapeType type, const sf::Vector2f& position);

	void Update();
	void Draw(sf::RenderWindow& window);

	void DestroyAllShapes();
	void SetDestroryFlag(bool flag);
};

#endif