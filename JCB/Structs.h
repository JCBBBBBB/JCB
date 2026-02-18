#pragma once
#include "Types.h"

struct VertexTexture
{
	Vec3 position = { 0.f,0.f,0.f };
	Vec2 uv = { 0.f,0.f };
};

struct VertexColor
{
	Vec3 position = { 0.f,0.f,0.f };
	Color color = { 0.f,0.f,0.f,0.f };
};