#ifndef MY_CONTACT_LISTENER_HPP
#define MY_CONTACT_LISTENER_HPP

#include "box2d/box2d.h"

/** b2ContactListener Example
 */
class MyContactListener : public b2ContactListener
{
	/**
	 * NOTE: Instantiate and register with b2World::SetContactListener.
	 *
	 * NOTE: Can use for sounds and game logic.
	 *
	 * NOTE: You cannot alter the physics world inside a callback as you may
	 *       delete objects Box2D is processing.
	 *
	 * NOTE: Recommended practice for processing contact points: Buffer all
	 *       contact data that you care about and process it after the time step.
	 *
	 * NOTE: Process contact points immediately after time step (some other client
	 *       code may alter the physics world)
	 */

private:
	bool m_beginContactOutput = false;
	bool m_endContactOutput = false;
	bool m_preSolveOutput = false;
	bool m_postSolveOutput = false;

	void OutputContactInfo(b2Contact* contact, const char* eventType)
	{
		std::cout << "---------------------------------------------\n"
			"b2ContactListener::" << eventType << "\n"
			"  GetFriction()    - " << contact->GetFriction() << "\n"
			"  GetRestitution() - " << contact->GetRestitution() << "\n"
			"    FixtureA->GetFriction() - " << contact->GetFixtureA()->GetFriction() << "\n"
			"    FixtureA->GetDensity()  - " << contact->GetFixtureA()->GetDensity() << "\n"
			"    FixtureB->GetFriction() - " << contact->GetFixtureB()->GetFriction() << "\n"
			"    FixtureB->GetDensity()  - " << contact->GetFixtureB()->GetDensity() << "\n\n";
	}

public:

	/** Beging event
	 * Called when two fixtures begin to overlap. Called for sensors
	 * and non-sensors.
	 */
	void BeginContact(b2Contact* contact) override
	{
		if (m_beginContactOutput)
			OutputContactInfo(contact, "BeginContact");
	}

	/** End event
	 * Called when two fixtures cease to overlap. Called for sensors
	 * and non-sensors.
	*/
	void EndContact(b2Contact* contact) override
	{
		if (m_endContactOutput)
			OutputContactInfo(contact, "EndContact");
	}

	/** Pre-solve event
	 * Called after collision detection, but before collision resolution.
	 * - Gives you a chance to disable a contact based on your current configuration.
	 * - May be fired multiple times per time step per contact due to continuous
	 *   collision detection.
	*/
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
	{
		if (m_preSolveOutput)
		{
			OutputContactInfo(contact, "PreSolve");
		}

		// Get contact point world coordinates
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		// Get states of old and current contact points
		b2PointState state1[2], state2[2];
		b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());

		// b2_addState - point was added in the update
		// https://box2d.org/documentation/b2__collision_8h.html#a0a894e3715ce8c61b7958dd6e083663d
    	if (state2[0] == b2_addState)
		{
			const b2Body* bodyA = contact->GetFixtureA()->GetBody();
        	const b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// Get first contact point
        	b2Vec2 point = worldManifold.points[0];

			// Get velocity vectors from both contact bodies
			b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
			b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);

			// Calculate approach velocity
			float approachVelocity = b2Dot(vB - vA, worldManifold.normal);

			std::cout << "> approachVelocity: " << approachVelocity << "\n";

			// Dot product > 1 means vectors point in same direction
			// https://www.mathsisfun.com/algebra/vectors-dot-product.html
			if (approachVelocity > 1.0f)
			{
				/* MyPlayCollisionSound(); */
				std::cout << "> play collision sound\n";
			}
		}
	}

	/** Post-solve event
	 * Where you can gather collision impulse results.
	 * - Implement PreSolve() if you don't need impulses.
	 */
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
	{
		if (m_postSolveOutput)
		{
			OutputContactInfo(contact, "PostSolve");
		}

		contact->SetEnabled(false);
	}
};

/** b2ContactFilter Example
 *
 * Achieve custom contact filtering by implementing a b2ContactFilter class.
 * Returns true if shapes should collide.
 *
 * - Requires an implementation of the ShouldCollide() function that receives two
 *   b2Shape pointers.
 */

class MyContactFilter : public b2ContactFilter
{
public:
	/** Default implementation of ShouldCollide uses b2FilterData.
	 *
	 * Instantiate contact filter at runtime and register to world with
	 * b2World::SetContactFilter.
	 */
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override
	{
		std::cout << "MyContactFilter::ShouldCollide\n";

		const b2Filter& filterA = fixtureA->GetFilterData();
		const b2Filter& filterB = fixtureB->GetFilterData();

		if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0)
			return filterA.groupIndex > 0;

		bool collideA = (filterA.maskBits & filterB.categoryBits) != 0;
		bool collideB = (filterB.categoryBits & filterA.maskBits) != 0;
		bool collide = collideA && collideB;
		return collide;
	}
};

/** MyRayCastCallback
 *
 * You can perform a ray cast by implementing a callback class and providing
 * the start and end points.
 * Invoke the ray cast with b2World::RayCast(callback, point1, point2)
 *
 * - The world class calls your class with each fixture hit by the ray.
 *
 * - The callback is provided the fixture, point of intersection, the unit normal
 *   vector, and fractional distance along the ray.
 *
 * - You control the continuation of the ray cast by returning a fraction:
 *       Return 0  : Ray cast should be terminated
 * 		 Return 1  : Ray cast continues as if no collision occured
 * 		 Return -1 : Filter the fixture
 * 		 Return fraction of 1 : Ray clipped to current intersection point
 */

class MyRayCastCallback : public b2RayCastCallback
{
private:
	b2Fixture* 	m_fixture;
	b2Vec2		m_point;
	b2Vec2 		m_normal;
	float 		m_fraction;

public:
	MyRayCastCallback()
	{
		m_fixture = nullptr;
	}

	float ReportFixture(b2Fixture* fixture,
						const b2Vec2& point,
						const b2Vec2& normal,
						float fraction) override
	{
		m_fixture = fixture;
		m_point = point;
		m_normal = normal;
		m_fraction = fraction;
		return fraction;
	}
};

/** MyDestructionListener
 *
 * If you destroy a body, all associated shapes and joints are automatically
 * destroyed (implicit destruction). You must nullify any pointers you have to
 * those shapes and joints.
 *
 * Box2D provides a listener class b2DestructionListener to help nullify joint
 * and fixture pointers - Implement and provide to b2World object with
 * b2World::SetDestructionListener().
 *
 * - World will notify you when a joint is going to be implicitly destroyed.
 * - Gives the application a chance to nullify orphansed pointers.
 */

class MyDestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Joint* joint) override
	{
		/* Remove all references to joint */
		joint = nullptr;
		std::cout << "MyDestructionListener::SayGoodbye(joint)\n";
	}

	void SayGoodbye(b2Fixture* fixture) override
	{
		/* Remove all references to fixture */
		fixture = nullptr;
		std::cout << "MyDestructionListener::SayGoodbye(fixture)\n";
	}
};

#include <SFML/Graphics.hpp>
#include "editor/constants.hpp"

/** AABB Query
 *
 * Sometimes you want to determine all shapes in a region - you can provide
 * an AABB in world coordinates and an implementation of b2QueryCallback.
 *
 * The world calls your class with each fixture whose AABB overlaps the
 * query AABB.
 */

class MyQueryCallback : public b2QueryCallback
{
public:
	/** ReportFixture
	 *
	 * Return true to continue the query; otherwise false.
	 * Call with world->Query(&callback, aabb)
	 */
	bool ReportFixture(b2Fixture* fixture) override
	{
		std::cout << "MyQueryCallback::ReportFixture - Entered AABB\n";
		b2Body* body = fixture->GetBody();
		body->SetAwake(true);
		return true;
	}
};

class TriggerZone
{
private:
	sf::RectangleShape	m_sprite;
	sf::Color			m_color;
	sf::Vector2f		m_position;
	sf::Vector2f		m_size;

	b2Vec2				m_lower;
	b2Vec2				m_upper;
	MyQueryCallback		m_callback;

public:
	TriggerZone(const sf::Vector2f& position, const sf::Vector2f& size)
	{
		m_position = position;
		m_size = size;
		m_color = sf::Color(0.f, 0.f, 255.f, 64.f);

		m_sprite.setSize(size);
		m_sprite.setPosition(position);
		m_sprite.setFillColor(m_color);

		m_lower.Set(m_position.x/SCALE, m_position.y/SCALE);
		m_upper.Set((m_position.x + m_size.x)/SCALE,
					(m_position.y + m_size.y)/SCALE);
	}

	void Query(b2World* world)
	{
		b2AABB aabb;
		aabb.lowerBound = m_lower;
		aabb.upperBound = m_upper;

		// Query b2World
		world->QueryAABB(&m_callback, aabb);
	}

	void Draw(sf::RenderWindow& window)
	{
		window.draw(m_sprite);
	}
};

#endif