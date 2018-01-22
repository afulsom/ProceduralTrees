#include "Engine/PCG/LeafGenerator.hpp"



LeafGenerator::LeafGenerator(const Vector3& position, const Vector3& orientation, float maxRadius, int minLeaves, int maxLeaves, float xSizeMin, float xSizeMax, float ySizeMin, float ySizeMax)
	: m_position(position)
	, m_orientation(orientation)
	, m_maxRadius(maxRadius)
	, m_leafCount(minLeaves, maxLeaves)
	, m_xSizeRange(xSizeMin, xSizeMax)
	, m_ySizeRange(ySizeMin, ySizeMax)
	, m_leafNormalOffset(-2.f, 2.f)
{

}

