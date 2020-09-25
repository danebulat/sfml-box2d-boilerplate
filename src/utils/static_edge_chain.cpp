#include "utils/static_edge_chain.hpp"
#include "utils/mouse_utils.hpp"
#include "utils/constants.hpp"

using sf::Color;
using sf::Vector2f;
using sf::RenderWindow;
using sf::LineStrip;
using sf::FloatRect;
using sf::Event;
using sf::Mouse;
using std::string;
using std::size_t;

// --------------------------------------------------------------------------------
// Initialisation
// --------------------------------------------------------------------------------

StaticEdgeChain::StaticEdgeChain()
	: m_vertexCount(0)
	, m_color(Color::Blue)
	, m_drawBoundingBox(false)
	, m_updateBoundingBox(false)
	, m_body(nullptr)
	, m_mouseMoved(false)
	, m_leftMouseDown(false)
	, m_selectedHandle(nullptr)
	, m_editable(false)
	, m_hoveringOnHandle(false)
{}

StaticEdgeChain::StaticEdgeChain(std::vector<Vector2f>& vertices, const string& tag, b2World* world)
	: m_tag(tag)
	, m_vertexCount(0)
	, m_color(Color::Blue)
	, m_drawBoundingBox(false)
	, m_updateBoundingBox(false)
	, m_body(nullptr)
	, m_mouseMoved(false)
	, m_leftMouseDown(false)
	, m_selectedHandle(nullptr)
	, m_editable(false)
	, m_hoveringOnHandle(false)
	, m_hoveringOnMoveHandle(false)
{
	Init(vertices, world);
}

void StaticEdgeChain::Init(std::vector<Vector2f>& vertices, b2World* world)
{
	m_vertexCount = vertices.size();
	m_vertices = vertices;

	// Initialise SFML vertex array
	m_vertexArray.setPrimitiveType(LineStrip);
	m_vertexArray.resize(m_vertexCount);
	for (std::size_t i = 0; i < m_vertexCount; ++i)
	{
		m_vertexArray[i].position = m_vertices[i];
		m_vertexArray[i].color = m_color;
	}

	// Initialise scaled vertex array
	m_scaledVertices.resize(m_vertexCount);
	for (std::size_t i = 0; i < m_vertexCount; ++i)
	{
		m_scaledVertices[i].x = m_vertices[i].x / SCALE;
		m_scaledVertices[i].y = m_vertices[i].y / SCALE;
	}

	// Initialise chain shape
	b2ChainShape chain;
	b2Vec2 prevVertex((m_vertices[0].x - 100.f)/SCALE, (m_vertices[0].y)/SCALE);
	b2Vec2 nextVertex((m_vertices[m_vertexCount].x + 100.f) / SCALE,
					  (m_vertices[m_vertexCount].y) / SCALE);

	chain.CreateChain(m_scaledVertices.data(), m_vertexCount,
		prevVertex, nextVertex);

	// Create fixture and body
	b2FixtureDef fixture;
	fixture.density = 0.f;
	fixture.shape = &chain;

	b2BodyDef bodyDef;
	bodyDef.position.Set(0, 0);
	bodyDef.type = b2_staticBody;

	b2Body* body = world->CreateBody(&bodyDef);
	body->CreateFixture(&fixture);

	m_body = body;

	// Instantiate world bounding box
	m_boundingBox.reset(new BoundingBox(m_vertices));

	// Instantiate move handle
	m_moveHandle.reset(new MoveHandle(m_boundingBox, m_tag));

	// Initialise vertex handles
	InitVertexHandles();
}

StaticEdgeChain::~StaticEdgeChain()
{}

void StaticEdgeChain::DeleteBody(b2World* world)
{
	if (m_body != nullptr)
	{
		world->DestroyBody(m_body);
		m_body = nullptr;
	}
}

void StaticEdgeChain::InitVertexHandles()
{
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		VertexHandle vHandle;
		vHandle.m_color = Color(0.f, 0.f, 255.f, 64.f);
		vHandle.m_hoverColor = Color(0.f, 0.f, 255.f, 128.f);
		vHandle.m_position.x = m_vertices[i].x;
		vHandle.m_position.y = m_vertices[i].y;

		vHandle.m_sprite.setRadius(vHandle.m_size);
		vHandle.m_sprite.setFillColor(vHandle.m_color);
		vHandle.m_sprite.setOutlineColor(Color(0.f, 0.f, 255.f, 124.f));
		vHandle.m_sprite.setOutlineThickness(1);

		vHandle.m_sprite.setOrigin(vHandle.m_size, vHandle.m_size);
		vHandle.m_sprite.setPosition(m_vertices[i].x, m_vertices[i].y);
		vHandle.m_vertexIndex = i;

		m_vertexHandles.push_back(vHandle);
	}
}

// --------------------------------------------------------------------------------
// Accessors
// --------------------------------------------------------------------------------

void StaticEdgeChain::SetColor(const Color color)
{
	m_color = color;
}

void StaticEdgeChain::SetEnabled(bool enabled)
{
	if (!enabled)
		m_color.a = 64.f;
	else
		m_color.a = 255.f;

	for (std::size_t i = 0; i < m_vertexCount; ++i)
		m_vertexArray[i].color = m_color;

	m_body->SetEnabled(enabled);
}

bool StaticEdgeChain::IsEnabled() const
{
	return m_body->IsEnabled();
}

void StaticEdgeChain::SetEditable(bool editable)
{
	m_editable = editable;
}

bool StaticEdgeChain::IsEditable() const
{
	return m_editable;
}

const std::string& StaticEdgeChain::GetTag() const
{
	return m_tag;
}

void StaticEdgeChain::SetTag(const std::string& tag)
{
	m_tag = tag;
}

void StaticEdgeChain::DrawBoundingBox(bool flag)
{
	m_drawBoundingBox = flag;
}

// --------------------------------------------------------------------------------
// Input
// --------------------------------------------------------------------------------

void StaticEdgeChain::HandleInput(const Event& event, RenderWindow& window)
{
	// MouseButtonPressed
	if (event.type == Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == Mouse::Left)
		{
			m_leftMouseDown = true;
		}
	}

	// MouseButtonReleased
	if (event.type == Event::MouseButtonReleased)
	{
		if (event.mouseButton.button == Mouse::Left)
		{
			m_leftMouseDown = false;
		}
	}

	// MouseMoved
	if (event.type == Event::MouseMoved)
	{
		m_mouseMoved = true;
	}
	else
	{
		m_mouseMoved = false;
	}
}

// --------------------------------------------------------------------------------
// Update
// --------------------------------------------------------------------------------

void StaticEdgeChain::Update(RenderWindow& window)
{
	// Get mouse position
	Vector2f mousePos = GetMousePosition(window);

	if (m_editable)
	{
		// ----------------------------------------------------------------------
		// Move handle
		// ----------------------------------------------------------------------

		// Handle previous frame (move handle)
		if ((m_hoveringOnMoveHandle && m_leftMouseDown) && !(m_hoveringOnHandle && m_leftMouseDown))
		{
			// Get move increment (pixel coordinates)
			Vector2f moveIncrement = GetIncrement(m_prevMousePosition, mousePos);
			m_moveHandle->Update(moveIncrement);

			// Disable the body before updating vertices
			m_body->SetEnabled(false);

			b2Fixture* fixture = m_body->GetFixtureList();
			if (fixture->GetType() == b2Shape::e_chain)
			{
				b2ChainShape* shape = dynamic_cast<b2ChainShape*>(fixture->GetShape());

				for (size_t i = 0; i < m_vertexCount; ++i)
				{
					shape->m_vertices[i].x += moveIncrement.x / SCALE;
					shape->m_vertices[i].y += moveIncrement.y / SCALE;
				}
			}

			// Enable body after updating its data
			m_body->SetEnabled(true);

			// Update cached vertices and SFML VertexArray
			for (size_t i = 0; i < m_vertexCount; ++i)
			{
				m_vertices[i] += moveIncrement;
				m_vertexArray[i].position += moveIncrement;

				m_scaledVertices[i].x += moveIncrement.x / SCALE;
				m_scaledVertices[i].y += moveIncrement.y / SCALE;
			}

			// Update vertex handle positions
			for (auto& handle : m_vertexHandles)
			{
				handle.m_position += moveIncrement;
				handle.m_sprite.setPosition(handle.m_position);
			}

			// Flag to update bounding box
			m_updateBoundingBox = true;
		}

		// Check if mouse is hovering over the move handle
		if (m_mouseMoved)
		{
			Vector2f moveHandlePos = m_moveHandle->GetPosition();
			float radius = m_moveHandle->GetSize();

			if (((mousePos.x > moveHandlePos.x - radius) && (mousePos.x < moveHandlePos.x + radius)) &&
				((mousePos.y > moveHandlePos.y - radius) && (mousePos.y < moveHandlePos.y + radius)))
			{
				m_moveHandle->SetHoveredState(true);
				m_hoveringOnMoveHandle = true;
			}
			else
			{
				m_moveHandle->SetHoveredState(false);
				m_hoveringOnMoveHandle = false;
			}
		}

		// ----------------------------------------------------------------------
		// Vertex handles
		// ----------------------------------------------------------------------

		// Handle previous frame before updating hovering flag
		if (m_hoveringOnHandle && m_leftMouseDown)
		{
			m_selectedHandle->m_position += GetIncrement(m_prevMousePosition, mousePos);
			m_selectedHandle->m_sprite.setPosition(m_selectedHandle->m_position);

			// Disable body before updating vertices
			m_body->SetEnabled(false);

			// TODO: Destroy body and re-create new chain
			b2Fixture* fixture = m_body->GetFixtureList();
			if (fixture->GetType() == b2Shape::e_chain)
			{
				b2ChainShape* shape = dynamic_cast<b2ChainShape*>(fixture->GetShape());

				shape->m_vertices[m_selectedHandle->m_vertexIndex].Set(
					m_selectedHandle->m_position.x / SCALE,
					m_selectedHandle->m_position.y / SCALE);
			}

			// Enable body after updating its data
			m_body->SetEnabled(true);

			// Update cached vertices and SFML VertexArray
			m_vertices[m_selectedHandle->m_vertexIndex] = m_selectedHandle->m_position;
			m_vertexArray[m_selectedHandle->m_vertexIndex].position = m_selectedHandle->m_position;

			// Flag to update bounding box
			m_updateBoundingBox = true;
		}

		// Handler gets selected automatically if the mouse is hovering over it
		if (m_mouseMoved)
		{
			for (int i = 0; i < m_vertexHandles.size(); ++i)
			{
				Vector2f handlePos = m_vertexHandles[i].m_position;
				float radius = m_vertexHandles[i].m_size;

				// Check if mouse is hovering over a vertex handle
				if (((mousePos.x > handlePos.x - radius) && (mousePos.x < handlePos.x + radius)) &&
					((mousePos.y > handlePos.y - radius) && (mousePos.y < handlePos.y + radius)))
				{
					m_vertexHandles[i].m_sprite.setFillColor(m_vertexHandles[i].m_hoverColor);

					// If another vertex is already selected, unselect it
					if (m_selectedHandle != nullptr && m_selectedHandle != &m_vertexHandles[i])
						m_selectedHandle->m_sprite.setFillColor(m_selectedHandle->m_color);

					// Cache the hovered handle
					if (m_selectedHandle != &m_vertexHandles[i])
						m_selectedHandle = &m_vertexHandles[i];

					// Set up an update for the next frame
					m_hoveringOnHandle = true;
					break;
				}
				else
				{
					m_hoveringOnHandle = false;
					m_vertexHandles[i].m_sprite.setFillColor(m_vertexHandles[i].m_color);
				}
			}// end for
		}

		// Re-caclulate bounding box if a vertex has been moved
		if (m_updateBoundingBox)
		{
			m_boundingBox->Update(m_vertices);
			m_moveHandle->Update(m_boundingBox);
			m_updateBoundingBox = false;
		}

		// Clear selected handle cache if mouse is not hovering over a handle
		if (!m_hoveringOnHandle)
		{
			m_selectedHandle = nullptr;
		}

		m_prevMousePosition = mousePos;
	}
}

// --------------------------------------------------------------------------------
// Draw
// --------------------------------------------------------------------------------

void StaticEdgeChain::Draw(RenderWindow& window)
{
	window.draw(m_vertexArray);

	// Draw bounding box
	if (m_drawBoundingBox)
	{
		m_boundingBox->Draw(window);
	}

	// Draw move handle vertex handles
	if (m_editable)
	{
		for (auto& handle : m_vertexHandles)
			window.draw(handle.m_sprite);
	}

	m_moveHandle->Draw(window, m_editable);
}