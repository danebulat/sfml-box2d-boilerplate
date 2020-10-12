#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"

/** CircleSprite Struct
 */

struct CircleSprite
{
	sf::CircleShape m_sprite;
	sf::Vector2f	m_position;

	CircleSprite()
	{
		m_sprite.setFillColor(sf::Color::White);
		m_sprite.setOutlineColor(sf::Color::Black);
		m_sprite.setOutlineThickness(2.f);
		m_sprite.setRadius(CIRCLE_RADIUS);
		m_sprite.setOrigin(CIRCLE_RADIUS, CIRCLE_RADIUS);
	}

	void SetPosition(const sf::Vector2f& position)
	{
		m_position = position;
		m_sprite.setPosition(m_position);
	}
};

/** DemoDistanceJoint Class
 */

class DemoDistanceJoint
{
private:
	CircleSprite		m_circleA;
	CircleSprite 		m_circleB;

	b2World*			m_world;
	b2Joint*			m_joint;

	sf::VertexArray		m_line;

private:
	void InitJoint()
	{
		// Body Definition
		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(200.f/SCALE, 100.f/SCALE);
		bodyDef.type = b2_dynamicBody;
		bodyDef.angularDamping = .1f;

		// Shape Definition
		b2CircleShape shape;
		shape.m_radius = CIRCLE_RADIUS/SCALE;

		// Fixture Definition
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.7f;
		fixtureDef.shape = &shape;

		// BodyA (circle)
		b2Body* bodyA = m_world->CreateBody(&bodyDef);
		bodyA->CreateFixture(&fixtureDef);

		// BodyB (circle)
		bodyDef.position.Set(250.f/SCALE, 100.0f/SCALE);
		b2Body* bodyB = m_world->CreateBody(&bodyDef);
		bodyB->CreateFixture(&fixtureDef);

		// Distance joint
		b2DistanceJointDef jointDef;
		jointDef.collideConnected = true;
		jointDef.bodyA = bodyA;
		jointDef.bodyB = bodyB;

		jointDef.localAnchorA.Set(0.0f, 0.0f);
		jointDef.localAnchorB.Set(0.0f, 0.0f);

		// Adjust accordinly for a rigid distance
		jointDef.length = 100.f/SCALE;
		jointDef.minLength = 90.f/SCALE;
		jointDef.maxLength = 110.f/SCALE;

		// Soft distance joints can be enabled by setting stiffness and damping
		float frequencyHz = 4.0f;
		float dampingRatio = 0.5f;
		b2LinearStiffness(jointDef.stiffness, jointDef.damping,
			frequencyHz, dampingRatio, jointDef.bodyA, jointDef.bodyB);

		// This code didn't work:
		// b2Vec2 worldAnchorA(200.f/SCALE, 100.f/SCALE);
		// b2Vec2 worldAnchorB(300.f/SCALE, 100.f/SCALE);
		// jointDef.Initialize(bodyA, bodyB, worldAnchorA, worldAnchorB);

		m_joint = m_world->CreateJoint(&jointDef);
	}

public:
	DemoDistanceJoint(b2World* world)
	{
		m_world = world;
		m_joint = nullptr;

		InitJoint();

		m_circleA.m_sprite.setFillColor(sf::Color(204.f, 204.f, 255.f));
		m_circleB.m_sprite.setFillColor(sf::Color(153.f, 255.f, 204.f));

		m_line.setPrimitiveType(sf::LineStrip);
		m_line.resize(2);
		for (uint i=0; i<2; ++i)
			m_line[i].color = sf::Color::Red;
	}

	~DemoDistanceJoint()
	{
		if (m_joint != nullptr)
		{
			m_world->DestroyJoint(m_joint);
		}
	}

	void Update()
	{
		b2Vec2 apos = m_joint->GetBodyA()->GetWorldCenter();
		b2Vec2 bpos = m_joint->GetBodyB()->GetWorldCenter();

		m_circleA.SetPosition(sf::Vector2f(apos.x*SCALE, apos.y*SCALE));
		m_circleB.SetPosition(sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE));

		m_line[0].position = sf::Vector2f(apos.x*SCALE, apos.y*SCALE);
		m_line[1].position = sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE);
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_circleA.m_sprite);
		window.draw(m_circleB.m_sprite);
		window.draw(m_line);
	}
};