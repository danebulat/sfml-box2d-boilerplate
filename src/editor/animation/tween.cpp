#include "editor/animation/tween.hpp"
#include "editor/animation/interpolate.hpp"

Tween::Tween()
		: m_function(InterpFunc::QuartEaseOut)
		, m_startValue(0.f)
		, m_targetValue(0.f)
		, m_changeValue(0.f)
		, m_duration(0.f)
		, m_elapsedTime(0.f)
		, m_isAnimating(false) {
	m_property = nullptr;
}

// Custom constructor to initialise data members with custom values.
Tween::Tween(float* property, float startValue, float targetValue, float duration,
			 InterpFunc function)
		: m_function(function)
		, m_startValue(startValue)
		, m_targetValue(targetValue)
		, m_changeValue(targetValue-startValue)
		, m_duration(duration)
		, m_elapsedTime(0.f)
		, m_isAnimating(false) {
	m_property = property;
}

Tween::~Tween() {
	// The animated property pointer is pointing to a
	// stack variable or dynamically allocated data that
	// will be cleaned up in it's respectable class.
}

void Tween::ResetAndStop() {
	m_elapsedTime = 0.f;
	(*m_property) = m_startValue;
	m_isAnimating = false;
}

void Tween::ResetAndPlay() {
	m_elapsedTime = 0.f;
	(*m_property) = m_startValue;
	m_isAnimating = true;
}

void Tween::Start() {
	m_isAnimating = true;
}

void Tween::Stop() {
	m_isAnimating = false;
}

bool Tween::IsAnimating() const {
	return m_isAnimating;
}

void Tween::Update(float dt) {
	// Update the tween if it's animating
	if (m_isAnimating) {

		// Update the elapsed time with delta-time
		m_elapsedTime += dt;

		// Stop the tween if it's ran its duration
		if (m_elapsedTime >= m_duration) {
			(*m_property) = m_targetValue;
			m_elapsedTime = 0.f;
			m_isAnimating = false;
			return;
		}

		// Otherwise, continue the animation
		float t = m_elapsedTime;
		float b = m_startValue;
		float c = m_changeValue;
		float d = m_duration;

		switch (m_function) {
		case InterpFunc::Linear:
			(*m_property) = Interpolate::Linear(t, b, c, d);
			break;

		case InterpFunc::QuadEaseIn:
			(*m_property) = Interpolate::EaseInQuad(t, b, c, d);
			break;

		case InterpFunc::QuadEaseOut:
			(*m_property) = Interpolate::EaseOutQuad(t, b, c, d);
			break;

		case InterpFunc::QuadEaseInOut:
			(*m_property) = Interpolate::EaseInOutQuad(t, b, c, d);
			break;

		case InterpFunc::CubicEaseIn:
			(*m_property) = Interpolate::EaseInCubic(t, b, c, d);
			break;

		case InterpFunc::CubicEaseOut:
			(*m_property) = Interpolate::EaseOutCubic(t, b, c, d);
			break;

		case InterpFunc::CubicEaseInOut:
			(*m_property) = Interpolate::EaseInOutCubic(t, b, c, d);
			break;

		case InterpFunc::QuartEaseIn:
			(*m_property) = Interpolate::EaseInQuart(t, b, c, d);
			break;

		case InterpFunc::QuartEaseOut:
			(*m_property) = Interpolate::EaseOutQuart(t, b, c, d);
			break;

		case InterpFunc::QuartEaseInOut:
			(*m_property) = Interpolate::EaseInOutQuart(t, b, c, d);
			break;

		case InterpFunc::QuintEaseIn:
			(*m_property) = Interpolate::EaseInQuint(t, b, c, d);
			break;

		case InterpFunc::QuintEaseOut:
			(*m_property) = Interpolate::EaseOutQuint(t, b, c, d);
			break;

		case InterpFunc::QuintEaseInOut:
			(*m_property) = Interpolate::EaseInOutQuint(t, b, c, d);
			break;

		case InterpFunc::SineEaseIn:
			(*m_property) = Interpolate::EaseInSine(t, b, c, d);
			break;

		case InterpFunc::SineEaseOut:
			(*m_property) = Interpolate::EaseOutSine(t, b, c, d);
			break;

		case InterpFunc::SineEaseInOut:
			(*m_property) = Interpolate::EaseInOutSine(t, b, c, d);
			break;

		case InterpFunc::ExpoEaseIn:
			(*m_property) = Interpolate::EaseInExpo(t, b, c, d);
			break;

		case InterpFunc::ExpoEaseOut:
			(*m_property) = Interpolate::EaseOutExpo(t, b, c, d);
			break;

		case InterpFunc::ExpoEaseInOut:
			(*m_property) = Interpolate::EaseInOutExpo(t, b, c, d);
			break;

		case InterpFunc::CircEaseIn:
			(*m_property) = Interpolate::EaseInCirc(t, b, c, d);
			break;

		case InterpFunc::CircEaseOut:
			(*m_property) = Interpolate::EaseOutCirc(t, b, c, d);
			break;

		case InterpFunc::CircEaseInOut:
			(*m_property) = Interpolate::EaseInOutCirc(t, b, c, d);
			break;

		case InterpFunc::BackEaseIn:
			(*m_property) = Interpolate::EaseInBack(t, b, c, d);
			break;

		case InterpFunc::BackEaseOut:
			(*m_property) = Interpolate::EaseOutBack(t, b, c, d);
			break;

		case InterpFunc::BackEaseInOut:
			(*m_property) = Interpolate::EaseInOutBack(t, b, c, d);
			break;

		case InterpFunc::ElasticEaseIn:
			(*m_property) = Interpolate::EaseInElastic(t, b, c, d);
			break;

		case InterpFunc::ElasticEaseOut:
			(*m_property) = Interpolate::EaseOutElastic(t, b, c, d);
			break;

		case InterpFunc::ElasticEaseInOut:
			(*m_property) = Interpolate::EaseInOutElastic(t, b, c, d);
			break;

		case InterpFunc::BounceEaseIn:
			(*m_property) = Interpolate::EaseInBounce(t, b, c, d);
			break;

		case InterpFunc::BounceEaseOut:
			(*m_property) = Interpolate::EaseOutBounce(t, b, c, d);
			break;

		case InterpFunc::BounceEaseInOut:
			(*m_property) = Interpolate::EaseInOutBounce(t, b, c, d);
			break;

		default:
			(*m_property) = Interpolate::EaseOutQuart(t, b, c, d);
			break;
		}
	}
}
