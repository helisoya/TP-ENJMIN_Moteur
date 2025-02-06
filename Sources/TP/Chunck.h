#pragma once

#include "SimpleMath.h"
#include "Engine/VertexLayout.h"
#include "Buffer.h"
#include "TP/Block.h"

#define CHUNCK_SIZE 16
#define CHUNCK_HEIGHT 64

using namespace DirectX::SimpleMath;

class Chunck
{
public:

private:
	int id;
	VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
	IndexBuffer indexBuffer;

	Matrix model;
	
	BlockId mapIds[CHUNCK_SIZE * CHUNCK_SIZE * CHUNCK_HEIGHT];

public:

	Chunck(Vector3 pos, int id);

	void Generate(DeviceResources* deviceResources);

	Matrix GetModel();
	BlockId GetData(int x, int y, int z);

	void SetModel(Matrix& matrix);
	void SetChunckData(Vector3 pos, BlockData& data);

	void GenerateMesh(DeviceResources* deviceRes, std::vector<Chunck>& chuncks);

	void PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx);
	void PushCube(Vector3 pos, BlockData& data, byte drawFlags);

	void Draw(DeviceResources* deviceResources);

private:

	Vector3 To3D(int x);
	int To1D(int x, int y, int z);

	byte GenerateFlags(int x, int y, int z, std::vector<Chunck>& chuncks);
	bool ShouldDisplayFace(int x, int y, int z, int dx, int dy, int dz, std::vector<Chunck>& chuncks);
};

