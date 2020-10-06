#ifndef SPRITE_MANAGER_HPP
#define SPRITE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include "editor/debug/debug_box.hpp"
#include "editor/debug/debug_circle.hpp"
#include "editor/debug/custom_polygon.hpp"
#include "editor/debug/multi_shape.hpp"
#include "editor/box2d_utils.hpp"

enum class ShapeType
{
	DebugBox = 1,
	DebugCircle,
	CustomPolygon,
	MultiShape
};

class SpriteManager
{
private:
	std::vector<DebugShape*>	m_debugShapes;
	b2World* 					m_world;
	bool						m_destroyFlag;

	bool 						m_wireframeMode;
	bool						m_rmbPressed;

public:
	static unsigned int DynamicBodiesCount;

	SpriteManager(b2World* world);
	~SpriteManager();

	SpriteManager(const SpriteManager&) = delete;
	SpriteManager& operator= (const SpriteManager&) = delete;

	void PushShape(const ShapeType type, const sf::Vector2f& position);

	void HandleInput(const sf::Event& event, sf::RenderWindow& window);
	void Update();
	void Draw(sf::RenderWindow& window);

	void DestroyAllShapes();
	void SetDestroryFlag(bool flag);

	bool* GetWireframeFlag();
	void ToggleWireframe();

	void DoTestPoint(RenderWindow& window);
	void ResetTestPoint();
};

#endif