#ifndef DEMO_DISTANCE_JOINT_HPP
#define DEMO_DISTANCE_JOINT_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/mouse_utils.hpp"
#include "editor/debug/demo_structs.hpp"

/** DemoDistanceJoint Class
 *
 * TODO: Despawn when outside of level bounds
 */

class DemoDistanceJoint
{
private:
	CircleSprite		m_circleA;
	CircleSprite 		m_circleB;

	b2World*			m_world;
	b2Joint*			m_joint;

	sf::VertexArray		m_line;

	bool 				m_leftMouseDown;
	bool 				m_mouseMoved;
	bool 				m_hoveringOnCircle;
	sf::Vector2f 		m_currMousePos;
	sf::Vector2f		m_prevMousePos;

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
		m_leftMouseDown = false;
		m_mouseMoved = false;
		m_hoveringOnCircle = false;

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
			m_joint = nullptr;
		}
	}

	void HandleInput(const sf::Event& event)
	{
		// Mouse Button Pressed
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_leftMouseDown = true;
			}
		}

		// Mouse move
		if (event.type == sf::Event::MouseMoved)
		{
			m_mouseMoved = true;
		}
		else
		{
			m_mouseMoved = false;
		}

		// Mouse Button Released
		if (event.type == sf::Event::MouseButtonReleased)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_leftMouseDown = false;
			}
		}
	}

	void Update(sf::RenderWindow& window)
	{
		auto mousePos = GetMousePosition(window);

		b2Body* bodyA = m_joint->GetBodyA();
		b2Body* bodyB = m_joint->GetBodyB();

		if (m_hoveringOnCircle && m_leftMouseDown)
		{
			// Get move increment
			sf::Vector2f moveIncrement = GetIncrement(m_prevMousePos, mousePos);

			// Update sprite and position
			m_circleA.SetPosition(m_circleA.m_position + moveIncrement);
			m_line[0].position = m_circleA.m_position;

			// Update Box2D world
			b2Vec2 posA(m_circleA.m_position.x/SCALE, m_circleA.m_position.y/SCALE);
			bodyA->SetTransform(posA, bodyA->GetAngle());

			// Sync circleB sprite with Box2D body
			b2Vec2 bpos = bodyB->GetWorldCenter();
			m_circleB.SetPosition(sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE));
			m_circleB.m_sprite.setRotation(bodyB->GetAngle() * 180/b2_pi);
			m_line[1].position = sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE);

			// Stop forces acting on bodies
			m_joint->GetBodyA()->SetLinearVelocity(b2Vec2(0,0));
			m_joint->GetBodyA()->SetAngularVelocity(0);
			m_joint->GetBodyB()->SetLinearVelocity(b2Vec2(0,0));
			m_joint->GetBodyB()->SetAngularVelocity(0);
		}
		else
		{
			b2Vec2 apos = m_joint->GetBodyA()->GetWorldCenter();
			m_circleA.SetPosition(sf::Vector2f(apos.x*SCALE, apos.y*SCALE));
			m_circleA.m_sprite.setRotation(bodyA->GetAngle() * 180/b2_pi);
			m_line[0].position = sf::Vector2f(apos.x*SCALE, apos.y*SCALE);

			b2Vec2 bpos = m_joint->GetBodyB()->GetWorldCenter();
			m_circleB.SetPosition(sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE));
			m_circleB.m_sprite.setRotation(bodyB->GetAngle() * 180/b2_pi);
			m_line[1].position = sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE);
		}

		if (m_mouseMoved)
		{
			if (m_circleA.intersects(mousePos))
			{
				m_hoveringOnCircle = true;
			}
			else
			{
				m_hoveringOnCircle = false;
			}
		}

		m_prevMousePos = mousePos;
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_circleA.m_sprite);
		window.draw(m_circleB.m_sprite);
		window.draw(m_line);
	}
};

#endif