#ifndef STATIC_EDGE_CHAIN_HPP
#define STATIC_EDGE_CHAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "box2d/box2d.h"
#include "utils/bounding_box.hpp"
#include "utils/move_handle.hpp"

/* Draggable point handle to edit edge chain vertices */
struct VertexHandle
{
	float 			m_size = 10.f;
	unsigned int 	m_vertexIndex;	// the vertex it refers to
	sf::Vector2f    m_position;		// world position
	sf::CircleShape m_sprite;
	sf::Color 		m_color;
	sf::Color 		m_hoverColor;
};

/* Static edge chain */
class StaticEdgeChain
{
private:
	std::string 			  		m_tag;
	unsigned int 			  		m_vertexCount;
	std::vector<sf::Vector2f> 		m_vertices;

	sf::Color    			  		m_color;
	sf::VertexArray			  		m_vertexArray;

	std::shared_ptr<BoundingBox>	m_boundingBox;
	bool 					  		m_drawBoundingBox;
	bool 					  		m_updateBoundingBox;

	std::vector<b2Vec2>		  		m_scaledVertices;
	b2Body*					 		m_body;

	// Handle data
	sf::Vector2f 			  		m_prevMousePosition;
	bool 					  		m_mouseMoved;
	bool 					  		m_leftMouseDown;

	// Vertex handle data
	std::vector<VertexHandle> 		m_vertexHandles;
	VertexHandle*			  		m_selectedHandle;
	bool					  		m_editable;
	bool					  		m_hoveringOnHandle;

	// Move handle data
	std::shared_ptr<MoveHandle>		m_moveHandle;
	bool 					  		m_hoveringOnMoveHandle;

private:
	void InitVertexHandles();

public:
	StaticEdgeChain();
	StaticEdgeChain(std::vector<sf::Vector2f>& vertices, const std::string& tag, b2World* world);
	~StaticEdgeChain();

	void Init(std::vector<sf::Vector2f>& vertices, b2World* world);
	void DeleteBody(b2World*);

	void SetColor(const sf::Color color);

	void SetEnabled(bool enabled);
	bool IsEnabled() const;

	void SetEditable(bool editable);
	bool IsEditable() const;

	const std::string& GetTag() const;
	void SetTag(const std::string& tag);

	void DrawBoundingBox(bool flag);

	void Update(sf::RenderWindow& window);
	void Draw(sf::RenderWindow& window);
	void HandleInput(const sf::Event& event, sf::RenderWindow& window);
};

#endif