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
	void Calculate(std::shared_ptr<BoundingBox> boundingBox)
	{
		sf::FloatRect bbRect = boundingBox->GetBoundingBox();

		m_position.x = bbRect.left + (bbRect.width * 0.5f);
		m_position.y = bbRect.top;
		m_sprite.setPosition(m_position);

		m_label.setPosition(
			m_position.x - (m_label.getLocalBounds().width * 0.5f),
			m_position.y - (m_size * 2.0f));
	}

public:
	MoveHandle(std::shared_ptr<BoundingBox> boundingBox, const sf::String& label)
	{
		m_hoverState = false;
		m_size = 15.f;

		m_color = sf::Color(102.f, 0.f, 102.f, 56.f);
		m_hoverColor = sf::Color(102.f, 0.f, 102.f, 128.f);

		m_sprite.setRadius(m_size);
		m_sprite.setFillColor(m_color);
		m_sprite.setOutlineColor(m_hoverColor);
		m_sprite.setOutlineThickness(1);
		m_sprite.setOrigin(m_size, m_size);

		// TODO: Get font from singleton
		if (!m_font.loadFromFile("content/Menlo-Regular.ttf"))
			std::cerr << "MoveHandle::MoveHandle() : Unable to load font" << std::endl;

		m_label.setFont(m_font);
		m_label.setFillColor(sf::Color::Black);
		m_label.setCharacterSize(12);
		m_label.setStyle(sf::Text::Regular);
		m_label.setString(label);

		// Calculate and set positions
		Calculate(boundingBox);
	}

	MoveHandle(const MoveHandle&) = delete;
	MoveHandle& operator= (const MoveHandle&) = delete;

	// Update when handle is being dragged
	void Update(sf::Vector2f& moveIncrement)
	{
		m_position += moveIncrement;
		m_sprite.setPosition(m_position);

		sf::Vector2f newLabelPos = m_label.getPosition() + moveIncrement;
		m_label.setPosition(newLabelPos);
	}

	// Update when bounding box changed
	void Update(std::shared_ptr<BoundingBox> boundingBox)
	{
		Calculate(boundingBox);
	}

	void Draw(sf::RenderWindow& window, bool inEditMode)
	{
		// Draw sprite if this edge chain is selected
		if (inEditMode)
		{
			if (m_hoverState)
				m_sprite.setFillColor(m_hoverColor);
			else
				m_sprite.setFillColor(m_color);

			window.draw(m_sprite);
			window.draw(m_label);
		}

		// Otherwise, just draw the label
		window.draw(m_label);
	}

	void SetHoveredState(bool flag)
	{
		m_hoverState = flag;
	}

	float GetSize() const
	{
		return m_size;
	}

	sf::Vector2f GetPosition()
	{
		return m_position;
	}
};

#endif