#ifndef GRID_HPP
#define GRID_HPP

#include <SFML/Graphics.hpp>
#include "editor/font_store.hpp"

enum class GridType
{
	STANDARD,
	CROSS_HAIR
};

class Grid
{
private:
	sf::Vector2f		m_resolution;
	sf::Color 			m_color;
	sf::VertexArray		m_vertexArray;

	GridType 			m_type;
	float 				m_unitSize;
	float 				m_crossHairSize;

	sf::View			m_gridHud;
	sf::Text 			m_xLabel;
	sf::Text 			m_yLabel;

	static constexpr float MIN_UNIT_SIZE = 1.f;
	static constexpr float MAX_UNIT_SIZE = 250.f;

private:
	void BuildStandardGrid()
	{
		m_vertexArray.clear();
		m_vertexArray.setPrimitiveType(sf::Lines);

		uint pointsX = (m_resolution.x / m_unitSize) + 1;
		uint pointsY = (m_resolution.y / m_unitSize) + 1;

		m_vertexArray.resize((pointsX * 2) + (pointsY * 2));

		uint i = 0;
		for (uint x = 0; x < pointsX; ++x)
		{
			sf::Vector2f position(x * m_unitSize, 0.f);
			m_vertexArray[i].position = position;
			m_vertexArray[i++].color = m_color;

			position.y = m_resolution.y;

			m_vertexArray[i].position = position;
			m_vertexArray[i++].color = m_color;
		}

		for (uint y = 0; y < pointsY; ++y)
		{
			sf::Vector2f position(0.f, y*m_unitSize);
			m_vertexArray[i].position = position;
			m_vertexArray[i++].color = m_color;

			position.x = m_resolution.x;

			m_vertexArray[i].position = position;
			m_vertexArray[i++].color = m_color;
		}
	}

	void BuildCrossHairGrid()
	{
		m_vertexArray.clear();
		m_vertexArray.setPrimitiveType(sf::Lines);

		uint pointsX = (m_resolution.x / m_unitSize) + 1;
		uint pointsY = (m_resolution.y / m_unitSize) + 1;

		m_vertexArray.resize((pointsX * pointsY) * 4);

		float l = m_crossHairSize;
		uint i = 0;
		for (uint x = 0; x < pointsX; ++x)
		{
			for (uint y = 0; y < pointsY; ++y)
			{
				sf::Vector2f p(x*m_unitSize, y*m_unitSize);
				m_vertexArray[i].position = sf::Vector2f(p.x-l, p.y);
				m_vertexArray[i++].color = m_color;

				m_vertexArray[i].position = sf::Vector2f(p.x+l, p.y);
				m_vertexArray[i++].color = m_color;

				m_vertexArray[i].position = sf::Vector2f(p.x, p.y-l);
				m_vertexArray[i++].color = m_color;

				m_vertexArray[i].position = sf::Vector2f(p.x, p.y+l);
				m_vertexArray[i++].color = m_color;
			}
		}
	}

	void BuildGrid()
	{
		switch (m_type)
		{
		case GridType::STANDARD:
			BuildStandardGrid();
			break;

		case GridType::CROSS_HAIR:
			BuildCrossHairGrid();
			break;
		}
	}

public:
	Grid(const sf::Vector2f& resolution)
		: m_resolution(resolution)
		, m_color(sf::Color(194.f, 194.f, 214.f, 96.f))
		, m_type(GridType::CROSS_HAIR)
		, m_unitSize(20.f)
		, m_crossHairSize(5.f)
	{
		m_gridHud.setSize(m_resolution);
		m_gridHud.setCenter(m_resolution.x * .5f, m_resolution.y * .5f);

		m_xLabel.setFont(FontStore::GetInstance()->GetFont("Menlo-Regular.ttf"));
		m_xLabel.setFillColor(sf::Color::Black);
		m_xLabel.setCharacterSize(12);
		m_xLabel.setPosition(7.f, m_resolution.y - 20.f);
		m_xLabel.setString("x-axis");

		m_yLabel.setFont(FontStore::GetInstance()->GetFont("Menlo-Regular.ttf"));
		m_yLabel.setFillColor(sf::Color::Black);
		m_yLabel.setCharacterSize(12);
		m_yLabel.setRotation(-90.f);
		m_yLabel.setPosition(2.5f, m_resolution.y - 25.f);
		m_yLabel.setString("y-axis");

		BuildGrid();
	}

	~Grid()
	{}

	void Update()
	{}

	void Draw(sf::RenderWindow& window)
	{
		// Draw vertex array
		window.draw(m_vertexArray);

		// Draw HUD
		window.setView(m_gridHud);
		window.draw(m_xLabel);
		window.draw(m_yLabel);
	}

	void SetUnitSize(float size)
	{
		if (size <= MAX_UNIT_SIZE && size >= MIN_UNIT_SIZE)
		{
			m_unitSize = size;
			BuildGrid();
		}
	}

	void IncrementUnitSize(float size)
	{
		if ((m_unitSize + size) <= MAX_UNIT_SIZE && (m_unitSize + size) >= MIN_UNIT_SIZE)
		{
			m_unitSize += size;
			BuildGrid();
		}
	}

	float GetUnitSize() const
	{
		return m_unitSize;
	}
};

#endif