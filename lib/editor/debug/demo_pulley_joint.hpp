#ifndef DEMO_PULLEY_JOINT_HPP
#define DEMO_PULLEY_JOINT_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/debug/demo_structs.hpp"

/** DemoPulleyJoint class
 *
 * TODO: LMB to apply force and torque.
 */

class DemoPulleyJoint
{
private:
	b2World* 		m_world;
	b2Joint*		m_joint;

	BoxSprite		m_box;
	CircleSprite	m_circle;

	sf::VertexArray	m_line1;
	sf::VertexArray	m_line2;

	sf::Vector2f	m_groundAnchor1;
	sf::Vector2f	m_groundAnchor2;

public:
	DemoPulleyJoint(b2World* world)
	{
		m_world = world;
		m_joint = nullptr;

		// body definition
		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(350.f/SCALE, 300.f/SCALE);
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
		fixtureDef.density = 1.f;
		bodyDef.position.Set(400.f/SCALE, 280.f/SCALE);
		b2Body* bodyB = m_world->CreateBody(&bodyDef);
		bodyB->CreateFixture(&fixtureDef);

		// Pulley joint
		b2Vec2 anchor1 = bodyA->GetWorldCenter();
		b2Vec2 anchor2 = bodyB->GetWorldCenter();

		b2Vec2 groundAnchor1(
			bodyA->GetWorldCenter().x,
			bodyA->GetWorldCenter().y - (50.f/SCALE));

		b2Vec2 groundAnchor2(bodyB->GetWorldCenter().x,
							 bodyB->GetWorldCenter().y - (50.f/SCALE));

		float ratio = 1.f;
		b2PulleyJointDef jointDef;

		jointDef.collideConnected = true;
		jointDef.Initialize(
			bodyA, bodyB, groundAnchor1, groundAnchor2, anchor1, anchor2, ratio);

		m_joint = m_world->CreateJoint(&jointDef);

		// Ground anchor data members
		m_groundAnchor1.x = groundAnchor1.x * SCALE;
		m_groundAnchor1.y = groundAnchor1.y * SCALE;

		m_groundAnchor2.x = groundAnchor2.x * SCALE;
		m_groundAnchor2.y = groundAnchor2.y * SCALE;

		// Line position
		m_line1.resize(2);
		m_line1.setPrimitiveType(sf::LineStrip);

		m_line1[0].position = m_groundAnchor1;
		m_line1[1].position = m_box.m_position;

		m_line2.resize(2);
		m_line2.setPrimitiveType(sf::LineStrip);

		m_line2[0].position = m_groundAnchor2;
		m_line2[1].position = m_circle.m_position;

		// Line color
		m_line1[0].color = sf::Color::Blue;
		m_line1[1].color = sf::Color::Blue;

		m_line2[0].color = sf::Color::Blue;
		m_line2[1].color = sf::Color::Blue;
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

		m_line1[1].position.x = aPos.x*SCALE;
		m_line1[1].position.y = aPos.y*SCALE;

		m_line2[1].position.x = bPos.x*SCALE;
		m_line2[1].position.y = bPos.y*SCALE;
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_box.m_sprite);
		window.draw(m_circle.m_sprite);

		window.draw(m_line1);
		window.draw(m_line2);
	}
};

#endif