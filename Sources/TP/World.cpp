#include "pch.h"

#include "World.h"

World::World()
{
}

void World::Generate(DeviceResources* deviceRes)
{
	constantBufferModel.Create(deviceRes);

	// GenerateWorld

	skybox.Generate(deviceRes);


	Chunck& chunck= chuncks.emplace_back(Vector3::Zero);

	BlockData data = BlockData::Get(EMPTY);
	int idx;
	for (int x = 0; x < Chunck::CHUNCK_SIZE; x++) {
		for (int y = 0; y < Chunck::CHUNCK_SIZE; y++) {
			for (int z = 0; z < Chunck::CHUNCK_SIZE; z++) {
				data = BlockData::Get(y < 12 ? STONE : (y < 15 ? DIRT : GRASS));
				chunck.SetChunckData(Vector3(x, y, z), data);
			}
		}
	}

	data = BlockData::Get(EMPTY);
	chunck.SetChunckData(Vector3(8, 15, 8), data);
	chunck.SetChunckData(Vector3(8, 14, 8), data);
	chunck.SetChunckData(Vector3(8, 13, 8), data);
	chunck.SetChunckData(Vector3(7, 13, 8), data);
	chunck.SetChunckData(Vector3(7, 12, 8), data);


	chunck.GenerateMesh(deviceRes);
}

void World::Draw(DeviceResources* deviceRes)
{
	ModelData dataModel = {};
	Vector3 pos;

	for (Chunck& chunck : chuncks) {
		dataModel.model = chunck.GetModel();

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


