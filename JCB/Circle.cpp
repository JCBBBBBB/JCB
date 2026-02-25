#include "pch.h"
#include "Circle.h"

// TODO: 여기에 원의 방정식을 이용해서 x가 원 안에 들어 있는지 아닌지에 따라
			//		true나 false 반환
bool Circle::isInside(int i , int j)
{
	const Vec2 temp = Vec2(i, j);
	//
	auto distance = XMVectorGetX(XMVector3Length(temp - _center));

	if (distance <= _radius)
	{
		return true;
	}

	return false;
}
