#pragma once

class Circle
{
public:
	Circle(const float radius, const Vec2& center, const Vec4& color)
		:_radius(radius), _center(center), _color(color)
	{

	}

	bool isInside(int i, int j);
public:
	float _radius;
	Vec2 _center;
	Vec4 _color;
};


