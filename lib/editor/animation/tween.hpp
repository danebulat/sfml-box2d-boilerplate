#ifndef TWEEN_HPP
#define TWEEN_HPP

enum class InterpFunc {
	Linear = 1,

	QuadEaseIn = 2,
	QuadEaseOut = 3,
	QuadEaseInOut = 4,

	CubicEaseIn = 5,
	CubicEaseOut = 6,
	CubicEaseInOut = 7,

	QuartEaseIn = 8,
	QuartEaseOut = 9,
	QuartEaseInOut = 10,

	QuintEaseIn = 11,
	QuintEaseOut = 12,
	QuintEaseInOut = 13,

	SineEaseIn = 14,
	SineEaseOut = 15,
	SineEaseInOut = 16,

	ExpoEaseIn = 17,
	ExpoEaseOut = 18,
	ExpoEaseInOut = 19,

	CircEaseIn = 20,
	CircEaseOut = 21,
	CircEaseInOut = 22,

	BackEaseIn = 23,
	BackEaseOut = 24,
	BackEaseInOut = 25,

	ElasticEaseIn = 26,
	ElasticEaseOut = 27,
	ElasticEaseInOut = 28,

	BounceEaseIn = 29,
	BounceEaseOut = 30,
	BounceEaseInOut = 31
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