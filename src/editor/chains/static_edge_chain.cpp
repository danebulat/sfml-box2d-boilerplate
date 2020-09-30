#include "editor/chains/static_edge_chain.hpp"
#include "editor/managers/edge_chain_manager.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/constants.hpp"

using sf::Color;
using sf::Vector2f;
using sf::RenderWindow;
using sf::LineStrip;
using sf::FloatRect;
using sf::Event;
using sf::Mouse;
using std::string;
using std::vector;
using std::size_t;

// --------------------------------------------------------------------------------
// Static Methods
// --------------------------------------------------------------------------------

void StaticEdgeChain::GetChainGhostVertices(b2Vec2& p, b2Vec2& n, vector<Vector2f>& vertices)
{
	int count = vertices.size() - 1;
	float offset = 100.f;

	p.x = (vertices[0].x - offset) / SCALE;
	p.y = (vertices[0].y) / SCALE;

	n.x = (vertices[count].x + offset) / SCALE;
	n.y = (vertices[count].y) / SCALE;
}

sf::Vector2f StaticEdgeChain::GetNextAddedVertexPosition(vector<Vector2f>& vertices)
{
	// Calculate new vertex position based on last 2 vertices
	Vector2f position;
	float offset = 50.f;

	if (vertices.size() > 2)
	{
		vector<Vector2f>::iterator last1 = vertices.end() - 1;
		vector<Vector2f>::iterator last2 = last1 - 1;

		position.x = last1->x;
		position.y = last1->y;

		if (last1->x > last2->x)
			position.x += offset;
		else
			position.x -= offset;
	}
	else if (vertices.size() == 1)
	{
		position = vertices[0];
		position.x += offset;
	}
	else
	{
		position.x = 100.f;
		position.y = 100.f;
	}

	return position;
}

// --------------------------------------------------------------------------------
// Initialisation
// --------------------------------------------------------------------------------

StaticEdgeChain::StaticEdgeChain(ChainManagerController* manager)
	: m_vertexCount(0)
	, m_color(Color::Blue)
	, m_drawBoundingBox(false)
	, m_updateBoundingBox(false)
	, m_body(nullptr)
	, m_manager(manager)
	, m_mouseMoved(false)
	, m_leftMouseDown(false)
	, m_selectedHandle(nullptr)
	, m_editable(false)
	, m_hoveringOnHandle(false)
	, m_addVertex(false)
	, m_removeVertex(false)
{}

StaticEdgeChain::StaticEdgeChain(std::vector<Vector2f>& vertices,
	const string& tag, b2World* world, ChainManagerController* manager)
	: m_tag(tag)
	, m_vertexCount(0)
	, m_color(Color::Blue)
	, m_drawBoundingBox(false)
	, m_updateBoundingBox(false)
	, m_body(nullptr)
	, m_manager(manager)
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

	// Build b2Body
	Vector2f p(0.f, 0.f);
	BuildBody(world, p);

	// Instantiate world bounding box
	m_boundingBox.reset(new BoundingBox(m_vertices));

	// Instantiate move handle
	m_moveHandle.reset(new MoveHandle(m_boundingBox, m_tag));

	// Initialise vertex handles
	for (unsigned int i = 0; i < m_vertices.size(); ++i)
		m_vertexHandles.push_back(VertexHandle(m_vertices[i], i));
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

void StaticEdgeChain::BuildBody(b2World* world, const Vector2f& position)
{
	if (m_body != nullptr)
	{
		world->DestroyBody(m_body);
		m_body = nullptr;
	}

	b2Vec2 worldPos(position.x/SCALE, position.y/SCALE);

	// Scale vertices
	vector<b2Vec2> scaled(m_vertexCount);

	for (int i = 0; i < scaled.size(); ++i)
	{
		scaled[i].x = m_vertices[i].x / SCALE;
		scaled[i].y = m_vertices[i].y / SCALE;
	}

	// Create fixture and body
	b2ChainShape chain;
	b2Vec2 p, n;
	GetChainGhostVertices(p, n, m_vertices);
	chain.CreateChain(scaled.data(), scaled.size(), p, n);

	b2FixtureDef fixture;
	fixture.density = 0.f;
	fixture.shape = &chain;

	b2BodyDef bodyDef;
	bodyDef.position.Set(worldPos.x, worldPos.y);
	bodyDef.type = b2_staticBody;

	m_body = world->CreateBody(&bodyDef);
	m_body->CreateFixture(&fixture);
}

void StaticEdgeChain::AddVertex(b2World* world)
{
	Vector2f position = GetNextAddedVertexPosition(m_vertices);

	// Update m_vertices vector with new vertex
	m_vertices.push_back(position);
	++m_vertexCount;
	dynamic_cast<EdgeChainManager*>(m_manager)->
		IncrementEdgeChainVertexCount(1);

	// Delete body and rebuild it
	b2Vec2 pos = m_body->GetWorldCenter();
	BuildBody(world, Vector2f(pos.x*SCALE, pos.y*SCALE));

	// Update SFML vertex array
	m_vertexArray.resize(m_vertexCount);
	m_vertexArray[m_vertexCount-1].position = position;
	m_vertexArray[m_vertexCount-1].color = m_color;

	// Update bounding box & move handle
	m_boundingBox->Update(m_vertices);
	m_moveHandle->Update(m_boundingBox);

	// Add a new VertexHandle
	m_vertexHandles.push_back(VertexHandle(position, m_vertexCount-1));
}

void StaticEdgeChain::RemoveVertex(b2World* world)
{
	if (m_vertices.size() > 3)
	{
		m_vertices.pop_back();
		--m_vertexCount;
		dynamic_cast<EdgeChainManager*>(m_manager)
			->IncrementEdgeChainVertexCount(-1);

		// Delete body and rebuild it
		b2Vec2 pos = m_body->GetWorldCenter();
		BuildBody(world, Vector2f(pos.x*SCALE, pos.y*SCALE));

		// Resize SFML vertex array
		m_vertexArray.resize(m_vertexCount);

		// Update bounding box & move handle
		m_boundingBox->Update(m_vertices);
		m_moveHandle->Update(m_boundingBox);

		// Remove last vertex handle
		m_vertexHandles.pop_back();
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

FloatRect StaticEdgeChain::GetMoveHandleLabelRect() const
{
	return m_moveHandle->GetLabelRectangle();
}

void StaticEdgeChain::SetAddVertexFlag(bool flag)
{
	m_addVertex = flag;
}

void StaticEdgeChain::SetRemoveVertexFlag(bool flag)
{
	m_removeVertex = flag;
}

long StaticEdgeChain::GetVertexCount()
{
	return m_vertexCount;
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

void StaticEdgeChain::Update(RenderWindow& window, b2World* world)
{
	// Get mouse position
	Vector2f mousePos = GetMousePosition(window);

	if (m_editable)
	{
		// Handle adding or removing vertices
		if (m_addVertex)
		{
			AddVertex(world);
			m_addVertex = false;
		}

		if (m_removeVertex)
		{
			RemoveVertex(world);
			m_removeVertex = false;
		}

		// ----------------------------------------------------------------------
		// Move handle
		// ----------------------------------------------------------------------

		// Handle previous frame (move handle)
		if ((m_hoveringOnMoveHandle && m_leftMouseDown) && !(m_hoveringOnHandle && m_leftMouseDown))
		{
			// Get move increment (pixel coordinates)
			Vector2f moveIncrement = GetIncrement(m_prevMousePosition, mousePos);
			m_moveHandle->Update(moveIncrement);

			m_body->SetEnabled(false);

			// NOTE: Do not try to update the body's world position, as the vertices
			// are not updated. Also causes crashing when trying to rebuild the vertices.
			// Therefore, do not modify an edge chain's world position after creation.
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
			}

			// Update vertex handle positions (if whole chain was moved)
			for (auto& handle : m_vertexHandles)
				handle.Update(moveIncrement);

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
			// The index of the vertex being clicked on
			unsigned int index = m_selectedHandle->GetVertexIndex();

			m_selectedHandle->Update(GetIncrement(m_prevMousePosition, mousePos));

			// Disable body before updating vertices
			m_body->SetEnabled(false);

			// TODO: Destroy body and re-create new chain
			b2Fixture* fixture = m_body->GetFixtureList();
			if (fixture->GetType() == b2Shape::e_chain)
			{
				b2ChainShape* shape = dynamic_cast<b2ChainShape*>(fixture->GetShape());

				shape->m_vertices[index].Set(
					m_selectedHandle->GetPosition().x / SCALE,
					m_selectedHandle->GetPosition().y / SCALE);
			}

			// Enable body after updating its data
			m_body->SetEnabled(true);

			// Update cached vertices and SFML VertexArray
			m_vertices[index] = m_selectedHandle->GetPosition();
			m_vertexArray[index].position = m_selectedHandle->GetPosition();

			// Flag to update bounding box
			m_updateBoundingBox = true;
		}

		// Handler gets selected automatically if the mouse is hovering over it
		if (m_mouseMoved)
		{
			// Loop to cache a vertex handle if mouse is hovering over it
			for (int i = 0; i < m_vertexHandles.size(); ++i)
			{
				Vector2f handlePos = m_vertexHandles[i].GetPosition();
				float radius = m_vertexHandles[i].GetSize();

				// Check if mouse is hovering over a vertex handle
				if (((mousePos.x > handlePos.x - radius) && (mousePos.x < handlePos.x + radius)) &&
					((mousePos.y > handlePos.y - radius) && (mousePos.y < handlePos.y + radius)))
				{
					// If another vertex is already selected, unselect it
					if (m_selectedHandle != nullptr && m_selectedHandle != &m_vertexHandles[i])
						m_selectedHandle->SetHoveredState(false);

					// Cache the hovered handle
					if (m_selectedHandle != &m_vertexHandles[i])
						m_selectedHandle = &m_vertexHandles[i];

					// Set up an update for the next frame
					m_hoveringOnHandle = true;
					m_vertexHandles[i].SetHoveredState(m_hoveringOnHandle);
					break;
				}
				else
				{
					m_hoveringOnHandle = false;
					m_vertexHandles[i].SetHoveredState(m_hoveringOnHandle);
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

	// Set move handle label color
	m_moveHandle->SelectLabel(m_editable);
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

	// Draw vertex handles
	if (m_editable)
	{
		for (auto& handle : m_vertexHandles)
			handle.Draw(window);
	}

	// Draw move handle
	m_moveHandle->Draw(window, m_editable);
}