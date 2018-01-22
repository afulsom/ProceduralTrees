#pragma once
#include "Engine/Audio/Audio.hpp"
#include "Game/Camera3D.hpp"
#include "Engine/PCG/FoliageGenerator.hpp"

class Foliage;

class Game
{

public:
	Camera3D m_theCamera;
	FoliageGenerator* m_testGen;
	Foliage* m_testFoliage;
	Foliage* m_testFoliage2;
	Foliage* m_testFoliage3;
	std::vector<Vertex> m_terrain;
	Texture2D* m_terrainALB;
	Texture2D* m_terrainNRM;
	Texture2D* m_terrainSPC;
	Texture2D* m_testTexture;
	Texture2D* m_defaultNormal;

	ShaderProgram* m_unlitShader;
	ShaderProgram* m_litShader;
	ShaderProgram* m_leafShader;

	bool m_shouldRenderDebug = false;

	Game();
	~Game();

	void Initialize();

	void Update(float deltaSeconds);
	void Render() const;


private:
	bool m_isGamePaused;
	void HandleInput(float deltaSeconds);
};