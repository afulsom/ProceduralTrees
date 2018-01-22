#pragma once
#include "Engine\Core\LSystem.hpp"
#include "Engine\Math\FloatRange.hpp"
#include "Engine\PCG\Foliage.hpp"


class Texture2D;

struct TreeParameters
{
	TreeParameters(float minYaw, float maxYaw, float minPitch, float maxPitch, float minLength, float maxLength, float minWidth, float maxWidth)
		: m_yaw(minYaw, maxYaw)
		, m_pitch(minPitch, maxPitch)
		, m_length(minLength, maxLength)
		, m_width(minWidth, maxWidth)
	{

	}

	FloatRange m_yaw;
	FloatRange m_pitch;
	FloatRange m_length;
	FloatRange m_width;
};

class FoliageGenerator
{
public:
	FoliageGenerator(unsigned int systemIterations, const TreeParameters& trunkParameters, const TreeParameters& limbParameters, const TreeParameters& branchParameters, const FloatRange& xSizeRange, const FloatRange& ySizeRange, Texture2D* barkAlbedo = nullptr, Texture2D* barkNormal = nullptr, Texture2D* barkSpecular = nullptr, Texture2D* leafTop = nullptr, Texture2D* leafBottom = nullptr);

	Foliage* GenerateFoliage();

	void AddSystem(LSystem system);

	std::vector<LSystem> m_systems;
	unsigned int m_systemIterations;
	TreeParameters m_trunkParameters;
	TreeParameters m_limbParameters;
	TreeParameters m_branchParameters;

	FloatRange m_leafXSizeRange;
	FloatRange m_leafYSizeRange;

	unsigned int m_numSlices;
	Texture2D* m_barkAlbedo;
	Texture2D* m_barkNormal;
	Texture2D* m_barkSpecular;

	Texture2D* m_leafTop;
	Texture2D* m_leaftBottom;
};