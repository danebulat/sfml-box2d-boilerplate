#include "editor/debug/multi_shape.hpp"

using sf::Vector2f;
using sf::RenderWindow;
using sf::Color;
using std::vector;
using std::size_t;
using std::cout;

#include <cstdlib>

MultiShape::MultiShape(const Vector2f& position, b2World* world)
	: DebugShape(position, "custom_polygon")
{
	++MultiShapeCount;

	CreateMultipleFixtureBody();
}


MultiShape::~MultiShape()
{
	if (m_body != nullptr)
	{
		m_world->DestroyBody(m_body);
		m_body = nullptr;
	}

	--MultiShapeCount;

	//cout << "--BodyCount (CustomPolygon)\n";
}

void MultiShape::CreateMultipleFixtureBody()
{
	m_shape1 = {
		Vector2f(-50.f, 0.f),
		Vector2f(0.f, -75.f),
		Vector2f(50.f, 0.f),
		Vector2f(0.f, 20.f)
	};

	m_shape2 = {
		Vector2f(-30.f, -60.f),
		Vector2f(30.f, -60.f),
		Vector2f(10.f, -100.f),
		Vector2f(-10.f, -100.f)
	};

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
	bodyDef.angle = (float)(rand() % 360);

	if (!m_tag->empty())
		bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(m_tag);

	m_body = m_world->CreateBody(&bodyDef);

	// Create shape1
	b2PolygonShape shape1;
	b2Vec2 scaledShape1[m_shape1.size()];
	for (int i = 0; i < 4; ++i) {
		scaledShape1[i].x = m_shape1[i].x / SCALE;
		scaledShape1[i].y = m_shape1[i].y / SCALE;
	}
	shape1.Set(scaledShape1, m_shape1.size());

	// Create fixture def 1
	b2FixtureDef fixtureDef1;
	fixtureDef1.shape = &shape1;
	fixtureDef1.density = 1.f;
	fixtureDef1.friction = .7f;
	fixtureDef1.restitution = .8f;
	m_body->CreateFixture(&fixtureDef1);

	// Create shape2
	b2PolygonShape shape2;
	b2Vec2 scaledShape2[m_shape2.size()];
	for (int i = 0; i < 4; ++i) {
		scaledShape2[i].x = m_shape2[i].x / SCALE;
		scaledShape2[i].y = m_shape2[i].y / SCALE;
	}
	shape2.Set(scaledShape2, m_shape2.size());

	// Create fixture def 2
	b2FixtureDef fixtureDef2;
	fixtureDef2.shape = &shape2;
	fixtureDef2.density = 1.f;
	fixtureDef2.friction = .7f;
	fixtureDef2.restitution = 0;
	m_body->CreateFixture(&fixtureDef2);


	// Initialise SFML vertex arrays
	m_wireframe ? m_va1.setPrimitiveType(sf::LinesStrip) :
				  m_va1.setPrimitiveType(sf::TriangleFan);
	m_wireframe ? m_va2.setPrimitiveType(sf::LinesStrip) :
				  m_va2.setPrimitiveType(sf::TriangleFan);

	m_va1.resize(5);
	m_va2.resize(5);

	// Vertices for first shape
	for (int i = 0; i < 4; ++i) {
		m_va1[i].position = m_shape1[i];
		m_va1[i].color = Color::Cyan;
	}
	m_va1[4].position.x = m_shape1.front().x;
	m_va1[4].position.y = m_shape1.front().y;
	m_va1[4].color = Color::Cyan;

	// Vertices for second shape
	for (int i = 0; i < 4; ++i) {
		m_va2[i].position = m_shape2[i];
		m_va2[i].color = Color::Blue;
	}

	// std::cout << m_shape2.front().x << " " << m_shape2.front().y << "\n";
	m_va2[4].position.x = m_shape2.front().x;
	m_va2[4].position.y = m_shape2.front().y;
	m_va2[4].color = Color::Blue;
}

void MultiShape::Update()
{
	int fixture_index = 0;
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
				if (fixture_index == 0)
				{
					for (size_t i = 0; i < 4; ++i)
					{
						b2Vec2 point = m_body->GetWorldPoint(shape->m_vertices[i]);
						m_va1[i].position = Vector2f(point.x*SCALE, point.y*SCALE);
					}

					b2Vec2 firstPoint = m_body->GetWorldPoint(shape->m_vertices[0]);
					m_va1[m_shape1.size()].position =
						Vector2f(firstPoint.x*SCALE, firstPoint.y*SCALE);

					++fixture_index;
				}
				else
				{
					for (size_t i = 0; i < 4; ++i)
					{
						b2Vec2 point = m_body->GetWorldPoint(shape->m_vertices[i]);
						m_va2[i].position = Vector2f(point.x*SCALE, point.y*SCALE);
					}

					b2Vec2 firstPoint = m_body->GetWorldPoint(shape->m_vertices[0]);
					m_va2[m_shape2.size()].position =
						Vector2f(firstPoint.x*SCALE, firstPoint.y*SCALE);
				}
			}

			default:
				break;
		}

		fixture = fixture->GetNext();
	}
}

void MultiShape::SetWireframe(bool wireframe)
{
	m_wireframe = wireframe;

	if (m_wireframe)
	{
		m_va1.setPrimitiveType(sf::LineStrip);
		m_va2.setPrimitiveType(sf::LineStrip);
	}
	else
	{
		m_va1.setPrimitiveType(sf::TriangleFan);
		m_va2.setPrimitiveType(sf::TriangleFan);
	}
}

Vector2f MultiShape::GetBodyPosition() const
{
	b2Vec2 pos = m_body->GetWorldCenter();
	return Vector2f(pos.x*SCALE, pos.y*SCALE);
}

void MultiShape::Draw(RenderWindow& window)
{
	window.draw(m_va2);
	window.draw(m_va1);
}

void MultiShape::DoTestPoint(const Vector2f& point)
{
	if (m_body->GetType() == b2_dynamicBody)
	{
		b2Fixture* fixture = m_body->GetFixtureList();

		while (fixture != NULL)
		{
			b2Vec2 scaledPoint(point.x/SCALE, point.y/SCALE);

			if (fixture->TestPoint(scaledPoint))
			{
				SetVertexColor(Color::Green);
			}
			else
			{
				SetVertexColor(Color::Magenta);
			}

			fixture = fixture->GetNext();
		}
	}
}


void MultiShape::ResetTestPoint()
{
	SetVertexColor(Color::Magenta);
}

void MultiShape::SetVertexColor(const Color& color)
{
	for (int i = 0; i < m_shape1.size(); ++i)
		m_va1[i].color = color;

	for (int i = 0; i < m_shape2.size(); ++i)
		m_va2[i].color = color;
}