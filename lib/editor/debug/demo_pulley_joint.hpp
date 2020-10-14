#ifndef DEMO_PULLEY_JOINT_HPP
#define DEMO_PULLEY_JOINT_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/debug/demo_structs.hpp"

class DemoPulleyJoint
{
private:
	b2World* 		m_world;
	b2Joint*		m_joint;

	BoxSprite		m_box;
	CircleSprite	m_circle;

public:
	DemoPulleyJoint(b2World* world)
	{
		m_world = world;
		m_joint = nullptr;

		// body definition
		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(200.f/SCALE, 280.f/SCALE);
		bodyDef.type = b2_dynamicBody;
		bodyDef.angularDamping = .1;

		// Box shape definition
		b2PolygonShape boxShape;
		boxShape.SetAsBox(15.f/SCALE, 15.f/SCALE);
		m_box.SetSize(sf::Vector2f(30.f, 30.f));

		// Circle shape definition
		b2CircleShape circleShape;
		circleShape.m_radius = 10.f/SCALE;

		// Fixture definition
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.f;
		fixtureDef.friction = .7f;
		fixtureDef.shape = &boxShape;

		// BodyA (box)
		b2Body* bodyA = m_world->CreateBody(&bodyDef);
		bodyA->CreateFixture(&fixtureDef);

		// BodyB (circle)
		fixtureDef.shape = &circleShape;
		fixtureDef.density = 2.f;
		bodyDef.position.Set(230.f/SCALE, 280.f/SCALE);
		b2Body* bodyB = m_world->CreateBody(&bodyDef);
		bodyB->CreateFixture(&fixtureDef);

		// Pulley joint
		b2Vec2 anchor1 = bodyA->GetWorldCenter();
		b2Vec2 anchor2 = bodyB->GetWorldCenter();

		b2Vec2 groundAnchor1(bodyA->GetWorldCenter().x,
							 bodyA->GetWorldCenter().y - (50.f/SCALE));

		b2Vec2 groundAnchor2(bodyB->GetWorldCenter().x,
							 bodyB->GetWorldCenter().y - (50.f/SCALE));

		float ratio = 1.f;
		b2PulleyJointDef jointDef;

		jointDef.collideConnected = true;
		jointDef.Initialize(
			bodyA, bodyB, groundAnchor1, groundAnchor2, anchor1, anchor2, ratio);

		m_joint = m_world->CreateJoint(&jointDef);

		// TODO: Lines representing pulley
	}

	~DemoPulleyJoint()
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
		m_box.SetPosition(sf::Vector2f(aPos.x*SCALE, aPos.y*SCALE));
		m_box.m_sprite.setRotation(m_joint->GetBodyA()->GetAngle()*180/b2_pi);

		b2Vec2 bPos = m_joint->GetBodyB()->GetWorldCenter();
		m_circle.SetPosition(sf::Vector2f(bPos.x*SCALE, bPos.y*SCALE));
		m_circle.m_sprite.setRotation(m_joint->GetBodyB()->GetAngle()*180/b2_pi);
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_box.m_sprite);
		window.draw(m_circle.m_sprite);
	}
};

#endif