#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <SFML/Graphics.hpp>

static const float SCALE = 30.f;
static const float SQUARE_SIZE = 32.f;
static const float CIRCLE_RADIUS = 18.f;

enum class RMBMode
{
	PanCameraMode = 1,
	BoxSpawnMode,
	TestPointMode,
	RayCastMode
};

struct EditorSettings
{
	static RMBMode 		 mode;
	static sf::Vector2u  levelSize;
	static sf::Vector2f  RESOLUTION;
};

#endif