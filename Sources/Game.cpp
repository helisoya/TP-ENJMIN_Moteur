//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;

ComPtr<ID3D11Buffer> vertexBuffer;
ComPtr<ID3D11Buffer> indexBuffer;
ComPtr<ID3D11Buffer> constantBuffer;
ComPtr<ID3D11InputLayout> inputLayout;

struct MatrixData {
	Matrix mModel;
	Matrix mView;
	Matrix mProjection;
};

MatrixData matrixData;


// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
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

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());

	auto device = m_deviceResources->GetD3DDevice();


	const std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDescs = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	device->CreateInputLayout(
		InputElementDescs.data(), InputElementDescs.size(),
		basicShader->vsBytecode.data(), basicShader->vsBytecode.size(),
		inputLayout.ReleaseAndGetAddressOf());

	// TP: allouer vertexBuffer ici

	std::vector<float> position = {
		-0.5f,0.5f, 0.0f, 1.0f,0.0f,0.0f, 1.0f,
		0.5f,0.5f, 0.0f, 1.0f,1.0f,1.0f, 1.0f,
		0.5f,-0.5f, 0.0f, 0.0f,0.0f,1.0f, 1.0f,
		-0.5f,-0.5f, 0.0f, 1.0f,1.0f,1.0f, 1.0f
	};

	std::vector<UINT> indexData = {
		0,1,2,
		2,3,0
	};

	matrixData = {};
	matrixData.mModel = Matrix().Transpose();
	matrixData.mView = Matrix::CreateLookAt(Vector3(0,0,5), Vector3::Zero, Vector3::Up).Transpose();
	matrixData.mProjection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(60),1,0.1f, 1000.0f).Transpose();



	// Vertex Buffer
	CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
		sizeof(float) * position.size(),
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = position.data();


	device->CreateBuffer(
		&desc,
		&data,
		vertexBuffer.ReleaseAndGetAddressOf()
	);

	// Constant Buffer
	CD3D11_BUFFER_DESC descConstant = CD3D11_BUFFER_DESC(
		sizeof(MatrixData),
		D3D11_BIND_CONSTANT_BUFFER
	);

	D3D11_SUBRESOURCE_DATA dataConstant = {};
	dataConstant.pSysMem = &matrixData;


	device->CreateBuffer(
		&descConstant,
		&dataConstant,
		constantBuffer.ReleaseAndGetAddressOf()
	);

	auto context = m_deviceResources->GetD3DDeviceContext();
	ID3D11Buffer* cbs[] = { constantBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cbs);

	// Index Buffer
	
	CD3D11_BUFFER_DESC descIdx = CD3D11_BUFFER_DESC(
		sizeof(UINT) * indexData.size(),
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA dataIdx = {};
	dataIdx.pSysMem = indexData.data();


	device->CreateBuffer(
		&descIdx,
		&dataIdx,
		indexBuffer.ReleaseAndGetAddressOf()
	);

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

	Vector3 newPos = Vector3(std::cosf(timer.GetTotalSeconds() * 5), std::sinf(timer.GetTotalSeconds() * 5), 5);
	matrixData.mView = Matrix::CreateLookAt(newPos, newPos + Vector3::Forward, Vector3::Up).Transpose();

	matrixData.mModel = matrixData.mModel.CreateRotationX(XMConvertToRadians(timer.GetTotalSeconds() * 200)).Transpose();

	//matrixData.mView = matrixData.mView.CreateRotationX(XMConvertToRadians(timer.GetTotalSeconds()*50));
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

	context->ClearRenderTargetView(renderTarget, Color(0,1,0));
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	basicShader->Apply(m_deviceResources.get());

	// TP: Tracer votre vertex buffer ici

	context->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &matrixData, 0, 0);

	ID3D11Buffer* vbs[] = { vertexBuffer.Get()};
	const UINT strides[] = { sizeof(float) * 7 };
	const UINT offsets[] = { 0 };

	context->IASetVertexBuffers(
		0,
		1,
		vbs,
		strides,
		offsets
	);

	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT,0 );

	context->DrawIndexed(6, 0,0);

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
