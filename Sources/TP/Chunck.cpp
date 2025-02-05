#include "pch.h"
#include "Chunck.h"
#include "Engine/VertexLayout.h"
#include "TP/Buffer.h"
#include "TP/Block.h"

Chunck::Chunck(Vector3 pos)
{
	model = Matrix::CreateTranslation(pos);
}

void Chunck::Generate(DeviceResources* deviceResources)
{
	vertexBuffer.Create(deviceResources);
	indexBuffer.Create(deviceResources);
}

Matrix Chunck::GetModel()
{
	return model;
}

void Chunck::SetModel(Matrix& matrix)
{
	this->model = matrix;
}

void Chunck::SetChunckData(Vector3 pos, BlockData& data)
{
	int idx = To1D(pos.x, pos.y, pos.z);
	mapIds[idx] = data.id;
}

void Chunck::GenerateMesh(DeviceResources* deviceRes)
{
	BlockData data = BlockData::Get(EMPTY);

	vertexBuffer.Flush();
	indexBuffer.Flush();

	for (int x = 0; x < CHUNCK_SIZE; x++) {
		for (int y = 0; y < CHUNCK_SIZE; y++) {
			for (int z = 0; z < CHUNCK_SIZE; z++) {
				data = BlockData::Get(mapIds[To1D(x,y,z)]);
				if(data.id != EMPTY) PushCube(Vector3(x, y, z), data,GenerateFlags(x,y,z));
			}
		}
	}

	Generate(deviceRes);
}

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}

void Chunck::PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx)
{
	int currentIndex = vertexBuffer.Size();
	float uvx =  uvIdx % 16;
	float uvy = uvIdx / 16;

	vertexBuffer.PushVertex({ ToVec4(pos), {uvx, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up), {uvx, uvy} });
	vertexBuffer.PushVertex({ ToVec4(pos + right), {uvx + 1.0f, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up + right), {uvx + 1.0f, uvy} });

	indexBuffer.PushTriangle(currentIndex, currentIndex + 1, currentIndex + 2);
	indexBuffer.PushTriangle(currentIndex + 2, currentIndex + 1, currentIndex + 3);
}

void Chunck::PushCube(Vector3 pos, BlockData& data, byte drawFlags)
{
	// (facesToGenerate & (1 << 0)) != 0


	if((drawFlags & (1 << 1)) != 0) PushFace({ pos.x - 0.5f, pos.y - 0.5f, pos.z + 0.5f }, Vector3::Up, Vector3::Right, data.texIdSide);
	if ((drawFlags & (1 << 5)) != 0) PushFace({ pos.x + 0.5f, pos.y - 0.5f, pos.z + 0.5f }, Vector3::Up, Vector3::Forward, data.texIdSide);
	if ((drawFlags & (1 << 0)) != 0) PushFace({ pos.x + 0.5f, pos.y - 0.5f, pos.z -0.5f }, Vector3::Up, Vector3::Left, data.texIdSide);
	if ((drawFlags & (1 << 4)) != 0) PushFace({ pos.x - 0.5f, pos.y - 0.5f, pos.z -0.5f }, Vector3::Up, Vector3::Backward, data.texIdSide);
	if ((drawFlags & (1 << 3)) != 0) PushFace({ pos.x - 0.5f, pos.y + 0.5f, pos.z + 0.5f }, Vector3::Forward, Vector3::Right, data.texIdTop);
	if ((drawFlags & (1 << 2)) != 0) PushFace({ pos.x - 0.5f, pos.y - 0.5f, pos.z - 0.5f }, Vector3::Backward, Vector3::Right, data.texIdBottom);
}

void Chunck::Draw(DeviceResources* deviceResources)
{
	vertexBuffer.Apply(deviceResources, 0);
	indexBuffer.Apply(deviceResources);

	deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBuffer.Size(), 0, 0);
}

Vector3 Chunck::To3D(int x)
{
	Vector3 vec;
	vec.z = ((int)x / (CHUNCK_SIZE * CHUNCK_SIZE));
	x -= ((int)vec.z * CHUNCK_SIZE * CHUNCK_SIZE);
	vec.y = (int)x / CHUNCK_SIZE;
	vec.z = (int)x % CHUNCK_SIZE;

	return vec;
}

int Chunck::To1D(int x, int y, int z)
{
	return (z * CHUNCK_SIZE * CHUNCK_SIZE) + (y * CHUNCK_SIZE) + x;
}

byte Chunck::GenerateFlags(int x, int y, int z)
{
	byte result = 0;
	result |= ShouldDisplayFace(x,y,z,0,0,-1) ? 1 << 0 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, 0, 1) ? 1 << 1 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, -1, 0) ? 1 << 2 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, 1, 0) ? 1 << 3 : 0;
	result |= ShouldDisplayFace(x, y, z, -1, 0, 0) ? 1 << 4 : 0;
	result |= ShouldDisplayFace(x, y, z, 1, 0, 0) ? 1 << 5 : 0;

	return result;
}

bool Chunck::ShouldDisplayFace(int x, int y, int z, int dx, int dy, int dz)
{
	int nx = x + dx;
	int ny = y + dy;
	int nz = z + dz;

	if (nx < 0 || nx >= CHUNCK_SIZE || ny < 0 || ny >= CHUNCK_SIZE || nz < 0 || nz >= CHUNCK_SIZE) return true;

	BlockData data =BlockData::Get(mapIds[To1D(nx, ny, nz)]);
	return data.id == EMPTY || data.transparent;
}
