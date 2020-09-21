#ifndef HELLO_WORLD_HPP
#define HELLO_WORLD_HPP

#include <box2d/box2d.h>
#include <iostream>

void helloWorld()
{
	/** Creating a b2World
	*/

	// First define a gravity vector
	b2Vec2 gravity(0.f, -10.f);

	// Create the world object (on stack in this case)
	b2World world(gravity);


	/** Creating a Ground Box
	*/

	// Body definition defines initial position (static body by default)
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.f, -10.f);

	// Create the ground body
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	// Create a ground polygon (box(hx, hy))
	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.f, 10.f);

	// Create a shape fixture using shortcut (shape density kg p/meter squared, 0 for static bodies)
	groundBody->CreateFixture(&groundBox, 0.0f);


	/** Creating a Dynamic Body
	*/

	// Create the body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.f, 4.f);
	b2Body* body = world.CreateBody(&bodyDef);

	// Create and attach polgon shape using a fixture definition
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.f, 1.f);


	/** TMP **/

	/** Circle shapes
	*/
	b2CircleShape circle;
	circle.m_p.Set(2.f, 3.f);
	circle.m_radius = .5f;

	/** Convex polygons
	*/

	// Define a triangle in CCW order
	b2Vec2 vertices[3];
	vertices[0].Set(0.f, 0.f);
	vertices[1].Set(1.f, 0.f);
	vertices[2].Set(0.f, 1.f);

	int32 count = 3;
	b2PolygonShape polygon;
	polygon.Set(vertices, count);

	/** Edge shapes
	*/

	// Define an edge with ghost vertices on either side of edge
	b2Vec2 v0(1.7f, 0.f);
	b2Vec2 v1(1.f, .25f);
	b2Vec2 v2(0.f, 0.f);
	b2Vec2 v3(-1.7f, .4f);
	b2EdgeShape edge;
	edge.SetOneSided(v0, v1, v2, v3);

	/** Chain shapes
	*/

	// Chain shapes - loop (CCW = outward normal, CW = inward normal)
	b2Vec2 vs[4];
	vs[0].Set(1.7f, 0.f);
	vs[1].Set(1.f, .25f);
	vs[2].Set(0.f, 0.f);
	vs[2].Set(-1.7f, .4f);
	b2ChainShape chain;
	chain.CreateLoop(vs, 4);

	// To connect several chains together using ghost vertices (I.e., in scrolling game worlds)
	b2ChainShape chain2;
	b2Vec2 prev(1.9f, 0.f);
	b2Vec2 next(-1.8f, .4f);
	chain2.CreateChain(vs, 4, prev, next);

	// Visit each child edge in chain
	for (int32 i = 0; i < chain.GetChildCount(); ++i) {
		b2EdgeShape edge;
		chain.GetChildEdge(&edge, i);
	}

	/** Shape point test
	*/
	b2Transform transform;
	transform.SetIdentity();
	b2Vec2 point(5.f, 2.f);
	bool hit = polygon.TestPoint(transform, point);

	/** Shape Ray Cast
	*/
	b2Transform transform2;
	transform2.SetIdentity();

	b2RayCastInput input;
	input.p1.Set(0.f, 0.f);
	input.p2.Set(1.f, 0.f);
	input.maxFraction = 1.f;

	int32 childIndex; // for chain shapes
	b2RayCastOutput output;

	bool hit2 = chain.RayCast(&output, input, transform2, childIndex);
	if (hit2) {
		b2Vec2 hitPoint = input.p1 + output.fraction * (input.p2 - input.p1);
		std::cout << "hit (" << hitPoint.x << "," << hitPoint.y << ")\n";
	}

	/** END TMP **/


	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.f;
	fixtureDef.friction = .3f;

	// Create fixture using definition
	body->CreateFixture(&fixtureDef);


	/** Simulating the world
	*/

	// Create a time step
	float timeStep = 1.f/60.f;

	int32 velocityIterations = 6;		// 8 default
	int32 positionInterations = 2;		// 3 default

	// Call b2World::Step to process the simulation loop; one call usually enough per frame
	for (int32 i = 0; i < 60; ++i)
	{
		world.Step(timeStep, velocityIterations, positionInterations);
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		std::cout << "position: (" << position.x << "," << position.y << ")\tangle: " << angle << "radians\n";
	}
}

#endif