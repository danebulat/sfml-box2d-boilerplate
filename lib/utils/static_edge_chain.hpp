#ifndef STATIC_EDGE_CHAIN_HPP
#define STATIC_EDGE_CHAIN_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "box2d/box2d.h"
#include "utils/constants.hpp"
#include "utils/mouse_utils.hpp"

/* Draggable point handle to edit edge chain vertices */
struct VertexHandle
{
	float 			m_size = 10.f;
	unsigned int 	m_vertexIndex;	// the vertex it refers to
	sf::Vector2f    m_position;		// world position
	sf::CircleShape m_sprite;
	sf::Color 		m_color;
	sf::Color		m_hoverColor;
};

/* Static edge chain */
class StaticEdgeChain
{
private:
	unsigned int 			  m_vertexCount;
	std::vector<sf::Vector2f> m_vertices;

	sf::Color    			  m_color;
	sf::VertexArray			  m_vertexArray;

	std::vector<b2Vec2>		  m_scaledVertices;
	b2Body*					  m_body;

	std::vector<VertexHandle> m_vertexHandles;
	bool					  m_editable;
	VertexHandle*			  m_selectedHandle;

	bool 					  m_leftMouseDown;
	bool					  m_hoveringOnHandle;

private:
	void InitVertexHandles()
	{
		for (int i = 0; i < m_vertices.size(); ++i)
		{
			VertexHandle vHandle;
			vHandle.m_color = sf::Color(0.f, 0.f, 255.f, 96.f);
			vHandle.m_hoverColor = sf::Color(255.f, 0.f, 0, 128.f);
			vHandle.m_position.x = m_vertices[i].x;
			vHandle.m_position.y = m_vertices[i].y;

			vHandle.m_sprite.setRadius(vHandle.m_size);
			vHandle.m_sprite.setFillColor(vHandle.m_color);
			vHandle.m_sprite.setOrigin(vHandle.m_size, vHandle.m_size);
			vHandle.m_sprite.setPosition(m_vertices[i].x, m_vertices[i].y);
			vHandle.m_vertexIndex = i;

			m_vertexHandles.push_back(vHandle);
		}
	}

public:
	StaticEdgeChain()
		: m_vertexCount(0)
		, m_color(sf::Color::Blue)
		, m_body(nullptr)
		, m_editable(false)
		, m_selectedHandle(nullptr)
		, m_leftMouseDown(false)
		, m_hoveringOnHandle(false)
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
		b2Vec2 prevVertex((m_vertices[0].x - 100.f) / SCALE,
						  (m_vertices[0].y) / SCALE);

		b2Vec2 nextVertex((m_vertices[m_vertexCount].x + 100.f) / SCALE,
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

		// Initialise vertex handles
		InitVertexHandles();
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

	void SetEditable(bool editable)
	{
		m_editable = editable;
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_vertexArray);

		// Draw vertex handles
		if (m_editable)
		{
			for (auto& handle : m_vertexHandles)
				window.draw(handle.m_sprite);
		}
	}

	void HandleInput(const sf::Event& event, sf::RenderWindow& window)
	{
		// MouseButtonPressed
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_leftMouseDown = true;
			}
		}

		// MouseButtonReleased
		if (event.type == sf::Event::MouseButtonReleased)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_leftMouseDown = false;
			}
		}

		// MouseMoved
		if (event.type == sf::Event::MouseMoved)
		{
			if (m_editable)
			{
				// Get mouse position
				sf::Vector2f mousePos = GetMousePosition(window);

				// Handle previous frame before updating hovering flag
				if (m_hoveringOnHandle && m_leftMouseDown)
				{
					m_selectedHandle->m_position = mousePos;
					m_selectedHandle->m_sprite.setPosition(m_selectedHandle->m_position);

					// Disable body before updating vertices
					m_body->SetEnabled(false);

					// TODO: Destroy body and re-create new chain
					b2Fixture* fixture = m_body->GetFixtureList();
					if (fixture->GetType() == b2Shape::e_chain)
					{
						b2ChainShape* shape = dynamic_cast<b2ChainShape*>(fixture->GetShape());

						shape->m_vertices[m_selectedHandle->m_vertexIndex].Set(
							m_selectedHandle->m_position.x / SCALE,
							m_selectedHandle->m_position.y / SCALE);
					}

					// Enable body after updating its data
					m_body->SetEnabled(true);

					// Update sf::VertexArray
					m_vertexArray[m_selectedHandle->m_vertexIndex].position = m_selectedHandle->m_position;
				}

				// Handle gets selected automatically if the mouse is hovering over it
				m_hoveringOnHandle = false;

				for (int i = 0; i < m_vertexHandles.size(); ++i)
				{
					sf::Vector2f handlePos = m_vertexHandles[i].m_position;
					float radius = m_vertexHandles[i].m_size;

					// Check if mouse is hovering over a vertex handle
					if (((mousePos.x > handlePos.x - radius) && (mousePos.x < handlePos.x + radius)) &&
						((mousePos.y > handlePos.y - radius) && (mousePos.y < handlePos.y + radius)))
					{
						m_vertexHandles[i].m_sprite.setFillColor(m_vertexHandles[i].m_hoverColor);

						// If another vertex is already selected, unselect it
						if (m_selectedHandle != nullptr && m_selectedHandle != &m_vertexHandles[i])
							m_selectedHandle->m_sprite.setFillColor(m_selectedHandle->m_color);

						// Cache the hovered handle
						if (m_selectedHandle != &m_vertexHandles[i])
						{
							m_selectedHandle = &m_vertexHandles[i];
							std::cout << "handle selected\n";
						}

						// Cache mouse position
						m_hoveringOnHandle = true;
						break;
					}
					else
					{
						m_vertexHandles[i].m_sprite.setFillColor(m_vertexHandles[i].m_color);
					}
				}// end for

				if (!m_hoveringOnHandle)
				{
					m_selectedHandle = nullptr;
				}
			}// end editable
		}
	}
};

#endif