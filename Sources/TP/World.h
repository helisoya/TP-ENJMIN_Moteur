#pragma once

#include "TP/Block.h"
#include "TP/Buffer.h"
#include "TP/Chunck.h"
#include "TP/Skybox.h"
#include "PerlinNoise.hpp"


class Camera;
#define MAP_SIZE 20

using namespace DirectX;
using namespace DirectX::SimpleMath;


class World {

	struct ModelData {
		Matrix model;
	};

public:

private:
	std::vector<Chunck> chuncks;
	Skybox skybox;

	ConstantBuffer<ModelData> constantBufferModel;

	DeviceResources* deviceRes;

public:

	World();

	void Generate(DeviceResources* deviceRes);

	void Draw(DeviceResources* deviceRes, Camera& camera);
	void DrawSkybox(Vector3 pos, DeviceResources* deviceRes);
	void DrawWater(DeviceResources* deviceRes, Camera& camera);

	void SearchForBlock(Vector3 pos, Vector3 forward,BlockId replaceWith);
	std::vector<std::array<int, 3>> Raycast(Vector3 pos, Vector3 dir, float maxDist);

	BlockId GetCube(int gx, int gy, int gz);

private:

	Vector2 To2D(int x);
	int To1D(int x, int y);

	void GenerateChunck(Vector3 pos, siv::BasicPerlinNoise<float> &noise, siv::BasicPerlinNoise<float>& noiseCave, DeviceResources* deviceRes);
	void GenerateMesh(DeviceResources* deviceRes);

	
};