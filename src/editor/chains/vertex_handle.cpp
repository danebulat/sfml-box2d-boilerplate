#include "editor/chains/vertex_handle.hpp"

using sf::Vector2f;
using sf::Color;
using sf::CircleShape;
using sf::RenderWindow;

VertexHandle::VertexHandle(Vector2f& vertex, unsigned int index)
{
	m_size 		   = 10.0f;
	m_vertexIndex  = index;
	m_position 	   = vertex;
	m_hoveredState = false;
	m_color 	   = Color(0.f, 0.f, 255.f, 64.f);
	m_hoverColor   = Color(0.f, 0.f, 255.f, 128.f);

	m_sprite.setRadius(m_size);
	m_sprite.setFillColor(m_color);
	m_sprite.setOutlineColor(m_hoverColor);
	m_sprite.setOutlineThickness(1.f);
	m_sprite.setOrigin(m_size, m_size);
	m_sprite.setPosition(m_position);
}

VertexHandle::~VertexHandle()
{}

void VertexHandle::SetHoveredState(bool flag)
{
	m_hoveredState = flag;
}

unsigned int VertexHandle::GetVertexIndex() const
{
	return m_vertexIndex;
}

Vector2f VertexHandle::GetPosition() const
{
	return m_position;
}

float VertexHandle::GetSize() const
{
	return m_size;
}

void VertexHandle::Update(const Vector2f& moveIncrement)
{
	m_position += moveIncrement;
	m_sprite.setPosition(m_position);
}

void VertexHandle::Draw(RenderWindow& window)
{
	if (!m_hoveredState)
		m_sprite.setFillColor(m_color);
	else
		m_sprite.setFillColor(m_hoverColor);

	window.draw(m_sprite);
}