#ifndef STATIC_EDGE_CHAIN_HPP
#define STATIC_EDGE_CHAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "box2d/box2d.h"

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
	void InitVertexHandles();

public:
	StaticEdgeChain();

	void Init(std::vector<sf::Vector2f> vertices, b2World* world);

	void SetColor(const sf::Color color);
	void SetEnabled(bool enabled);

	bool IsEnabled() const;
	void SetEditable(bool editable);

	void Draw(sf::RenderWindow& window);
	void HandleInput(const sf::Event& event, sf::RenderWindow& window);
};

#endif