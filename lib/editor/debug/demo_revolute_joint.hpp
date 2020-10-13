#ifndef DEMO_REVOLUTE_JOINT_HPP
#define DEMO_REVOLUTE_JOINT_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/debug/demo_structs.hpp"

/** DemoRevoluteJoint Class
 */

class DemoRevoluteJoint
{
private:
	BoxSprite 	m_boxA;
	BoxSprite 	m_boxB;

	b2World*	m_world;
	b2Joint*	m_joint;

private:
	void InitJoint()
	{
		// Body definition
		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(260.f/SCALE, 100.f/SCALE);
		bodyDef.type = b2_dynamicBody;
		bodyDef.angularDamping = .1;

		// Shape definition
		b2PolygonShape shape;
		shape.SetAsBox(10.f/SCALE, 10.f/SCALE);

		// Fixture definition
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.f;
		fixtureDef.friction = .7f;
		fixtureDef.shape = &shape;

		// BodyA (Box)
		b2Body* bodyA = m_world->CreateBody(&bodyDef);
		bodyA->CreateFixture(&fixtureDef);

		// BodyB (Box)
		bodyDef.position = b2Vec2(340.f/SCALE, 100.f/SCALE);
		b2Body* bodyB = m_world->CreateBody(&bodyDef);
		bodyB->CreateFixture(&fixtureDef);

		// Revolute joint
		b2RevoluteJointDef jointDef;
		b2Vec2 hingePoint(300.f/SCALE, 80.f/SCALE);
		jointDef.Initialize(bodyA, bodyB, hingePoint);
		jointDef.collideConnected = false;

		jointDef.lowerAngle = -0.5f * b2_pi;	// -90 degrees
		jointDef.upperAngle = 0.25f * b2_pi;	// 45 degrees
		jointDef.enableLimit = true;

		jointDef.maxMotorTorque = 10.f;			// max torque force
		jointDef.motorSpeed = 0.f;				// simulate friction
		jointDef.enableMotor = true;

		m_joint = m_world->CreateJoint(&jointDef);
	}

public:
	DemoRevoluteJoint(b2World* world)
	{
		m_world = world;
		m_joint = nullptr;

		InitJoint();
	}

	~DemoRevoluteJoint()
	{
		if (m_joint != nullptr)
		{
			m_world->DestroyJoint(m_joint);
			m_joint = nullptr;
		}
	}

	void Update()
	{
		b2Vec2 aPos = m_joint->GetBodyA()->GetWorldCenter();
		m_boxA.SetPosition(sf::Vector2f(aPos.x*SCALE, aPos.y*SCALE));
		m_boxA.m_sprite.setRotation(m_joint->GetBodyA()->GetAngle()*180/b2_pi);

		b2Vec2 bPos = m_joint->GetBodyB()->GetWorldCenter();
		m_boxB.SetPosition(sf::Vector2f(bPos.x*SCALE, bPos.y*SCALE));
		m_boxB.m_sprite.setRotation(m_joint->GetBodyB()->GetAngle()*180/b2_pi);
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_boxA.m_sprite);
		window.draw(m_boxB.m_sprite);
	}
};

#endif