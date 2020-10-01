#ifndef GRID_HPP
#define GRID_HPP

#include <SFML/Graphics.hpp>

class Grid
{
private:
	sf::Vector2f		m_resolution;
	sf::Color 			m_color;
	sf::VertexArray		m_vertexArrayX;
	sf::VertexArray		m_vertexArrayY;
	float 				m_unitSize;

	static constexpr float MIN_UNIT_SIZE = 1.f;
	static constexpr float MAX_UNIT_SIZE = 250.f;

private:
	void BuildVertexArrayX()
	{
		m_vertexArrayX.clear();

		// Resize vertex array X
		uint gridLinesX = (m_resolution.x / m_unitSize) + 1;
		uint vertexCountX = gridLinesX * 2;
		m_vertexArrayX.resize(vertexCountX);
		m_vertexArrayX.setPrimitiveType(sf::Lines);

		// Build vertical grid lines
		for (uint x = 0; x < gridLinesX; ++x)
		{
			sf::Vector2f position(x * m_unitSize, 0.f);
			m_vertexArrayX[x].position = position;
			m_vertexArrayX[x].color = m_color;

			++x;
			position.y = m_resolution.y;

			m_vertexArrayX[x].position = position;
			m_vertexArrayX[x].color = m_color;
		}
	}

	void BuildVertexArrayY()
	{
		m_vertexArrayY.clear();

		// Resize vertex array Y
		uint gridLinesY = (m_resolution.y / m_unitSize) + 1;
		uint vertexCountY = gridLinesY * 2;
		m_vertexArrayY.resize(vertexCountY);
		m_vertexArrayY.setPrimitiveType(sf::Lines);

		// Build horizontal grid lines
		for (uint y = 0; y < gridLinesY; ++y)
		{
			sf::Vector2f position(0.f, y * m_unitSize);
			m_vertexArrayY[y].position = position;
			m_vertexArrayY[y].color = m_color;

			++y;
			position.x = m_resolution.x;

			m_vertexArrayY[y].position = position;
			m_vertexArrayY[y].color = m_color;
		}
	}

public:
	Grid(const sf::Vector2f& resolution)
		: m_resolution(resolution)
		, m_color(sf::Color(194.f, 194.f, 214.f, 96.f))
		, m_unitSize(20.f)
	{

		BuildVertexArrayX();
		BuildVertexArrayY();
	}

	~Grid()
	{}

	void Update()
	{}

	void Draw(sf::RenderWindow& window)
	{
		// Draw vertex arrays
		window.draw(m_vertexArrayX);
		window.draw(m_vertexArrayY);
	}

	void SetUnitSize(float size)
	{
		if (size <= MAX_UNIT_SIZE && size >= MIN_UNIT_SIZE)
		{
			m_unitSize = size;
			BuildVertexArrayX();
			BuildVertexArrayY();
		}
	}

	void IncrementUnitSize(float size)
	{
		if ((m_unitSize + size) <= MAX_UNIT_SIZE && (m_unitSize + size) >= MIN_UNIT_SIZE)
		{
			m_unitSize += size;
			BuildVertexArrayX();
			BuildVertexArrayY();
		}
	}

	float GetUnitSize() const
	{
		return m_unitSize;
	}
};

#endif