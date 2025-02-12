#include "pch.h"
#include "Chunck.h"
#include "Engine/VertexLayout.h"
#include "TP/Buffer.h"
#include "TP/Block.h"
#include "TP/World.h"

Chunck::Chunck(Vector3 pos, int id)
{
	this->id = id;
	model = Matrix::CreateTranslation(pos);

	bounds = DirectX::BoundingBox(pos + Vector3(CHUNCK_SIZE / 2 - 0.5, CHUNCK_HEIGHT / 2 - 0.5, CHUNCK_SIZE / 2 - 0.5), Vector3(CHUNCK_SIZE / 2, CHUNCK_HEIGHT / 2, CHUNCK_SIZE / 2));

	for (int i = 0; i < CHUNCK_SIZE * CHUNCK_SIZE * CHUNCK_HEIGHT; i++) {
		mapIds[i] = EMPTY;
	}
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

BlockId Chunck::GetData(int x, int y, int z)
{
	return mapIds[To1D(x, y, z)];
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

void Chunck::GenerateMesh(DeviceResources* deviceRes, std::vector<Chunck> &chuncks)
{
	BlockData data = BlockData::Get(EMPTY);

	vertexBuffer.Flush();
	indexBuffer.Flush();

	for (int x = 0; x < CHUNCK_SIZE; x++) {
		for (int y = 0; y < CHUNCK_HEIGHT; y++) {
			for (int z = 0; z < CHUNCK_SIZE; z++) {
				data = BlockData::Get(mapIds[To1D(x,y,z)]);
				if(data.id != EMPTY) PushCube(Vector3(x, y, z), data,GenerateFlags(x,y,z,chuncks));
			}
		}
	}

	Generate(deviceRes);
	GenerateWaterMesh(deviceRes, 20);
}

void Chunck::GenerateWaterMesh(DeviceResources* deviceRes, int height)
{
	BlockData data = BlockData::Get(WATER);

	Vector3 pos;
	Vector3 up = Vector3::Forward;
	Vector3 right = Vector3::Right;
	int currentIndex;
	float uvy;
	float uvx;
	Vector4 normal;


	for (int x = 0; x < CHUNCK_SIZE; x++) {
		for (int z = 0; z < CHUNCK_SIZE; z++) {
			if (mapIds[To1D(x, height, z)] == EMPTY) { 

				currentIndex = vertexBufferWater.Size();
				uvx = data.texIdTop % 16;
				uvy = data.texIdTop / 16;

				pos = { x - 0.5f, (float)height, z + 0.5f };

				normal = ToVec4(up.Cross(right));

				vertexBufferWater.PushVertex({ ToVec4(pos), normal, {uvx, uvy + 1.0f} });
				vertexBufferWater.PushVertex({ ToVec4(pos + up), normal, {uvx, uvy} });
				vertexBufferWater.PushVertex({ ToVec4(pos + right), normal, {uvx + 1.0f, uvy + 1.0f} });
				vertexBufferWater.PushVertex({ ToVec4(pos + up + right), normal, {uvx + 1.0f, uvy} });

				indexBufferWater.PushTriangle(currentIndex, currentIndex + 1, currentIndex + 2);
				indexBufferWater.PushTriangle(currentIndex + 2, currentIndex + 1, currentIndex + 3);

				normal = -normal;

				vertexBufferWater.PushVertex({ ToVec4(pos), normal, {uvx, uvy + 1.0f} });
				vertexBufferWater.PushVertex({ ToVec4(pos + up), normal, {uvx, uvy} });
				vertexBufferWater.PushVertex({ ToVec4(pos + right), normal, {uvx + 1.0f, uvy + 1.0f} });
				vertexBufferWater.PushVertex({ ToVec4(pos + up + right), normal, {uvx + 1.0f, uvy} });

				indexBufferWater.PushTriangle(currentIndex, currentIndex + 2, currentIndex + 1);
				indexBufferWater.PushTriangle(currentIndex + 2, currentIndex + 3, currentIndex + 1);
			}
		}
	}

	if (vertexBufferWater.Size() == 0) {
		vertexBufferWater.PushVertex({ {0,0,0,0}, {0,0,0,0}, {0, 0} });
		vertexBufferWater.PushVertex({ {0,0,0,0}, {0,0,0,0}, {0, 0} });
		vertexBufferWater.PushVertex({ {0,0,0,0}, {0,0,0,0}, {0, 0} });
		indexBufferWater.PushTriangle(0, 1, 2);
	}

	vertexBufferWater.Create(deviceRes);
	indexBufferWater.Create(deviceRes);
}

Vector4 Chunck::ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}

void Chunck::PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx)
{
	int currentIndex = vertexBuffer.Size();
	float uvx =  uvIdx % 16;
	float uvy = uvIdx / 16;

	Vector4 normal = up.Cross(right);

	vertexBuffer.PushVertex({ ToVec4(pos), normal, {uvx, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up), normal, {uvx, uvy} });
	vertexBuffer.PushVertex({ ToVec4(pos + right), normal, {uvx + 1.0f, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up + right), normal, {uvx + 1.0f, uvy} });

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

void Chunck::DrawWater(DeviceResources* deviceResources)
{
	vertexBufferWater.Apply(deviceResources, 0);
	indexBufferWater.Apply(deviceResources);

	deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBufferWater.Size(), 0, 0);
}

Vector3 Chunck::To3D(int x)
{
	Vector3 vec;
	vec.z = ((int)x / (CHUNCK_SIZE * CHUNCK_HEIGHT));
	x -= ((int)vec.z * CHUNCK_SIZE * CHUNCK_HEIGHT);
	vec.y = (int)x / CHUNCK_SIZE;
	vec.z = (int)x % CHUNCK_SIZE;

	return vec;
}

int Chunck::To1D(int x, int y, int z)
{
	return (z * CHUNCK_SIZE * CHUNCK_HEIGHT) + (y * CHUNCK_SIZE) + x;
}

byte Chunck::GenerateFlags(int x, int y, int z, std::vector<Chunck>& chuncks)
{
	byte result = 0;
	result |= ShouldDisplayFace(x,y,z,0,0,-1,chuncks) ? 1 << 0 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, 0, 1, chuncks) ? 1 << 1 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, -1, 0, chuncks) ? 1 << 2 : 0;
	result |= ShouldDisplayFace(x, y, z, 0, 1, 0, chuncks) ? 1 << 3 : 0;
	result |= ShouldDisplayFace(x, y, z, -1, 0, 0, chuncks) ? 1 << 4 : 0;
	result |= ShouldDisplayFace(x, y, z, 1, 0, 0, chuncks) ? 1 << 5 : 0;

	return result;
}

bool Chunck::ShouldDisplayFace(int x, int y, int z, int dx, int dy, int dz, std::vector<Chunck>& chuncks)
{
	int nx = x + dx;
	int ny = y + dy;
	int nz = z + dz;

	BlockData data = BlockData::Get(mapIds[To1D(x, y, z)]);
	BlockData dataOther = BlockData::Get(EMPTY);

	if (ny < 0 || ny >= CHUNCK_HEIGHT) return true; // Seulement 1 chunck de hauteur

	// coords hors chunck
	if (nx < 0 || nx >= CHUNCK_SIZE || nz < 0 || nz >= CHUNCK_SIZE) {

		int xc = id % MAP_SIZE;
		int yc = id / MAP_SIZE;

		int xo = xc + dx;
		int yo = yc + dz;

		
		if (xo < 0 || xo >= MAP_SIZE || yo < 0 || yo >= MAP_SIZE) {
			// coords hors World
			return true;
		}
		else {
			// coords dans World
			int idO = yo * MAP_SIZE + xo;

			dataOther = BlockData::Get(chuncks.at(idO).GetData((nx + CHUNCK_SIZE) % CHUNCK_SIZE, ny, (nz + CHUNCK_SIZE) % CHUNCK_SIZE));
		}
	}
	else {
		// coords dans chunck
		dataOther = BlockData::Get(mapIds[To1D(nx, ny, nz)]);
	}


	

	if (dataOther.transparent && data.id == dataOther.id) return false;
	return dataOther.id == EMPTY || dataOther.transparent;
}
