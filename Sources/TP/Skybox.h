#pragma once

#include "Engine/VertexLayout.h"
#include "TP/Buffer.h"

class Skybox {
public:

private:
	VertexBuffer<VertexLayout_PositionNormalUV> vertexBuffer;
	IndexBuffer indexBuffer;

public:

	Skybox();

	void Generate(DeviceResources* deviceResources);

	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx);

	void Draw(DeviceResources* deviceResources);

private:

	Vector4 ToVec4(const Vector3& v);
};