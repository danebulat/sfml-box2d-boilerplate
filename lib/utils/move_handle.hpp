#ifndef MOVE_HANDLE_HPP
#define MOVE_HANDLE_HPP

#include <SFML/Graphics.hpp>
#include "utils/bounding_box.hpp"

class MoveHandle
{
private:
	sf::CircleShape		m_sprite;
	sf::Vector2f 		m_position;

	sf::Font 			m_font;
	sf::Text			m_label;

	sf::Color 			m_color;
	sf::Color 			m_hoverColor;

	float 				m_size;
	bool				m_hoverState;

private:
	void Calculate(std::shared_ptr<BoundingBox> boundingBox);

public:
	MoveHandle(std::shared_ptr<BoundingBox> boundingBox, const sf::String& label);
	~MoveHandle();

	MoveHandle(const MoveHandle&) = delete;
	MoveHandle& operator= (const MoveHandle&) = delete;

	/* Update when handle is being dragged */
	void Update(sf::Vector2f& moveIncrement);

	/* Update when bounding box changes */
	void Update(std::shared_ptr<BoundingBox> boundingBox);

	void Draw(sf::RenderWindow& window, bool inEditMode);

	void SetHoveredState(bool flag);
	float GetSize() const;
	sf::Vector2f GetPosition();
};

#endif