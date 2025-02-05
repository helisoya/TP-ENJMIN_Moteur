#pragma once

#include "TP/Block.h"
#include "TP/Buffer.h"
#include "TP/Chunck.h"
#include "TP/Skybox.h"

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

public:

	World();

	void Generate(DeviceResources* deviceRes);

	void Draw(DeviceResources* deviceRes);
	void DrawSkybox(Vector3 pos, DeviceResources* deviceRes);

private:
};