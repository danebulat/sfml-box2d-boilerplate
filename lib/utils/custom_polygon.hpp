#ifndef CUSTOM_POLYGON_HPP
#define CUSTOM_POLYGON_HPP

#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <vector>
#include "utils/constants.hpp"

class CustomPolygon
{
private:
	sf::Vector2f 			  m_startPos;	// spawn position in world coords
	std::vector<sf::Vector2f> m_vertices;	// vertex data in world coords
	std::string 			  m_tag;		// for body custom data

	sf::VertexArray 		  m_vertexArray;
	sf::Color       		  m_color;

	unsigned int 		      m_vertexCount;
	bool					  m_wireframe;
	b2Body* 				  m_body;

public:
	CustomPolygon()
		: m_color(sf::Color::Magenta)
		, m_vertexCount(0)
		, m_wireframe(false)
		, m_body(nullptr)
	{}

	~CustomPolygon()
	{} // body deallocated by b2World

	void Init(std::vector<sf::Vector2f> vertices, const sf::Vector2f& startPos, b2World* world)
	{
		m_vertices = vertices;
		m_vertexCount = m_vertices.size();
		m_startPos = startPos;

		// Resize SFML vertex array and set color
		if (!m_wireframe)
			m_vertexArray.setPrimitiveType(sf::TriangleFan);
		else
			m_vertexArray.setPrimitiveType(sf::LineStrip);

		m_vertexArray.resize(m_vertexCount + 1);
		for (std::size_t i = 0; i < m_vertexCount + 1; ++i)
			m_vertexArray[i].color = m_color;

		// Create Box2D scaled vertices
		b2Vec2 scaledVertices[m_vertexCount];
		for (std::size_t i = 0; i < m_vertexCount; ++i)
		{
			scaledVertices[i].Set(m_vertices[i].x / SCALE, m_vertices[i].y / SCALE);
		}

		// Create polygon shape
		b2PolygonShape shape;
		shape.Set(scaledVertices, m_vertexCount);

		// Create fixture definition and bind shape
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.f;
		fixtureDef.shape = &shape;

		// Create body
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(m_startPos.x / SCALE, m_startPos.y / SCALE);

		if (!m_tag.empty())
			bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(&m_tag);

		m_body = world->CreateBody(&bodyDef);

		// Create fixture (bind shape to body)
		m_body->CreateFixture(&fixtureDef);
	}

	void SetColor(const sf::Color& color)
	{
		m_color = color;
	}

	void SetTag(const std::string& tag)
	{
		m_tag = tag;
	}

	void SetWireframe(bool wireframe)
	{
		m_wireframe = wireframe;
		if (m_wireframe)
			m_vertexArray.setPrimitiveType(sf::LineStrip);
		else
			m_vertexArray.setPrimitiveType(sf::TriangleFan);
	}

	bool IsWireframe() const
	{
		return m_wireframe;
	}

	void Draw(sf::RenderWindow& window)
	{
		b2Fixture* fixture = m_body->GetFixtureList();

		if (fixture->GetType() == b2Shape::e_polygon)
		{
			// Get polygon shape to access vertices
			b2PolygonShape* shape =
				dynamic_cast<b2PolygonShape*>(fixture->GetShape());

			// Update SFML vertex array
			for (std::size_t i = 0; i < m_vertexCount; ++i)
			{
				b2Vec2 point = m_body->GetWorldPoint(shape->m_vertices[i]);
				m_vertexArray[i].position = sf::Vector2f(point.x * SCALE, point.y * SCALE);
			}

			// Connect last vertex with first vertex for a closed shape
			b2Vec2 firstPoint = m_body->GetWorldPoint(shape->m_vertices[0]);
			m_vertexArray[m_vertexCount].position =
				sf::Vector2f(firstPoint.x * SCALE, firstPoint.y * SCALE);
		}

		// Render vertex array to window
		window.draw(m_vertexArray);
	}
};

#endif