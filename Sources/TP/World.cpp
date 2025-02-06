#include "pch.h"

#include "World.h"
#include "PerlinNoise.hpp"

World::World()
{
}

void World::Generate(DeviceResources* deviceRes)
{
	constantBufferModel.Create(deviceRes);

	// GenerateWorld

	skybox.Generate(deviceRes);

	siv::BasicPerlinNoise<float> noise(546657456757);
	siv::BasicPerlinNoise<float> noiseCaves(494911989849);

	for (int z = 0; z < MAP_SIZE; z++) {
		for (int x = 0; x < MAP_SIZE; x++) {
			GenerateChunck(Vector3(CHUNCK_SIZE * x, 0, CHUNCK_SIZE * z),noise, noiseCaves, deviceRes);
		}
	}

	GenerateMesh(deviceRes);
}

void World::Draw(DeviceResources* deviceRes)
{
	ModelData dataModel = {};
	Vector3 pos;

	for (Chunck& chunck : chuncks) {
		dataModel.model = chunck.GetModel().Transpose();

		constantBufferModel.SetData(dataModel, deviceRes);
		constantBufferModel.ApplyToVS(deviceRes, 0);

		chunck.Draw(deviceRes);
	}
}

void World::DrawSkybox(Vector3 pos,DeviceResources* deviceRes)
{
	ModelData dataModel = {};
	dataModel.model = Matrix::CreateTranslation(pos).Transpose();

	constantBufferModel.SetData(dataModel, deviceRes);
	skybox.Draw(deviceRes);
}


void World::GenerateChunck(Vector3 pos, siv::BasicPerlinNoise<float>& noise, siv::BasicPerlinNoise<float>& noiseCave, DeviceResources* deviceRes)
{
	int id = chuncks.size();
	Chunck& chunck = chuncks.emplace_back(pos, id);

	BlockData data = BlockData::Get(EMPTY);
	int idx;
	float noiseValue;
	int yMax;
	float mapSize = CHUNCK_SIZE * MAP_SIZE;

	for (int x = 0; x < CHUNCK_SIZE; x++) {
		for (int z = 0; z < CHUNCK_SIZE; z++) {
			
			noiseValue = (noise.noise2D((pos.x + x) / mapSize, (pos.z + z) / mapSize) + 1) / 2;

			yMax =  (int)(noiseValue * 42);

			for (int y = 0; y < yMax; y++) {
				if (y < 3) {
					data = BlockData::Get(STONE);
				}
				else {
					data = BlockData::Get(y < yMax / 2 ? STONE : (y < yMax - 1 ? DIRT : GRASS));
				}
				
				chunck.SetChunckData(Vector3(x, y, z), data);
			}
		}
	}
}

void World::GenerateMesh(DeviceResources* deviceRes)
{
	for (Chunck& chunck : chuncks) {
		chunck.GenerateMesh(deviceRes, chuncks);
	}
}
