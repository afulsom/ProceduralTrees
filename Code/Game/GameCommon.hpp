#pragma once
#include "Engine/Renderer/RHI/SimpleRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Core/ConsoleSystem.hpp"
#include "Engine/Math/Random.hpp"

class App;

extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern App* g_theApp;

constexpr unsigned int WINDOW_DEFAULT_RESOLUTION_X = 1280;
constexpr unsigned int WINDOW_DEFAULT_RESOLUTION_Y = 720;

extern float ORTHO_X_DIMENSION;
extern float ORTHO_Y_DIMENSION;