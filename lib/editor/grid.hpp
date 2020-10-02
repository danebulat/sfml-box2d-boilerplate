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
	bool 				m_visible;

	sf::View			m_gridHud;
	sf::Text 			m_xLabel;
	sf::Text 			m_yLabel;

	static constexpr float MIN_UNIT_SIZE = 10.f;
	static constexpr float MAX_UNIT_SIZE = 250.f;

private:
	void BuildGrid();
	void BuildStandardGrid();
	void BuildCrossHairGrid();
	void InitLabels();

public:
	Grid(const sf::Vector2f& resolution);
	~Grid();

	void Reset();

	void Update();
	void Draw(sf::RenderWindow& window);

	void  SetUnitSize(float size);
	float GetUnitSize() const;
	void  IncrementUnitSize(float size);
	void  SetType(GridType type);
	void  IsVisible(bool flag);
	bool  IsVisible() const;

	sf::Color GetLineColor() const;
	void SetLineColor(const sf::Color& color);
};

#endif