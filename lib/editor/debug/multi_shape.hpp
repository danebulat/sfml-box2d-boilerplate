#ifndef MULTI_SHAPE_HPP
#define MULTI_SHAPE_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include "editor/debug/debug_shape.hpp"
#include "editor/constants.hpp"

class MultiShape : public DebugShape
{
private:
	bool					  m_wireframe;
	b2Body* 				  m_body;
	b2World* 				  m_world;

	std::vector<sf::Vector2f> m_shape1;
	std::vector<sf::Vector2f> m_shape2;
	sf::VertexArray			  m_va1;
	sf::VertexArray			  m_va2;
	bool 					  m_multipleFixture;

private:
	void CreateMultipleFixtureBody();
	void SetVertexColor(const sf::Color& color);

public:
	MultiShape(const sf::Vector2f& position, b2World* world);

	virtual ~MultiShape();

	void CreateBody();
	void DeleteBody();

	void SetColor(const sf::Color& color);
	void SetWireframe(bool wireframe);
	bool IsWireframe() const;
	sf::Vector2f GetBodyPosition() const;

	virtual void Update() override;
	virtual void Draw(sf::RenderWindow& window) override;

	void DoTestPoint(const sf::Vector2f& point);
	void ResetTestPoint();
};

#endif