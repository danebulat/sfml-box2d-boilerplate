#ifndef DEMO_PRISMATIC_JOINT_HPP
#define DEMO_PRISMATIC_JOINT_HPP

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include "editor/constants.hpp"
#include "editor/debug/demo_structs.hpp"

class DemoPrismaticJoint
{
private:
	BoxSprite 	m_boxA;
	BoxSprite 	m_boxB;

	b2World*	m_world;
	b2Joint*	m_joint;

public:

};

#endif