#include "utils/debug_shape.hpp"

using sf::Vector2f;
using std::string;

unsigned int DebugShape::ShapeBodyCount = 0;
unsigned int DebugShape::DebugBoxCount = 0;
unsigned int DebugShape::DebugCircleCount = 0;

DebugShape::DebugShape(const Vector2f& position,const string& tag)
{
	++ShapeBodyCount;
	m_markedForDelete = false;
	m_position = position;
	m_tag = new string(tag.c_str());
}

DebugShape::~DebugShape()
{}

void DebugShape::MarkForDelete(bool flag)
{
	m_markedForDelete = flag;
}

bool DebugShape::IsMarkedForDelete() const
{
	return m_markedForDelete;
}

Vector2f DebugShape::GetPosition() const
{
	return m_position;
}