#include "editor/animation/interpolate.hpp"

const float Interpolate::fPI = static_cast<float>(M_PI);

float Interpolate::Linear(float t, float b, float c, float d) {
	return c * t/d + b;
}

// quadratic easing in - accelerating from zero velocity
float Interpolate::EaseInQuad(float t, float b, float c, float d) {
	t /= d;
	return c*t*t + b;
}

float Interpolate::EaseInQuad(float t) {
	return t*t;
}

// quadratic easing out - decelerating to zero velocity
float Interpolate::EaseOutQuad(float t, float b, float c, float d) {
	t /= d;
	return -c * t*(t-2.f) + b;
}

float Interpolate::EaseOutQuad(float t) {
	return 1.f - (1.f - t) * (1.f - t);
}

// quadratic easing in/out - acceleration until halfway, then deceleration
float Interpolate::EaseInOutQuad(float t, float b, float c, float d) {
	t /= d/2.f;
	if (t < 1.f)
		return c/2.f*t*t + b;
	t--;
	return -c/2.f * (t*(t-2.f) - 1.f) + b;
}

float Interpolate::EaseInOutQuad(float t) {
	return t < 0.5f ?
		2.f * t * t :
		1.f - pow(-2.f * t + 2.f, 2.f) / 2.f;
}

// cubic easing in - accelerating from zero velocity
float Interpolate::EaseInCubic(float t, float b, float c, float d) {
	t /= d;
	return c*t*t*t + b;
}

float Interpolate::EaseInCubic(float t) {
	return t * t * t;
}

// cubic easing out - decelerating to zero velocity
float Interpolate::EaseOutCubic(float t, float b, float c, float d) {
	t /= d;
	t--;
	return c*(t*t*t + 1.f) + b;
}

float Interpolate::EaseOutCubic(float t) {
	return 1.f - std::powf(1.f - t, 3);
}

// cubic easing in/out - acceleration until halfway, then deceleration
float Interpolate::EaseInOutCubic(float t, float b, float c, float d) {
	t /= d/2.f;
	if (t < 1.f)
		return c/2.f*t*t*t + b;
	t -= 2.f;
	return c/2*(t*t*t + 2.f) + b;
}

float Interpolate::EaseInOutCubic(float t) {
	return t < 0.5f ?
		4.f * t * t * t :
		1.f - std::powf(-2.f * t + 2.f, 3.f) / 2.f;
}

// quartic easing in - accelerating from zero velocity
float Interpolate::EaseInQuart(float t, float b, float c, float d) {
	t /= d;
	return c*t*t*t*t + b;
}

float Interpolate::EaseInQuart(float t) {
	return t * t * t * t;
}

// quartic easing out - decelerating to zero velocity
float Interpolate::EaseOutQuart(float t, float b, float c, float d) {
	t /= d;
	t--;
	return -c * (t*t*t*t - 1.f) + b;
}

float Interpolate::EaseOutQuart(float t) {
	return 1.f - std::powf(1 - t, 4.f);
}

// quartic easing in/out - acceleration until halfway, then deceleration
float Interpolate::EaseInOutQuart(float t, float b, float c, float d) {
	t /= d/2.f;
	if (t < 1.f) return c/2.f*t*t*t*t + b;
	t -= 2.f;
	return -c/2.f * (t*t*t*t - 2.f) + b;
}

float Interpolate::EaseInOutQuard(float t) {
	return t < 0.5f ?
		8.f * t * t * t * t :
		1.f - std::powf(-2.f * t + 2.f, 4.f) / 2.f;
}

// quintic easing in - accelerating from zero velocity
float Interpolate::EaseInQuint(float t, float b, float c, float d) {
	t /= d;
	return c*t*t*t*t*t + b;
}

float Interpolate::EaseInQuint(float t) {
	return t * t * t * t * t;
}

// quintic easing out - decelerating to zero velocity
float Interpolate::EaseOutQuint(float t, float b, float c, float d) {
	t /= d;
	t--;
	return c*(t*t*t*t*t + 1.f) + b;
}

float Interpolate::EaseOutQuant(float t) {
	return 1.f - std::powf(1.f - t, 5.f);
}

// quintic easing in/out - acceleration until halfway, then deceleration
float Interpolate::EaseInOutQuint(float t, float b, float c, float d) {
	t /= d/2.f;
	if (t < 1.f)
		return c/2*t*t*t*t*t + b;
	t -= 2.f;
	return c/2.f*(t*t*t*t*t + 2.f) + b;
}

float Interpolate::EaseInOutQuint(float t) {
	return t < 0.5f ?
		16.f * t * t * t * t * t :
		1.f - std::pow(-2.f * t + 2.f, 5.f) / 2.f;
}

// sinusoidal easing in - accelerating from zero velocity
float Interpolate::EaseInSine(float t, float b, float c, float d) {
	return -c * std::cos(t/d * (fPI/2.f)) + c + b;
}

float Interpolate::EaseInSine(float t) {
	return 1.f - std::cosf((t * fPI) / 2.f);
}

// sinusoidal easing out - decelerating to zero velocity
float Interpolate::EaseOutSine(float t, float b, float c, float d) {
	return c * std::sin(t/d * (fPI/2.f)) + b;
}

float Interpolate::EaseOutSine(float t) {
	return std::sinf((t * fPI) / 2.f);
}

// sinusoidal easing in/out - accelerating until halfway, then decelerating
float Interpolate::EaseInOutSine(float t, float b, float c, float d) {
	return -c/2.f * (std::cos(fPI*t/d) - 1.f) + b;
}

float Interpolate::EaseInOutSine(float t) {
	return -(std::cosf(fPI * t) - 1.f) / 2.f;
}

// exponential easing in - accelerating from zero velocity
float Interpolate::EaseInExpo(float t, float b, float c, float d) {
	return c * std::powf( 2.f, 10.f * (t/d - 1.f) ) + b;
}

float Interpolate::EaseInExpo(float t) {
	return t == 0 ? 0 : std::powf(2.f, 10.f * t - 10.f);
}

// exponential easing out - decelerating to zero velocity
float Interpolate::EaseOutExpo(float t, float b, float c, float d) {
	return c * ( -std::powf( 2.f, -10.f * t/d ) + 1.f ) + b;
}

float Interpolate::EaseOutExpo(float t) {
	return t == 1.f ?
		1.f :
		1.f - std::powf(2.f, -10.f * t);
}

// exponential easing in/out - accelerating until halfway, then decelerating
float Interpolate::EaseInOutExpo(float t, float b, float c, float d) {
	t /= d/2.f;
	if (t < 1.f)
		return c/2.f * std::powf( 2.f, 10.f * (t - 1.f) ) + b;
	t--;
	return c/2 * ( -std::powf( 2.f, -10.f * t) + 2.f ) + b;
}

float Interpolate::EaseInOutExpo(float t) {
	if (t == 0) {
		return 0;
	}
	else {
		if (t == 1) {
			return 1;
		} else {
			if (t < 0.5f) {
				return std::powf(2.f, 20.f * t - 10) / 2.f;
			}
			else {
				return (2.f - std::powf(2.f, -20.f * t + 10.f)) / 2.f;
			}
		}
	}
}

// circular easing in - accelerating from zero velocity
float Interpolate::EaseInCirc(float t, float b, float c, float d) {
	t /= d;
	return -c * (std::sqrtf(1.f - t*t) - 1.f) + b;
}

float Interpolate::EaseInCirc(float t) {
	return 1.f - std::sqrtf(1.f - std::powf(t, 2.f));
}

// circular easing out - decelerating to zero velocity
float Interpolate::EaseOutCirc(float t, float b, float c, float d) {
	t /= d;
	t--;
	return c * std::sqrtf(1.f - t*t) + b;
}

float Interpolate::EaseOutCirc(float t) {
	return std::sqrtf(1.f - std::powf(t - 1.f, 2.f));
}

// circular easing in/out - acceleration until halfway, then deceleration
float Interpolate::EaseInOutCirc(float t, float b, float c, float d) {
	t /= d/2.f;
	if (t < 1.f)
		return -c/2.f * (std::sqrtf(1.f - t*t) - 1.f) + b;
	t -= 2.f;
	return c/2.f * (std::sqrtf(1.f - t*t) + 1.f) + b;
}

float Interpolate::EaseInOutCirc(float t) {
	return t < 0.5f
		? (1.f - std::sqrtf(1.f - std::powf(2.f * t, 2.f))) / 2.f
		: (std::sqrtf(1.f - std::powf(-2.f * t + 2.f, 2.f)) + 1.f) / 2.f;
}

// ease in back - pulls back before easing in
float Interpolate::EaseInBack(float t, float b, float c, float d, float c1) {
	// tweak x for a more dramatic pull back
	t /= d; // 0-1 on curve
	float c3 = c1 + 1.f;
	float val = c3 * t * t * t - c1 * t * t; // modify t with ease in back formula
	return (c * val) + b;
}

float Interpolate::EaseInBack(float t, float c1) {
	float c3 = c1 + 1.f;
	return c3 * t * t * t - c1 * t * t;
}

// ease out back - over throws before easing out
float Interpolate::EaseOutBack(float t, float b, float c, float d, float c1) {
	// tweak c1 value for a more dramatic over throw
	t /= d; // 0-1 on curve
	float c3 = c1 + 1.f;
	float val = 1.f + c3 * std::powf(t - 1.f, 3.f) + c1 * std::powf(t - 1.f, 2.f); // apply ease in back formula
	return (c * val) + b;
}

float Interpolate::EaseOutBack(float t, float c1=1.70158f) {
	float c3 = c1 + 1;
	return 1.f + c3 * std::powf(t - 1.f, 3.f) + c1 * std::powf(t - 1.f, 2.f);
}

// ease in out back - pull back and over throw
float Interpolate::EaseInOutBack(float t, float b, float c, float d, float c1) {
	// tweak c1 value for a more dramatic pull back and over throw
	t /= d;
	float c2 = c1 * 1.525f;

	// apply pull back / over throw formula
	float val = t < 0.5f
		? (pow(2.f * t, 2.f) * ((c2 + 1.f) * 2.f * t - c2)) / 2.f
		: (pow(2.f * t - 2.f, 2.f) * ((c2 + 1.f) * (t * 2.f - 2.f) + c2) + 2.f) / 2.f;

	return (c * val) + b;
}

float Interpolate::EaseInOutBack(float t, float c1=1.70158) {
	float c2 = c1 * 1.525;

	return t < 0.5f
		? (std::powf(2.f * t, 2.f) * ((c2 + 1.f) * 2.f * t - c2)) / 2.f
		: (std::powf(2.f * t - 2.f, 2.f) * ((c2 + 1.f) * (t * 2.f - 2.f) + c2) + 2.f) / 2.f;
}

// ease in elastic
float Interpolate::EaseInElastic(float t, float b, float c, float d) {
	t /= d;
	float x = 10.75f; // magic number
	float c4 = (2.f * fPI) / 3.f;
	float val = t == 0
		? 0
		: t == 1.f
			? 1.f
			: -std::powf(2.f, 10.f * t - 10.f) * std::sinf((t * 10.f - x) * c4);

	return (c * val) + b;
}

float Interpolate::EaseInElastic(float t) {
	float c4 = (2.f * fPI) / 3.f;
	float x = 10.75f; // magic number
	return t == 0
		? 0
		: t == 1.f
			? 1.f
			: -std::powf(2.f, 10.f * t - 10.f) * std::sinf((t * 10.f - x) * c4);
}

// ease out elastic
float Interpolate::EaseOutElastic(float t, float b, float c, float d) {
	t /= d;
	float x = 0.75f; // magic number
	float c4 = (2.f * fPI) / 3.f;
	float val = t == 0
		? 0
		: t == 1.f
			? 1.f
			: std::powf(2.f, -10.f * t) * std::sinf((t * 10.f - x) * c4) + 1.f;

	return (c * val) + b;
}

float Interpolate::EaseOutElastic(float t) {
	float c4 = (2.f * fPI) / 3.f;
	float x = 0.75f; // magic number

	return t == 0
		? 0
		: t == 1.f
			? 1.f
			: std::powf(2.f, -10.f * t) * std::sinf((t * 10.f - x) * c4) + 1.f;
}

// ease in out elastic
float Interpolate::EaseInOutElastic(float t, float b, float c, float d) {
	t /= d;
	float x = 11.125f; // magic number
	float c5 = (2.f * fPI) / 4.5f;
	float val = t == 0
		? 0
		: t == 1.f
			? 1.f
			: t < 0.5f
				? -(std::powf(2.f, 20.f * t - 10.f) * std::sinf((20.f * t - x) * c5)) / 2.f
				: (std::powf(2.f, -20.f * t + 10.f) * std::sinf((20.f * t - x) * c5)) / 2.f + 1.f;

	return (c * val) + b;
}

float Interpolate::EaseInOutElastic(float t) {
	float x = 11.125f; // magic number
	float c5 = (2.f * fPI) / 4.5f;

	return t == 0
		? 0
		: t == 1.f
			? 1.f
			: t < 0.5f
				? -(std::powf(2.f, 20.f * t - 10.f) * std::sinf((20.f * t - x) * c5)) / 2.f
				: (std::powf(2.f, -20.f * t + 10.f) * std::sinf((20.f * t - x) * c5)) / 2.f + 1.f;

}

// ease out bounce
float Interpolate::EaseOutBounce(float t, float b, float c, float d) {
	t /= d;
	float n1 = 7.5625f;
	float d1 = 2.75f;
	float val = 0.f;

	if (t < 1.f / d1) {
		val = n1 * t * t;
	} else if (t < 2.f / d1) {
		t -= (1.5f / d1);
		val = n1 * t * t + 0.75f;;
	} else if (t < 2.5f / d1) {
		t -= (2.25f / d1);
		val = n1 * t * t + 0.9375f;
	} else {
		t -= (2.625f / d1);
		val = n1 * t * t + 0.984375f;
	}

	return (c * val) + b;
}

// ease out bounce - pass value between 0 and 1 (also utility for easeInBounce())
float Interpolate::EaseOutBounce(float t) {
	float n1 = 7.5625f;
	float d1 = 2.75f;
	float val = 0.f;

	if (t < 1.f / d1) {
		val = n1 * t * t;
	} else if (t < 2.f / d1) {
		t -= (1.5f / d1);
		val = n1 * t * t + 0.75f;;
	} else if (t < 2.5f / d1) {
		t -= (2.25f / d1);
		val = n1 * t * t + 0.9375f;
	} else {
		t -= (2.625f / d1);
		val = n1 * t * t + 0.984375f;
	}

	return val;
}

// ease in bounce
float Interpolate::EaseInBounce(float t, float b, float c, float d) {
	float x = t /= d;
	float val = 1.f - EaseOutBounce(1.f - x);

	return (c * val) + b;
}

// ease in bounce - pass value between 0 and 1 (utility for easeInOutBounce())
float Interpolate::EaseInBounce(float t) {
	return 1.f - EaseOutBounce(1.f - t);
}

// ease in out bounce
float Interpolate::EaseInOutBounce(float t, float b, float c, float d) {
	t /= d;
	float val = t < 0.5f
		? (1.f - EaseOutBounce(1.f - 2.f * t)) / 2.f
		: (1.f + EaseOutBounce(2.f * t - 1.f)) / 2.f;

	return (c * val) + b;
}
