#include "utils/debug_circle.hpp"

using sf::Vector2f;
using sf::Color;
using sf::RenderWindow;
using std::string;
using std::cout;

DebugCircle::DebugCircle(const Vector2f& position,
	b2World* world) : DebugShape(position, "debug_circle")
{
	++DebugCircleCount;
	m_radius = 18.f;
	m_world = world;

	m_sprite.setPosition(m_position);
	m_sprite.setRadius(m_radius);
	m_sprite.setOrigin(m_radius, m_radius);
	m_sprite.setFillColor(Color::White);
	m_sprite.setOutlineColor(Color::Black);
	m_sprite.setOutlineThickness(2);

	CreateBody();
}

DebugCircle::~DebugCircle()
{
	if (m_body != nullptr)
	{
		m_world->DestroyBody(m_body);
		m_body = nullptr;
	}

	--DebugCircleCount;

	cout << "--BodyCount (DebugCirle)\n";
}

void DebugCircle::CreateBody()
{
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(m_position.x/SCALE, m_position.y/SCALE);
	bodyDef.type = b2_dynamicBody;
	bodyDef.angularDamping = .1f;
	bodyDef.linearDamping = 0.f;
	bodyDef.allowSleep = true;
	bodyDef.awake = true;

	if (!m_tag->empty())
		bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(m_tag);

	m_body = m_world->CreateBody(&bodyDef);

	// Define shape and body physical properties with b2FixtureDef
	b2CircleShape shape;
	shape.m_radius = m_radius / SCALE;

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .7f;
	fixtureDef.shape = &shape;
	m_body->CreateFixture(&fixtureDef);
}

void DebugCircle::DeleteBody()
{
	if (m_body != nullptr)
	{
		m_world->DestroyBody(m_body);
		m_body = nullptr;
	}
}

void DebugCircle::Update()
{
	if (m_body->GetType() == b2_dynamicBody)
	{
		b2Fixture* fixture = m_body->GetFixtureList();

		while (fixture != NULL)
		{
			switch (fixture->GetType())
			{
				case b2Shape::e_circle:
				{
					m_position.x = m_body->GetPosition().x * SCALE;
					m_position.y = m_body->GetPosition().y * SCALE;
					m_sprite.setPosition(m_position);
					m_sprite.setRotation(m_body->GetAngle() * 180/b2_pi);
					break;
				}
				default:
					break;
			}

			fixture = fixture->GetNext();
		}
	}
}

void DebugCircle::Draw(RenderWindow& window)
{
	window.draw(m_sprite);
}

string* DebugCircle::GetUserData()
{
	b2BodyUserData data = m_body->GetUserData();

	if (data.pointer != 0)
	{
		return reinterpret_cast<string*>(data.pointer);
	}

	return nullptr;
}
