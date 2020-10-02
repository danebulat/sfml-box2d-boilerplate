#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SFML/Graphics.hpp>
#include "editor/animation/tween.hpp"
//#include "engine/circle.hpp"

class Camera {
private:

	/** General data
	*/
	sf::Vector2f	m_position;

	sf::Vector2u    m_backgroundSize;
	float 			m_minX;
	float 			m_minY;
	float			m_maxX;
	float			m_maxY;
	bool			m_clampToBackground;

	/** Data for tween animation
	*/
	InterpFunc 		m_interpolation;
	float      		m_duration;
	Tween*			m_tweenX;
	Tween*			m_tweenY;
	bool 			m_tweenXActive;
	bool 			m_tweenYActive;

private:
	void InitDefault();
	void ClampPosition(const sf::Vector2f& pos);
	void CalculateMinMaxPos(
		const sf::Vector2u& backgroundSize, const sf::Vector2f& resolution);

	void SpawnTweenX(float targetX);
	void SpawnTweenY(float targetY);

public:
	/** Initialises a camera with default values.
	*
	* Disables clamping to the background size and provides
	* default interpolation and duration values for the tweens.
	*/
	Camera();

	/** Initialises a camera for clamping to a background.
	*
	* Initialises data members to enable clamping the camera's
	* position to the background container. Default interpolation
	* and duration values are initialised.
	*/
	Camera(const sf::Vector2f& position,
		   const sf::Vector2u& backgroundSize,
		   const sf::Vector2f& resolution,
		   bool  clamp /*=true*/ );

	/** Deallocates camera's tween objects.
	*/
	~Camera();

	// TODO: Copy constructor
	// TODO: Copy assignment operator
	// TODO: Move contructor
	// TODO: Move assignment operator

	/** Public API
	*/
	void AnimateTo(const sf::Vector2f& target);
	void ClampTo(const sf::Vector2u& backgroundSize, const sf::Vector2f& resolution);
	bool IsAnimating() const;
	void Update(float dt, const sf::Vector2f& target);
	void IncrementDuration(float i);

	/** Accessors
	*/
	void SetInterpolation(InterpFunc interp);
	InterpFunc GetInterpolation() const;

	sf::Vector2f GetPosition() const;
	void SetPosition(const sf::Vector2f& pos);

	float GetDuration() const;
	void SetDuration(float duration);
};

#endif