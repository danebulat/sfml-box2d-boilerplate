#ifndef STATIC_EDGE_CHAIN_HPP
#define STATIC_EDGE_CHAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "box2d/box2d.h"
#include "utils/constants.hpp"

class StaticEdgeChain
{
private:
	unsigned int 			  m_vertexCount;
	std::vector<sf::Vector2f> m_vertices;

	sf::Color    			  m_color;
	sf::VertexArray			  m_vertexArray;

	std::vector<b2Vec2>		  m_scaledVertices;
	b2Body*					  m_body;

public:
	StaticEdgeChain()
		: m_vertexCount(0)
		, m_color(sf::Color::Blue)
		, m_body(nullptr)
	{}

	void Init(std::vector<sf::Vector2f> vertices, b2World* world)
	{
		m_vertexCount = vertices.size();
		m_vertices = vertices;

		// Initialise SFML vertex array
		m_vertexArray.setPrimitiveType(sf::LinesStrip);
		m_vertexArray.resize(m_vertexCount);
		for (std::size_t i = 0; i < m_vertexCount; ++i)
		{
			m_vertexArray[i].position = m_vertices[i];
			m_vertexArray[i].color = m_color;
		}

		// Initialise scaled vertex array
		m_scaledVertices.resize(m_vertexCount);
		for (std::size_t i = 0; i < m_vertexCount; ++i)
		{
			m_scaledVertices[i].x = m_vertices[i].x / SCALE;
			m_scaledVertices[i].y = m_vertices[i].y / SCALE;
		}

		// Initialise chain shape
		b2ChainShape chain;
		b2Vec2 prevVertex((m_vertices[0].x - 10.f) / SCALE,
						  (m_vertices[0].y) / SCALE);

		b2Vec2 nextVertex((m_vertices[m_vertexCount].x + 10.f) / SCALE,
					      (m_vertices[m_vertexCount].y) / SCALE);

		chain.CreateChain(m_scaledVertices.data(),
						  m_vertexCount,
						  prevVertex, nextVertex);

		// Create fixture and body
		b2FixtureDef fixture;
		fixture.density = 0.f;
		fixture.shape = &chain;

		b2BodyDef bodyDef;
		bodyDef.position.Set(0, 0);
		bodyDef.type = b2_staticBody;

		b2Body* body = world->CreateBody(&bodyDef);
		body->CreateFixture(&fixture);

		m_body = body;
	}

	void SetColor(const sf::Color color)
	{
		m_color = color;
	}

	void SetEnabled(bool enabled)
	{
		if (!enabled)
			m_color.a = 64.f;
		else
			m_color.a = 255.f;

		for (std::size_t i = 0; i < m_vertexCount; ++i)
			m_vertexArray[i].color = m_color;

		m_body->SetEnabled(enabled);
	}

	bool IsEnabled() const
	{
		return m_body->IsEnabled();
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_vertexArray);
	}
};

#endif