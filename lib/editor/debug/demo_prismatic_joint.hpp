#ifndef DEMO_PRISMATIC_JOINT_HPP
#define DEMO_PRISMATIC_JOINT_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/mouse_utils.hpp"
#include "editor/constants.hpp"
#include "editor/debug/demo_structs.hpp"

class DemoPrismaticJoint
{
private:
	BoxSprite 	m_boxA;
	BoxSprite 	m_boxB;

	b2World*	m_world;
	b2Joint*	m_joint;

	bool 				m_leftMouseDown;
	bool 				m_mouseMoved;
	bool 				m_hoveringOnCircle;
	sf::Vector2f 		m_currMousePos;
	sf::Vector2f		m_prevMousePos;

public:
	DemoPrismaticJoint(b2World* world)
	{
		m_leftMouseDown = false;
		m_mouseMoved = false;
		m_hoveringOnCircle = false;

		m_world = world;
		m_joint = nullptr;

		// Body Definition
		b2BodyDef bodyDef;
		bodyDef.position = b2Vec2(280.f/SCALE, 180.f/SCALE);
		bodyDef.type = b2_dynamicBody;
		bodyDef.angularDamping = .1f;

		// Shape definition
		b2PolygonShape shape;
		shape.SetAsBox(10.f/SCALE, 10.f/SCALE);

		// Fixture definition
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.f;
		fixtureDef.friction = .7f;
		fixtureDef.shape = &shape;

		// BoxA (Box)
		b2Body* bodyA = m_world->CreateBody(&bodyDef);
		bodyA->CreateFixture(&fixtureDef);

		// BoxB (Box)
		bodyDef.position.Set(340.f/SCALE,180.f/SCALE);
		b2Body* bodyB = m_world->CreateBody(&bodyDef);
		bodyB->CreateFixture(&fixtureDef);

		// Prismatic joint
		b2PrismaticJointDef jointDef;
		b2Vec2 worldAxis(1.f, 0.f);
		jointDef.Initialize(bodyA, bodyB, bodyA->GetWorldCenter(), worldAxis);

		jointDef.lowerTranslation = -5.f;
		jointDef.upperTranslation = 2.5f;
		jointDef.enableLimit = true;

		jointDef.maxMotorForce = 2.f;
		jointDef.motorSpeed = 5.f;
		jointDef.enableMotor = true;

		m_joint = m_world->CreateJoint(&jointDef);
	}

	~DemoPrismaticJoint()
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
			m_boxA.SetPosition(m_boxA.m_position + moveIncrement);

			// Update Box2D world
			b2Vec2 posA(m_boxA.m_position.x/SCALE, m_boxA.m_position.y/SCALE);
			bodyA->SetTransform(posA, bodyA->GetAngle());

			// Sync boxB with Box2D body
			b2Vec2 bpos = bodyB->GetWorldCenter();
			m_boxB.SetPosition(sf::Vector2f(bpos.x*SCALE, bpos.y*SCALE));
			m_boxB.m_sprite.setRotation(bodyB->GetAngle() * 180/b2_pi);

			// Stop forces acting on bodies
			m_joint->GetBodyA()->SetLinearVelocity(b2Vec2(0,0));
			m_joint->GetBodyA()->SetAngularVelocity(0);
			m_joint->GetBodyB()->SetLinearVelocity(b2Vec2(0,0));
			m_joint->GetBodyB()->SetAngularVelocity(0);
		}
		else
		{
			b2Vec2 aPos = m_joint->GetBodyA()->GetWorldCenter();
			m_boxA.SetPosition(sf::Vector2f(aPos.x*SCALE, aPos.y*SCALE));
			m_boxA.m_sprite.setRotation(m_joint->GetBodyA()->GetAngle()*180/b2_pi);

			b2Vec2 bPos = m_joint->GetBodyB()->GetWorldCenter();
			m_boxB.SetPosition(sf::Vector2f(bPos.x*SCALE, bPos.y*SCALE));
			m_boxB.m_sprite.setRotation(m_joint->GetBodyB()->GetAngle()*180/b2_pi);
		}

		if (m_mouseMoved)
		{
			if (m_boxA.intersects(mousePos))
			{
				m_hoveringOnCircle = true;
				std::cout << "hovering\n";
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
		window.draw(m_boxA.m_sprite);
		window.draw(m_boxB.m_sprite);
	}
};

#endif