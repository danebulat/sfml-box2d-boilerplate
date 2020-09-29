#include "utils/custom_polygon.hpp"

CustomPolygon::CustomPolygon(const sf::Vector2f& position, const std::vector<sf::Vector2f>& vertices, b2World* world)
	: DebugShape(position, "custom_polygon")
{
	m_world = world;
	m_color = sf::Color::Magenta;
	m_wireframe = false;
	m_body = nullptr;

	m_vertices = vertices;
	m_vertexCount = m_vertices.size();

	// Initialise SFML vertex array
	m_wireframe ? m_vertexArray.setPrimitiveType(sf::LineStrip) :
					m_vertexArray.setPrimitiveType(sf::TriangleFan);

	m_vertexArray.resize(m_vertexCount + 1);
	for (std::size_t i = 0; i < m_vertexCount + 1; ++i)
	{
		m_vertexArray[i].position = m_vertices[i];
		m_vertexArray[i].color = m_color;
	}

	CreateBody();
}

CustomPolygon::~CustomPolygon()
{
	if (m_body != nullptr)
	{
		m_world->DestroyBody(m_body);
		m_body = nullptr;
	}

	--CustomPolygonCount;

	std::cout << "--BodyCount (CustomPolygon)\n";
}

void CustomPolygon::CreateBody()
{
	// Initialise b2Body vertices
	b2Vec2 scaledVertices[m_vertexCount];

	for (std::size_t i = 0; i < m_vertexCount; ++i)
		scaledVertices[i].Set(m_vertices[i].x / SCALE, m_vertices[i].y / SCALE);

	// Create body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(m_position.x/SCALE, m_position.y/SCALE);
	bodyDef.angularDamping = 0.1f;
	bodyDef.linearDamping = 0.f;
	bodyDef.allowSleep = true;
	bodyDef.awake = true;
	bodyDef.fixedRotation = false;
	bodyDef.bullet = false;
	bodyDef.enabled = true;

	if (!m_tag->empty())
		bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(m_tag);

	m_body = m_world->CreateBody(&bodyDef);

	// Create polygon shape
	b2PolygonShape shape;
	shape.Set(scaledVertices, m_vertexCount);

	// Create fixture definition and bind shape
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .7f;
	fixtureDef.shape = &shape;
	m_body->CreateFixture(&fixtureDef);
}

void CustomPolygon::SetColor(const sf::Color& color)
{
	m_color = color;
}

void CustomPolygon::SetWireframe(bool wireframe)
{
	m_wireframe = wireframe;
	if (m_wireframe)
		m_vertexArray.setPrimitiveType(sf::LineStrip);
	else
		m_vertexArray.setPrimitiveType(sf::TriangleFan);
}

bool CustomPolygon::IsWireframe() const
{
	return m_wireframe;
}

void CustomPolygon::DeleteBody()
{
	if (m_body != nullptr)
	{
		m_world->DestroyBody(m_body);
		m_body = nullptr;
	}
}

void CustomPolygon::Update()
{
	if (m_body->GetType() == b2_dynamicBody)
	{
		b2Fixture* fixture = m_body->GetFixtureList();

		while (fixture != NULL)
		{
			switch (fixture->GetType())
			{
				case b2Shape::e_polygon:
				{
					// Get polygon shape to access vertices
					b2PolygonShape* shape =
						dynamic_cast<b2PolygonShape*>(fixture->GetShape());

					// Update SFML vertex array
					for (std::size_t i = 0; i < m_vertexCount; ++i)
					{
						b2Vec2 point = m_body->GetWorldPoint(shape->m_vertices[i]);
						m_vertexArray[i].position = sf::Vector2f(point.x*SCALE, point.y*SCALE);
					}

					// Connect last vertex with first vertex for a closed shape
					b2Vec2 firstPoint = m_body->GetWorldPoint(shape->m_vertices[0]);
					m_vertexArray[m_vertexCount].position =
						sf::Vector2f(firstPoint.x*SCALE, firstPoint.y*SCALE);

					break;
				}
				default:
					break;
			}

			fixture = fixture->GetNext();
		}
	}
}

sf::Vector2f CustomPolygon::GetBodyPosition() const
{
	b2Vec2 pos = m_body->GetWorldCenter();
	return sf::Vector2f(pos.x*SCALE, pos.y*SCALE);
}

void CustomPolygon::Draw(sf::RenderWindow& window)
{
	window.draw(m_vertexArray);
}