//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"
#include "Engine/VertexLayout.h"
#include "TP/Chunck.h"
#include "TP/Buffer.h"
#include "TP/Camera.h"
#include "Engine/Texture.h"
#include "TP/Block.h"
#include "TP/Skybox.h"
#include "TP/World.h"
#include "TP/Light.h";
#include "Engine/BlendState.h"
#include "Engine/RenderTarget.h"
#include "TP/Player.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;
Shader* blockShader;
Shader* skyboxShader;
Shader* waterShader;
Shader* shadowShader;
Shader* depthShader;
VertexBuffer<VertexLayout_PositionColor> debugLine;

struct ModelData {
	Matrix model;
};



BlendState blendStateDefault;
BlendState blendStateWater(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);
Texture texture(L"terrain");
Texture textureSky(L"skybox");
Player player(Vector3(150,80,150));


RenderTarget targetDepth(512, 512, RenderTarget::DEPTH_ONLY);

World world;
Light light;


// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	blockShader = new Shader(L"Block");
	skyboxShader = new Shader(L"Skybox");
	waterShader = new Shader(L"Water");
	shadowShader = new Shader(L"Shadow");
	depthShader = new Shader(L"depth");
	basicShader->Create(m_deviceResources.get());
	skyboxShader->Create(m_deviceResources.get());
	waterShader->Create(m_deviceResources.get());
	shadowShader->Create(m_deviceResources.get());
	depthShader->Create(m_deviceResources.get());
	blockShader->Create(m_deviceResources.get());


	texture.Create(m_deviceResources.get());
	textureSky.Create(m_deviceResources.get());

	targetDepth.Create(m_deviceResources.get());

	auto device = m_deviceResources->GetD3DDevice();

	GenerateInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get(), blockShader);
	GenerateInputLayout<VertexLayout_PositionColor>(m_deviceResources.get(), basicShader);


	// TP: allouer vertexBuffer ici

	world.Generate(m_deviceResources.get());
	light.Generate(m_deviceResources.get());

	player.SetWorld(&world);
	//camera = PerspectiveCamera(60, (float)width / (float)height);

	blendStateDefault.Create(m_deviceResources.get());
	blendStateWater.Create(m_deviceResources.get());


	debugLine.PushVertex({ {0,0,0,1},{1,0,0,1} });
	debugLine.PushVertex({ {25,25,25,1},{0,0,0,1} });
	debugLine.Create(m_deviceResources.get());
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	
	// add kb/mouse interact here
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);

	player.Update(timer.GetElapsedSeconds(), kb, ms);
	//camera.Update(timer.GetElapsedSeconds(), kb, &m_mouse->Get(),world);
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::DeepSkyBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	ApplyInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get());

	// Render depth

	
	depthShader->Apply(m_deviceResources.get());
	targetDepth.ApplyTarget(m_deviceResources.get());
	targetDepth.Clear(m_deviceResources.get());

	player.GetCamera()->ApplyCamera(m_deviceResources.get());
	texture.Apply(m_deviceResources.get());
	light.Apply(m_deviceResources.get());
	blendStateDefault.Apply(m_deviceResources.get());

	world.Draw(m_deviceResources.get(), *(player.GetCamera()));

	//context->ClearRenderTargetView(renderTarget, Colors::DeepSkyBlue);
	//context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	// Draw Skybox
	skyboxShader->Apply(m_deviceResources.get());

	player.GetCamera()->ApplyCamera(m_deviceResources.get());
	textureSky.Apply(m_deviceResources.get());
	blendStateDefault.Apply(m_deviceResources.get());

	world.DrawSkybox(player.GetCamera()->GetPosition(), m_deviceResources.get());

	
	

	// Rendering World

	blockShader->Apply(m_deviceResources.get());
	targetDepth.ApplyShaderResourcePS(m_deviceResources.get(), 1,2);
	
	player.GetCamera()->ApplyCamera(m_deviceResources.get());
	texture.Apply(m_deviceResources.get());
	light.Apply(m_deviceResources.get());
	blendStateDefault.Apply(m_deviceResources.get());
	

	world.Draw(m_deviceResources.get(), *(player.GetCamera()));

	// Rendering Water

	waterShader->Apply(m_deviceResources.get());

	player.GetCamera()->ApplyCamera(m_deviceResources.get());
	texture.Apply(m_deviceResources.get());
	light.Apply(m_deviceResources.get());
	blendStateWater.Apply(m_deviceResources.get());

	world.DrawWater(m_deviceResources.get(), *(player.GetCamera()));


	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion
