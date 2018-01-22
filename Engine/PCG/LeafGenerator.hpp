#pragma once
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\FloatRange.hpp"
#include "Engine\Math\IntVector2.hpp"


class LeafGenerator
{
public:
	LeafGenerator(const Vector3& position, const Vector3& orientation, float maxRadius, int minLeaves, int maxLeaves, float xSizeMin, float xSizeMax, float ySizeMin, float ySizeMax);

	Vector3 m_position;
	Vector3 m_orientation;
	float m_maxRadius;
	IntVector2 m_leafCount;
	FloatRange m_xSizeRange;
	FloatRange m_ySizeRange;
	FloatRange m_leafNormalOffset;
};