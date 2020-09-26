#ifndef VERTEX_HANDLE_HPP
#define VERTEX_HANDLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class VertexHandle
{
private:
	sf::CircleShape	m_sprite;
	sf::Vector2f	m_position;
	sf::Color		m_color;
	sf::Color		m_hoverColor;

	// Radius of circle sprite
	float			m_size;

	// Index of the vertex in passed buffer handle is associated with
	unsigned int	m_vertexIndex;

	// Flag to toggle hovered and unhovered state
	bool			m_hoveredState;

public:
	VertexHandle(sf::Vector2f& vertex, unsigned int index);

	void SetHoveredState(bool flag);

	unsigned int GetVertexIndex() const;
	sf::Vector2f GetPosition() const;
	float GetSize() const;

	void Update(const sf::Vector2f& moveIncrement);
	void Draw(sf::RenderWindow& window);
};

#endif