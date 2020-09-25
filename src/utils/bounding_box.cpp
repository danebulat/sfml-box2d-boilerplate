#include "utils/bounding_box.hpp"

using sf::Color;
using sf::FloatRect;
using sf::Vector2f;
using sf::RenderWindow;
using std::vector;

BoundingBox::BoundingBox(vector<Vector2f>& vertices)
{
	m_sprite.setFillColor(Color(196.f, 196.f, 196.f, 20.f));
	m_sprite.setOutlineColor(Color(196.f, 196.f, 196.f, 196.f));
	m_sprite.setOutlineThickness(1.f);

	Calculate(vertices);
}

BoundingBox::~BoundingBox()
{}

void BoundingBox::Calculate(vector<Vector2f>& vertices)
{
	// Initialise variables to store min/max coordinates
	float lowerX = 0.f, lowerY = 0.f;
	float upperX = 0.f, upperY = 0.f;

	if (vertices.size() > 0)
	{
		lowerX = vertices[0].x;
		lowerY = vertices[0].y;
		upperX = vertices[0].x;
		upperY = vertices[0].y;
	}

	// Retrieve min/max coordinates from vertex array
	for (auto& vertex : vertices)
	{
		float x = vertex.x;
		float y = vertex.y;

		if      (x < lowerX) lowerX = x;
		else if (x > upperX) upperX = x;

		if 	 	(y < lowerY) lowerY = y;
		else if (y > upperY) upperY = y;
	}

	// Construct FloatRect
	m_rectangle.left = lowerX;
	m_rectangle.top = lowerY;
	m_rectangle.width = upperX - lowerX;
	m_rectangle.height = upperY - lowerY;

	// Initialise rectangle shape size and position
	m_sprite.setPosition(m_rectangle.left, m_rectangle.top);
	m_sprite.setSize(Vector2f(m_rectangle.width, m_rectangle.height));
}

void BoundingBox::Update(vector<Vector2f>& vertices)
{
	Calculate(vertices);
}

void BoundingBox::Draw(RenderWindow& window)
{
	window.draw(m_sprite);
}

FloatRect BoundingBox::GetBoundingBox()
{
	return m_rectangle;
}