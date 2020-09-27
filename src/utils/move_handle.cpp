#include "utils/move_handle.hpp"
#include "utils/font_store.hpp"

using sf::Color;
using sf::FloatRect;
using sf::String;
using sf::Text;
using sf::RenderWindow;
using sf::Vector2f;
using std::shared_ptr;
using std::cerr;
using std::endl;

MoveHandle::MoveHandle(shared_ptr<BoundingBox> boundingBox, const String& label)
{
	m_hoverState = false;
	m_size = 15.f;

	m_color = Color(102.f, 0.f, 102.f, 56.f);
	m_hoverColor = Color(102.f, 0.f, 102.f, 128.f);

	m_labelSelected = false;
	m_labelColor = Color(0.f, 0.f, 0.f, 255.f);
	m_labelSelectedColor = Color(0.f, 0.f, 153.f, 255.f);

	m_sprite.setRadius(m_size);
	m_sprite.setFillColor(m_color);
	m_sprite.setOutlineColor(m_hoverColor);
	m_sprite.setOutlineThickness(1);
	m_sprite.setOrigin(m_size, m_size);

	sf::Font& font = FontStore::GetInstance()->GetFont("Menlo-Regular.ttf");
	m_label.setFont(font);
	m_label.setFillColor(m_labelColor);
	m_label.setCharacterSize(12);
	m_label.setStyle(Text::Regular);
	m_label.setString(label);

	// Calculate and set positions
	Calculate(boundingBox);
}

MoveHandle::~MoveHandle()
{}

void MoveHandle::Calculate(shared_ptr<BoundingBox> boundingBox)
{
	FloatRect bbRect = boundingBox->GetBoundingBox();

	m_position.x = bbRect.left + (bbRect.width * 0.5f);
	m_position.y = bbRect.top;
	m_sprite.setPosition(m_position);

	m_label.setPosition(
		m_position.x - (m_label.getLocalBounds().width * 0.5f),
		m_position.y - (m_size * 2.2f));
}

// Update when handle is being dragged
void MoveHandle::Update(Vector2f& moveIncrement)
{
	m_position += moveIncrement;
	m_sprite.setPosition(m_position);

	Vector2f newLabelPos = m_label.getPosition() + moveIncrement;
	m_label.setPosition(newLabelPos);
}

// Update when bounding box changed
void MoveHandle::Update(shared_ptr<BoundingBox> boundingBox)
{
	Calculate(boundingBox);
}

void MoveHandle::Draw(RenderWindow& window, bool inEditMode)
{
	// Draw sprite if this edge chain is selected
	if (inEditMode)
	{
		if (!m_hoverState)
			m_sprite.setFillColor(m_color);
		else
			m_sprite.setFillColor(m_hoverColor);

		window.draw(m_sprite);
		window.draw(m_label);
	}

	// Draw the label
	if (!m_labelSelected)
	{
		m_label.setFillColor(m_labelColor);
		m_label.setStyle(Text::Regular);
	}
	else
	{
		m_label.setFillColor(m_labelSelectedColor);
		m_label.setStyle(Text::Underlined);
	}

	window.draw(m_label);
}

void MoveHandle::SetHoveredState(bool flag)
{
	m_hoverState = flag;
}

float MoveHandle::GetSize() const
{
	return m_size;
}

Vector2f MoveHandle::GetPosition()
{
	return m_position;
}

FloatRect MoveHandle::GetLabelRectangle() const
{
	return m_label.getGlobalBounds();
}

void MoveHandle::SelectLabel(bool flag)
{
	m_labelSelected = flag;
}