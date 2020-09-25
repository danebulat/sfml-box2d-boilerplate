#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class BoundingBox
{
private:
	sf::RectangleShape	m_sprite;
	sf::FloatRect		m_rectangle;
	//bool 				m_enabled;

private:
	void Calculate(std::vector<sf::Vector2f>& vertices);

public:
	BoundingBox(std::vector<sf::Vector2f>& vertices);

	// Not copyable
	BoundingBox(const BoundingBox&) = delete;
	BoundingBox& operator= (const BoundingBox&) = delete;

	void Update(std::vector<sf::Vector2f>& vertices);
	void Draw(sf::RenderWindow& window);

	sf::FloatRect GetBoundingBox();
};

#endif