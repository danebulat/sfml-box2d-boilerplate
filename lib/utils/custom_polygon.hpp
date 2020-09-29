#ifndef CUSTOM_POLYGON_HPP
#define CUSTOM_POLYGON_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include "utils/debug_shape.hpp"
#include "utils/constants.hpp"

class CustomPolygon : public DebugShape
{
private:
	std::vector<sf::Vector2f> m_vertices;	// vertex data in world coords

	sf::VertexArray 		  m_vertexArray;
	sf::Color       		  m_color;

	unsigned int 		      m_vertexCount;
	bool					  m_wireframe;
	b2Body* 				  m_body;
	b2World* 				  m_world;

private:
	void SetVertexColor(const sf::Color& color);

public:
	CustomPolygon(const sf::Vector2f& position,
		const std::vector<sf::Vector2f>& vertices, b2World* world);

	virtual ~CustomPolygon();

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