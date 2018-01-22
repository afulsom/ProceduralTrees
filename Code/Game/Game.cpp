#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/ConfigSystem.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Game/App.hpp"
#include "Engine/Core/ConsoleSystem.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/UI/UIElement.hpp"
#include "Engine/Core/LSystem.hpp"
#include "Engine/PCG/FoliageGenerator.hpp"


Game::Game()
	: m_isGamePaused(false)
	, m_testFoliage(false)
	, m_theCamera()
	, m_testTexture(nullptr)
{
	m_litShader = g_theRenderer->CreateShaderFromHLSLFile("Data/HLSL/lit.hlsl");
	m_leafShader = g_theRenderer->CreateShaderFromHLSLFile("Data/HLSL/leaf.hlsl");
	m_unlitShader = g_theRenderer->CreateShaderFromHLSLFile("Data/HLSL/unlit.hlsl");

	m_terrain = g_theRenderer->CreateSurfacePatch(Vector2(-300.f, -300.f), Vector2(300.f, 300.f), 5.f, 500, 20, Rgba(135, 135, 135), 0.f, 50.f, 3);
	m_terrainALB = g_theRenderer->CreateTexture2DFromFile("Data/Images/grass_diffuse.png");
	m_terrainNRM = g_theRenderer->CreateTexture2DFromFile("Data/Images/grass_normal.png");
	m_terrainSPC = g_theRenderer->CreateTexture2DFromFile("Data/Images/black.png");
	m_defaultNormal = g_theRenderer->CreateTexture2DFromFile("Data/Images/blue.png");

	Texture2D* barkALB = g_theRenderer->CreateTexture2DFromFile("Data/Images/Pine_ALB.png");
	Texture2D* barkNRM = g_theRenderer->CreateTexture2DFromFile("Data/Images/Pine_NRM.png");
	Texture2D* barkSPC = g_theRenderer->CreateTexture2DFromFile("Data/Images/Pine_SPC.png");
	Texture2D* leafBottom = g_theRenderer->CreateTexture2DFromFile("Data/Images/Oak_Green_Bottom.png");
	Texture2D* leafTop = g_theRenderer->CreateTexture2DFromFile("Data/Images/Oak_Green_Top.png");

	LSystem trunkTest;
	trunkTest.m_axiom = "X";
	trunkTest.m_rules.insert(std::pair<char, Rule>('X', Rule("+T[X]T[-X]", 2)));
	trunkTest.m_rules.insert(std::pair<char, Rule>('X', Rule("-T[X]T[+X]", 2)));
	trunkTest.m_rules.insert(std::pair<char, Rule>('T', Rule("TT", 1)));
	trunkTest.m_numIterations = 1;

	LSystem limbTest;
	limbTest.m_rules.insert(std::pair<char, Rule>('X', Rule("L[>C][<X]L[>X][<C]>LX", 3)));
	limbTest.m_rules.insert(std::pair<char, Rule>('X', Rule("L[<C][X]L[>C][X]>LX", 3)));
	limbTest.m_rules.insert(std::pair<char, Rule>('L', Rule("LL", 3)));
	limbTest.m_numIterations = 2;

	LSystem branchTest;
	branchTest.m_rules.insert(std::pair<char, Rule>('X', Rule("B[(C]B[)C])BC", 3)));
	branchTest.m_rules.insert(std::pair<char, Rule>('X', Rule("B[C]B[C])BC", 3)));
	branchTest.m_rules.insert(std::pair<char, Rule>('C', Rule("B[(C]B[)C])BC", 3)));
	branchTest.m_rules.insert(std::pair<char, Rule>('C', Rule("B[C]B[C])BC", 3)));
	branchTest.m_numIterations = 3;

	TreeParameters trunkParams(-20.f, 20.f, -20.f, 30.f, 7.5f, 15.f, 3.f, 3.25f);
	TreeParameters limbParams(-45.f, 45.f, -50.f, 50.f, 7.f, 10.f, 2.f, 2.1f);
	TreeParameters branchParams(-45.f, 45.f, -20.f, 30.f, 4.f, 6.f, 0.25f, 0.5f);

	m_testGen = new FoliageGenerator(3, trunkParams, limbParams, branchParams, FloatRange(1.5f, 2.5f), FloatRange(3.25f, 4.f), barkALB, barkNRM, barkSPC, leafTop, leafBottom);
	m_testGen->m_numSlices = 12;
	m_testGen->AddSystem(trunkTest);
	m_testGen->AddSystem(limbTest);
	m_testGen->AddSystem(branchTest);

	m_testFoliage = m_testGen->GenerateFoliage();
	m_testFoliage2 = m_testGen->GenerateFoliage();
	m_testFoliage3 = m_testGen->GenerateFoliage();

	m_testTexture = g_theRenderer->CreateTexture2DFromFile("Data/Images/Test_StbiAndDirectX.png");
}


Game::~Game()
{
	delete m_testTexture;

	delete m_testFoliage->m_barkAlbedo;
	delete m_testFoliage->m_barkNormal;
	delete m_testFoliage->m_barkSpecular;
	delete m_testFoliage->m_leafBottom;
	delete m_testFoliage->m_leafTop;
	delete m_terrainALB;
	delete m_terrainNRM;
	delete m_terrainSPC;
	delete m_defaultNormal;

	delete m_leafShader;
	delete m_litShader;
	delete m_unlitShader;
}


void Game::Initialize()
{

}

void Game::Update(float deltaSeconds)
{
	HandleInput(deltaSeconds);
	g_theRenderer->SetEyePosition(Vector4(m_theCamera.m_position.x, m_theCamera.m_position.y, m_theCamera.m_position.z, 0.f));
	g_theRenderer->EnablePointLight(POINT_LIGHT_ONE, Vector3(0.f, 5.f, -15.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));
	g_theRenderer->EnablePointLight(POINT_LIGHT_TWO, Vector3(0.f, 5.f, 15.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));

	g_theRenderer->EnablePointLight(POINT_LIGHT_THREE, Vector3(100.f, 5.f, -15.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));
	g_theRenderer->EnablePointLight(POINT_LIGHT_FOUR, Vector3(100.f, 5.f, 15.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));

	g_theRenderer->EnablePointLight(POINT_LIGHT_FIVE, Vector3(-100.f, 5.f, -15.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));
	g_theRenderer->EnablePointLight(POINT_LIGHT_SIX, Vector3(-100.f, 5.f, 15.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));

	g_theRenderer->EnablePointLight(POINT_LIGHT_ZERO, Vector3(-100.f, 150.f, 0.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));
	g_theRenderer->EnablePointLight(POINT_LIGHT_SEVEN, Vector3(100.f, 150.f, 0.f), Rgba(179, 164, 90), 8.f, Vector3(1.f, 0.25f, 0.f), Vector3(1.f, 0.25f, 0.1f));
}

void Game::Render() const
{
// 	g_theRenderer->SetShader(m_unlitShader);
// 	g_theRenderer->SetTexture(nullptr);
// 	g_theRenderer->SetTexture(1, m_defaultNormal);
// 	g_theRenderer->SetTexture(2, m_terrainSPC);
// 
// 	g_theRenderer->DrawVertices(m_terrain.data(), m_terrain.size());

	g_theRenderer->SetShader(m_litShader);

	g_theRenderer->SetTexture(m_testFoliage->m_barkAlbedo);
	g_theRenderer->SetTexture(1, m_testFoliage->m_barkNormal);
	g_theRenderer->SetTexture(2, m_testFoliage->m_barkSpecular);

	g_theRenderer->SetModelMatrix(Matrix4::CreateTranslation(Vector3(0.f, 0.f, 0.f)));
	g_theRenderer->DrawVertices(m_testFoliage->m_treeMesh.m_verts.data(), m_testFoliage->m_treeMesh.m_verts.size());

	g_theRenderer->SetModelMatrix(Matrix4::CreateTranslation(Vector3(100.f, 0.f, 0.f)));
	g_theRenderer->DrawVertices(m_testFoliage2->m_treeMesh.m_verts.data(), m_testFoliage2->m_treeMesh.m_verts.size());

	g_theRenderer->SetModelMatrix(Matrix4::CreateTranslation(Vector3(-100.f, 0.f, 0.f)));
	g_theRenderer->DrawVertices(m_testFoliage3->m_treeMesh.m_verts.data(), m_testFoliage3->m_treeMesh.m_verts.size());

	g_theRenderer->SetShader(m_leafShader);
	g_theRenderer->SetTexture(m_testFoliage->m_leafTop);
	g_theRenderer->SetTexture(1, m_defaultNormal);
	g_theRenderer->SetTexture(2, m_terrainSPC);

	g_theRenderer->SetModelMatrix(Matrix4::CreateTranslation(Vector3(0.f, 0.f, 0.f)));
	g_theRenderer->DrawVertices(m_testFoliage->m_leafMesh.m_verts.data(), m_testFoliage->m_leafMesh.m_verts.size());

	g_theRenderer->SetModelMatrix(Matrix4::CreateTranslation(Vector3(100.f, 0.f, 0.f)));
	g_theRenderer->DrawVertices(m_testFoliage2->m_leafMesh.m_verts.data(), m_testFoliage2->m_leafMesh.m_verts.size());

	g_theRenderer->SetModelMatrix(Matrix4::CreateTranslation(Vector3(-100.f, 0.f, 0.f)));
	g_theRenderer->DrawVertices(m_testFoliage3->m_leafMesh.m_verts.data(), m_testFoliage3->m_leafMesh.m_verts.size());

	g_theRenderer->EnableDepthTest(false);
	g_theRenderer->EnableDepthWrite(false);
	g_theRenderer->SetTexture(nullptr);
	g_theRenderer->SetTexture(1, (Texture2D*)nullptr);
	g_theRenderer->SetTexture(1, m_terrainSPC);

// 	g_theRenderer->DrawRectangularPrism3D(Vector3(9.5f, 4.5f, -0.5f), Vector3(10.5f, 5.5f, 0.5f), Rgba::WHITE);

	if(m_shouldRenderDebug)
	{
		g_theRenderer->DrawRectangularPrism3D(m_testFoliage->m_positions[0] - Vector3::ONE, m_testFoliage->m_positions[0] + Vector3::ONE, Rgba::RED);
		for (size_t positionIndex = 0; positionIndex < m_testFoliage->m_positions.size(); positionIndex += 2)
		{
// 			g_theRenderer->DrawLine3D(m_testFoliage->m_positions[positionIndex], m_testFoliage->m_positions[positionIndex + 1], m_testFoliage->m_colors[positionIndex + 1]);
			g_theRenderer->DrawRectangularPrism3D(m_testFoliage->m_positions[positionIndex + 1] - Vector3::ONE, m_testFoliage->m_positions[positionIndex + 1] + Vector3::ONE, m_testFoliage->m_colors[positionIndex + 1]);
		}
	}


}

void Game::HandleInput(float deltaSeconds)
{
	if (g_theApp->HasFocus())
	{
		IntVector2 mouseScreenPos = g_theInput->GetCursorScreenPos();
		IntVector2 screenCenter = g_theInput->GetScreenCenter();

		IntVector2 mouseDeltaMove = mouseScreenPos - screenCenter;

		g_theInput->SetCursorScreenPos(screenCenter);

		float mouseMovedX = (float)mouseDeltaMove.x;
		float mouseMovedY = (float)mouseDeltaMove.y;
		float mouseSensitivityY = 0.03f;
		float mouseSensitivityX = 0.03f;
		m_theCamera.m_yaw += mouseMovedX * mouseSensitivityX;
		m_theCamera.m_pitch += mouseMovedY * mouseSensitivityY;

		m_theCamera.m_pitch = ClampFloat(m_theCamera.m_pitch, -89.99f, 89.99f);
	}

	if (g_theInput->WasKeyJustPressed('R'))
	{
		delete m_testFoliage;
		delete m_testFoliage2;
		delete m_testFoliage3;
		m_testFoliage = m_testGen->GenerateFoliage();
		m_testFoliage2 = m_testGen->GenerateFoliage();
		m_testFoliage3 = m_testGen->GenerateFoliage();
	}

	if (g_theInput->WasKeyJustPressed('E'))
	{
		m_shouldRenderDebug = !m_shouldRenderDebug;
	}

	float playerSpeed = 10.f;
	if (g_theInput->IsKeyDown(KEYCODE_SHIFT))
	{
		playerSpeed *= 10.f;
	}

	//Forward Back
	if (g_theInput->IsKeyDown('W'))
	{
		m_theCamera.m_position += m_theCamera.GetForwardXZ() * playerSpeed * deltaSeconds;
	}
	else if (g_theInput->IsKeyDown('S'))
	{
		m_theCamera.m_position -= m_theCamera.GetForwardXZ() * playerSpeed * deltaSeconds;
	}

	//Left Right
	if (g_theInput->IsKeyDown('A'))
	{
		m_theCamera.m_position += m_theCamera.GetLeftXZ() * playerSpeed * deltaSeconds;
	}
	else if (g_theInput->IsKeyDown('D'))
	{
		m_theCamera.m_position -= m_theCamera.GetLeftXZ() * playerSpeed * deltaSeconds;
	}

	//Up Down
	if (g_theInput->IsKeyDown(KEYCODE_SPACE))
	{
		m_theCamera.m_position += Vector3::Y_AXIS * playerSpeed * deltaSeconds;
	}
	else if (g_theInput->IsKeyDown(KEYCODE_CTRL))
	{
		m_theCamera.m_position -= Vector3::Y_AXIS * playerSpeed * deltaSeconds;
	}

	if (g_theInput->IsKeyDown('E'))
	{
		m_theCamera.m_yaw += 90.f * deltaSeconds;
	}
	else if (g_theInput->IsKeyDown('Q'))
	{
		m_theCamera.m_yaw -= 90.f * deltaSeconds;
	}

	if (g_theInput->IsKeyDown('O'))
	{
		m_theCamera.m_position = Vector3::ZERO;
		m_theCamera.m_yaw = 0.f;
		m_theCamera.m_pitch = 0.f;
		m_theCamera.m_roll = 0.f;
	}

}
