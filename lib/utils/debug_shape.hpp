#ifndef DEBUG_SHAPE_HPP
#define DEBUG_SHAPE_HPP

#include <SFML/Graphics.hpp>

class DebugShape
{
protected:
	sf::Vector2f	m_position;
	std::string* 	m_tag;
	bool 			m_markedForDelete;

public:
	static unsigned int ShapeBodyCount;
	static unsigned int DebugBoxCount;
	static unsigned int DebugCircleCount;

	DebugShape(const sf::Vector2f& position,
			   const std::string& tag)
	{
		++ShapeBodyCount;
		m_markedForDelete = false;
		m_position = position;
		m_tag = new std::string(tag.c_str());
	}

	virtual ~DebugShape()
	{}

	void MarkForDelete(bool flag)
	{
		m_markedForDelete = flag;
	}

	bool IsMarkedForDelete() const
	{
		return m_markedForDelete;
	}

	sf::Vector2f GetPosition() const
	{
		return m_position;
	}

	virtual void Update() = 0;
	virtual void Draw(sf::RenderWindow& window) = 0;
};

#endif