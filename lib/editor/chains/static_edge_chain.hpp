#ifndef STATIC_EDGE_CHAIN_HPP
#define STATIC_EDGE_CHAIN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "box2d/box2d.h"
#include "editor/chains/bounding_box.hpp"
#include "editor/chains/move_handle.hpp"
#include "editor/chains/vertex_handle.hpp"
#include "editor/managers/chain_manager_controller.hpp"

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

	b2Body*					 		m_body;

	// Manager
	ChainManagerController*			m_manager;

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

	// Add and remove vertex flags
public:
	bool m_addVertex;
	bool m_removeVertex;

private:
	static void GetChainGhostVertices(b2Vec2& p, b2Vec2& n,
		std::vector<sf::Vector2f>& vertices);

	static sf::Vector2f GetNextAddedVertexPosition(
		std::vector<sf::Vector2f>& vertices);

	void BuildBody(b2World* world, const sf::Vector2f& position);

public:
	StaticEdgeChain(ChainManagerController* manager);
	StaticEdgeChain(std::vector<sf::Vector2f>& vertices, const std::string& tag,
		b2World* world, ChainManagerController* manager);
	~StaticEdgeChain();

	void Init(std::vector<sf::Vector2f>& vertices, b2World* world);
	void DeleteBody(b2World* world);

	void AddVertex(b2World* world);
	void RemoveVertex(b2World* world);

	void SetColor(const sf::Color color);

	void SetEnabled(bool enabled);
	bool IsEnabled() const;

	void SetEditable(bool editable);
	bool IsEditable() const;

	const std::string& GetTag() const;
	void SetTag(const std::string& tag);

	sf::FloatRect GetMoveHandleLabelRect() const;
	void DrawBoundingBox(bool flag);

	void SetAddVertexFlag(bool flag);
	void SetRemoveVertexFlag(bool flag);

	long GetVertexCount();

	void Update(sf::RenderWindow& window, b2World* world);
	void Draw(sf::RenderWindow& window);
	void HandleInput(const sf::Event& event, sf::RenderWindow& window);
};

#endif