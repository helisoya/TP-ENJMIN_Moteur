#pragma once

#include "SimpleMath.h"
#include "Engine/VertexLayout.h"
#include "Buffer.h"
#include "TP/Block.h"

using namespace DirectX::SimpleMath;

class Cube
{
public:

private:
	VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
	IndexBuffer indexBuffer;

	Matrix model;
	BlockData blockData;

public:

	Cube(Vector3 pos, BlockData& data);

	void Generate(DeviceResources* deviceResources);

	Matrix GetModel();
	void SetModel(Matrix& matrix);

	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx);

	void Draw(DeviceResources* deviceResources);

private:
};

