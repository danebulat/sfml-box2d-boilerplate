#ifndef TWEEN_HPP
#define TWEEN_HPP

enum class InterpFunc {
	Linear = 0,

	QuadEaseIn = 1,
	QuadEaseOut = 2,
	QuadEaseInOut = 3,

	CubicEaseIn = 4,
	CubicEaseOut = 5,
	CubicEaseInOut = 6,

	QuartEaseIn = 7,
	QuartEaseOut = 8,
	QuartEaseInOut = 9,

	QuintEaseIn = 10,
	QuintEaseOut = 11,
	QuintEaseInOut = 12,

	SineEaseIn = 13,
	SineEaseOut = 14,
	SineEaseInOut = 15,

	ExpoEaseIn = 16,
	ExpoEaseOut = 17,
	ExpoEaseInOut = 18,

	CircEaseIn = 19,
	CircEaseOut = 20,
	CircEaseInOut = 21,

	BackEaseIn = 22,
	BackEaseOut = 23,
	BackEaseInOut = 24,

	ElasticEaseIn = 25,
	ElasticEaseOut = 26,
	ElasticEaseInOut = 27,

	BounceEaseIn = 28,
	BounceEaseOut = 29,
	BounceEaseInOut = 30
};

class Tween {
	// Reference to the property being animated
	float* m_property;

	InterpFunc m_function;

    float m_startValue;
    float m_targetValue;
	float m_changeValue;
    float m_duration;
    float m_elapsedTime;
    bool  m_isAnimating;

public:
    // Default constructor where members should be initialised
	// manually after instantiation.
    Tween();
	~Tween();

	// Custom constructor to initialise data members with custom values.
    Tween(float* property,
		  float startValue,
		  float targetValue,
		  float duration,
		  InterpFunc function=InterpFunc::BounceEaseOut);

	// Disable copy constructor and assignment operator
	Tween& operator= (const Tween&) = delete;
	Tween(const Tween&) = delete;

    // API called from parent class
    void ResetAndStop();
	void ResetAndPlay();
	void Start();
	void Stop();
	bool IsAnimating() const;
	void Update(float dt);
};

#endif