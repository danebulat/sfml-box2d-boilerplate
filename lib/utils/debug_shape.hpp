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

	DebugShape(const sf::Vector2f& position, const std::string& tag);
	virtual ~DebugShape();

	void MarkForDelete(bool flag);
	bool IsMarkedForDelete() const;

	sf::Vector2f GetPosition() const;

	virtual void Update() = 0;
	virtual void Draw(sf::RenderWindow& window) = 0;
};

#endif