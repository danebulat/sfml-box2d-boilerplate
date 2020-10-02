#ifndef INTERPOLATE_HPP
#define INTERPOLATE_HPP

#include <cmath>

/**
Interpolation functions implemented for all the standard easing
functions documented here:
https://easings.net/

Functions with a single float parameter accept a value between 0 and 1.
*/

class Interpolate {
private:
	static const float fPI;

public:
	Interpolate() = delete;
	Interpolate(const Interpolate&) = delete;
	Interpolate& operator= (const Interpolate&) = delete;

	// linear - no easing, no acceleration
	static float Linear(float t, float b, float c, float d);

	// quadratic easing in - accelerating from zero velocity
	static float EaseInQuad(float t, float b, float c, float d);
	static float EaseInQuad(float t);

	// quadratic easing out - decelerating to zero velocity
	static float EaseOutQuad(float t, float b, float c, float d);
	static float EaseOutQuad(float t);

	// quadratic easing in/out - acceleration until halfway, then deceleration
	static float EaseInOutQuad(float t, float b, float c, float d);
	static float EaseInOutQuad(float t);

	// cubic easing in - accelerating from zero velocity
	static float EaseInCubic(float t, float b, float c, float d);
	static float EaseInCubic(float t);

	// cubic easing out - decelerating to zero velocity
	static float EaseOutCubic(float t, float b, float c, float d);
	static float EaseOutCubic(float t);

	// cubic easing in/out - acceleration until halfway, then deceleration
	static float EaseInOutCubic(float t, float b, float c, float d);
	static float EaseInOutCubic(float t);

	// quartic easing in - accelerating from zero velocity
	static float EaseInQuart(float t, float b, float c, float d);
	static float EaseInQuart(float t);

	// quartic easing out - decelerating to zero velocity
	static float EaseOutQuart(float t, float b, float c, float d);
	static float EaseOutQuart(float t);

	// quartic easing in/out - acceleration until halfway, then deceleration
	static float EaseInOutQuart(float t, float b, float c, float d);
	static float EaseInOutQuard(float t);

	// quintic easing in - accelerating from zero velocity
	static float EaseInQuint(float t, float b, float c, float d);
	static float EaseInQuint(float t);

	// quintic easing out - decelerating to zero velocity
	static float EaseOutQuint(float t, float b, float c, float d);
	static float EaseOutQuant(float t);

	// quintic easing in/out - acceleration until halfway, then deceleration
	static float EaseInOutQuint(float t, float b, float c, float d);
	static float EaseInOutQuint(float t);

	// sinusoidal easing in - accelerating from zero velocity
	static float EaseInSine(float t, float b, float c, float d);
	static float EaseInSine(float t);

	// sinusoidal easing out - decelerating to zero velocity
	static float EaseOutSine(float t, float b, float c, float d);
	static float EaseOutSine(float t);

	// sinusoidal easing in/out - accelerating until halfway, then decelerating
	static float EaseInOutSine(float t, float b, float c, float d);
	static float EaseInOutSine(float t);

	// exponential easing in - accelerating from zero velocity
	static float EaseInExpo(float t, float b, float c, float d);
	static float EaseInExpo(float t);

	// exponential easing out - decelerating to zero velocity
	static float EaseOutExpo(float t, float b, float c, float d);
	static float EaseOutExpo(float t);

	// exponential easing in/out - accelerating until halfway, then decelerating
	static float EaseInOutExpo(float t, float b, float c, float d);
	static float EaseInOutExpo(float t);

	// circular easing in - accelerating from zero velocity
	static float EaseInCirc(float t, float b, float c, float d);
	static float EaseInCirc(float t);

	// circular easing out - decelerating to zero velocity
	static float EaseOutCirc(float t, float b, float c, float d);
	static float EaseOutCirc(float t);

	// circular easing in/out - acceleration until halfway, then deceleration
	static float EaseInOutCirc(float t, float b, float c, float d);
	static float EaseInOutCirc(float t);

	// ease in back - pulls back before easing in
	static float EaseInBack(float t, float b, float c, float d, float c1=1.70158f);
	static float EaseInBack(float t, float c1);

	// ease out back - over throws before easing out
	static float EaseOutBack(float t, float b, float c, float d, float c1=1.70158f);
	static float EaseOutBack(float t, float c1);

	// ease in out back - pull back and over throw
	static float EaseInOutBack(float t, float b, float c, float d, float c1=1.70158f);
	static float EaseInOutBack(float t, float c1);

	// ease in elastic
	static float EaseInElastic(float t, float b, float c, float d);
	static float EaseInElastic(float t);

	// ease out elastic
	static float EaseOutElastic(float t, float b, float c, float d);
	static float EaseOutElastic(float t);

	// ease in out elastic
	static float EaseInOutElastic(float t, float b, float c, float d);
	static float EaseInOutElastic(float t);

	// ease out bounce
	static float EaseOutBounce(float t, float b, float c, float d);
	static float EaseOutBounce(float t);

	// ease in bounce
	static float EaseInBounce(float t, float b, float c, float d);
	static float EaseInBounce(float t);

	// ease in out bounce
	static float EaseInOutBounce(float t, float b, float c, float d);
};


#endif