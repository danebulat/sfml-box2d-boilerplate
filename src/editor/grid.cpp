#include "editor/grid.hpp"
#include "editor/mouse_utils.hpp"
#include <string>

using std::string;
using sf::Color;
using sf::Event;
using sf::RenderWindow;
using sf::Vector2f;

Grid::Grid(const Vector2f& resolution, const Vector2f& levelSize)
	: m_resolution(resolution)
	, m_levelSize(levelSize)
	, m_color(Color(194.f, 194.f, 214.f, 96.f))
	, m_type(GridType::STANDARD)
	, m_unitSize(30.f)
	, m_crossHairSize(5.f)
	, m_visible(true)
{
	m_gridHud.setSize(m_resolution);
	m_gridHud.setCenter(m_resolution.x * .5f, m_resolution.y * .5f);

	InitLabels();
	BuildGrid();
}

Grid::~Grid()
{}

void Grid::Reset()
{
	m_color = Color(194.f, 194.f, 214.f, 96.f);
	m_type = GridType::STANDARD;
	m_unitSize = 30.f;
	m_visible = true;
	BuildGrid();
}

void Grid::InitLabels()
{
	m_xLabel.setFont(FontStore::GetInstance()->GetFont("Menlo-Regular.ttf"));
	m_xLabel.setFillColor(Color::Black);
	m_xLabel.setCharacterSize(11);
	m_xLabel.setPosition(7.f, m_resolution.y - 20.f);
	m_xLabel.setString("x-axis");

	m_yLabel.setFont(FontStore::GetInstance()->GetFont("Menlo-Regular.ttf"));
	m_yLabel.setFillColor(Color::Black);
	m_yLabel.setCharacterSize(11);
	m_yLabel.setRotation(-90.f);
	m_yLabel.setPosition(2.5f, m_resolution.y - 25.f);
	m_yLabel.setString("y-axis");

	/* Mouse label */
	m_mouseLabel.setFont(FontStore::GetInstance()->GetFont("Menlo-Regular.ttf"));
	m_mouseLabel.setFillColor(sf::Color::Black);
	m_mouseLabel.setString("(X,Y)");
	m_mouseLabel.setCharacterSize(12);
}

void Grid::HandleInput(const Event& event, RenderWindow& window)
{
	// Mouse move
	if (event.type == Event::MouseMoved)
	{
		// Defined in mouse_utils.cpp
		SetMouseLabel(m_mouseLabel, window);
	}
}

void Grid::Update()
{}

void Grid::Draw(RenderWindow& window)
{
	if (m_visible)
	{
		// Draw vertex array
		window.draw(m_vertexArray);

		// Draw HUD
		window.setView(m_gridHud);
		window.draw(m_xLabel);
		window.draw(m_yLabel);
	}
}

void Grid::DrawMouseLabel(RenderWindow& window)
{
	window.draw(m_mouseLabel);
}

// --------------------------------------------------------------------------------
// Accessors
// --------------------------------------------------------------------------------

void Grid::SetUnitSize(float size)
{
	if (size <= MAX_UNIT_SIZE && size >= MIN_UNIT_SIZE)
	{
		m_unitSize = size;
		BuildGrid();
	}
}

float Grid::GetUnitSize() const
{
	return m_unitSize;
}

void Grid::IncrementUnitSize(float size)
{
	if ((m_unitSize + size) <= MAX_UNIT_SIZE && (m_unitSize + size) >= MIN_UNIT_SIZE)
	{
		m_unitSize += size;
		BuildGrid();
	}
}

void Grid::SetType(GridType type)
{
	m_type = type;
	BuildGrid();
}

sf::Color Grid::GetLineColor() const
{
	return m_color;
}

void Grid::SetLineColor(const sf::Color& color)
{
	m_color = color;
	BuildGrid();
}

void Grid::IsVisible(bool flag)
{
	m_visible = flag;
}

bool Grid::IsVisible() const
{
	return m_visible;
}

// --------------------------------------------------------------------------------
// Build Methods
// --------------------------------------------------------------------------------

void Grid::BuildGrid()
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

void Grid::BuildStandardGrid()
{
	m_vertexArray.clear();
	m_vertexArray.setPrimitiveType(sf::Lines);

	uint pointsX = (m_levelSize.x / m_unitSize) + 1;
	uint pointsY = (m_levelSize.y / m_unitSize) + 1;

	m_vertexArray.resize((pointsX * 2) + (pointsY * 2));

	uint i = 0;
	for (uint x = 0; x < pointsX; ++x)
	{
		Vector2f position(x * m_unitSize, 0.f);
		m_vertexArray[i].position = position;
		m_vertexArray[i++].color = m_color;

		position.y = m_levelSize.y;

		m_vertexArray[i].position = position;
		m_vertexArray[i++].color = m_color;
	}

	for (uint y = 0; y < pointsY; ++y)
	{
		Vector2f position(0.f, y*m_unitSize);
		m_vertexArray[i].position = position;
		m_vertexArray[i++].color = m_color;

		position.x = m_levelSize.x;

		m_vertexArray[i].position = position;
		m_vertexArray[i++].color = m_color;
	}
}

void Grid::BuildCrossHairGrid()
{
	m_vertexArray.clear();
	m_vertexArray.setPrimitiveType(sf::Lines);

	uint pointsX = (m_levelSize.x / m_unitSize) + 1;
	uint pointsY = (m_levelSize.y / m_unitSize) + 1;

	m_vertexArray.resize((pointsX * pointsY) * 4);

	float l = m_crossHairSize;
	uint i = 0;
	for (uint x = 0; x < pointsX; ++x)
	{
		for (uint y = 0; y < pointsY; ++y)
		{
			Vector2f p(x*m_unitSize, y*m_unitSize);
			m_vertexArray[i].position = Vector2f(p.x-l, p.y);
			m_vertexArray[i++].color = m_color;

			m_vertexArray[i].position = Vector2f(p.x+l, p.y);
			m_vertexArray[i++].color = m_color;

			m_vertexArray[i].position = Vector2f(p.x, p.y-l);
			m_vertexArray[i++].color = m_color;

			m_vertexArray[i].position = Vector2f(p.x, p.y+l);
			m_vertexArray[i++].color = m_color;
		}
	}
}