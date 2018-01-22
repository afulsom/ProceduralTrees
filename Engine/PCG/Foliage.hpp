#pragma once
#include <vector>
#include "Engine\Math\Vector3.hpp"
#include "Engine\Renderer\RHI\MeshBuilder.hpp"
#include "Engine\Core\Rgba.hpp"
#include "Engine\PCG\LeafGenerator.hpp"



class Foliage
{
public:
	Foliage();

	void AddLeafVerts();

	std::vector<Vector3> m_positions;
	std::vector<Rgba> m_colors;

	std::vector<LeafGenerator> m_leafGenerators;

	MeshBuilder m_treeMesh;
	MeshBuilder m_leafMesh;

	Texture2D* m_barkAlbedo;
	Texture2D* m_barkNormal;
	Texture2D* m_barkSpecular;

	Texture2D* m_leafTop;
	Texture2D* m_leafBottom;
private:
	void GenerateLeavesForGenerator(const LeafGenerator& generator);
};