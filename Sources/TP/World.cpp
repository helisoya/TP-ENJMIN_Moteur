#include "pch.h"

#include "World.h"
#include "PerlinNoise.hpp"
#include "Camera.h"

World::World()
{
}

void World::Generate(DeviceResources* deviceRes)
{
    this->deviceRes = deviceRes;
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

void World::Draw(DeviceResources* deviceRes, Camera& camera)
{
	ModelData dataModel = {};
	Vector3 pos;

	for (Chunck& chunck : chuncks) {
        if (chunck.bounds.Intersects(camera.GetFrostum())) {
            dataModel.model = chunck.GetModel().Transpose();

            constantBufferModel.SetData(dataModel, deviceRes);
            constantBufferModel.ApplyToVS(deviceRes, 0);

            chunck.Draw(deviceRes);
        }
	}

    dataModel.model = Matrix::Identity;
    constantBufferModel.SetData(dataModel,deviceRes);
}

void World::DrawSkybox(Vector3 pos,DeviceResources* deviceRes)
{
	ModelData dataModel = {};
	dataModel.model = Matrix::CreateTranslation(pos).Transpose();

	constantBufferModel.SetData(dataModel, deviceRes);
	skybox.Draw(deviceRes);

    dataModel.model = Matrix::Identity;
    constantBufferModel.SetData(dataModel,deviceRes);
}

void World::DrawWater(DeviceResources* deviceRes, Camera& camera)
{
    ModelData dataModel = {};
    Vector3 pos;

    for (Chunck& chunck : chuncks) {
        // if(camera.GetFrostum().Intersects(chunck.GetModel()))
        dataModel.model = chunck.GetModel().Transpose();

        constantBufferModel.SetData(dataModel, deviceRes);
        constantBufferModel.ApplyToVS(deviceRes, 0);

        chunck.DrawWater(deviceRes);
    }

    dataModel.model = Matrix::Identity;
    constantBufferModel.SetData(dataModel,deviceRes);
}


Vector2 World::To2D(int x)
{
	return Vector2(x % MAP_SIZE, x / MAP_SIZE);
}

int World::To1D(int x, int y)
{
	return y * MAP_SIZE + x;
}

void World::GenerateChunck(Vector3 pos, siv::BasicPerlinNoise<float>& noise, siv::BasicPerlinNoise<float>& noiseCave, DeviceResources* deviceRes)
{
	int id = chuncks.size();
	Chunck& chunck = chuncks.emplace_back(pos, id);

	BlockData data = BlockData::Get(EMPTY);
	int idx;
	float noiseValue;
    float noiseValueCave;
	int yMax;
	float mapSize = 64;

	for (int x = 0; x < CHUNCK_SIZE; x++) {
		for (int z = 0; z < CHUNCK_SIZE; z++) {
			
			noiseValue = (noise.noise2D((pos.x + x) / mapSize, (pos.z + z) / mapSize) + 1) / 2;
			yMax =  (int)(noiseValue * CHUNCK_HEIGHT);

			for (int y = 0; y < yMax; y++) {
                noiseValueCave = (noise.octave3D((pos.x + x) / mapSize, (pos.y + y) / mapSize, (pos.z + z) / mapSize,4,0.75f) + 1) / 2;

                if (y == 0) {
                    data = BlockData::Get(BEDROCK);
                }
				else if (noiseValueCave > 0.90f) {
					data = BlockData::Get(EMPTY);
				}
                else if (y == yMax - 1 && y <= 20) {
                    data = BlockData::Get(SAND);
                }
				else {
					data = BlockData::Get(y < yMax / 1.5f ? STONE : (y < yMax - 1 ? DIRT : GRASS));
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

void World::SearchForBlock(Vector3 pos, Vector3 forward, BlockId replaceWith)
{
    Vector3 target = pos + forward * 100.0f;
    BlockData data = BlockData::Get(replaceWith);

    Vector3 coordsInChunck;
    int chunckX;
    int chunckY;
    int chunckIdx;

    int x0 = (int)(pos.x + 0.5f);
    int y0 = (int)(pos.y + 0.5f);
    int z0 = (int)(pos.z + 0.5f);
    int x1 = (int)target.x;
    int y1 = (int)target.y;
    int z1 = (int)target.z;

    int xl = x0;
    int yl = y0;
    int zl = z0;

    int dx = abs(target.x - pos.x);
    int dy = abs(target.y - pos.y);
    int dz = abs(target.z - pos.z);
    int xs;
    int ys;
    int zs;

    if (target.x > pos.x)
        xs = 1;
    else
        xs = -1;
    if (target.y > pos.y)
        ys = 1;
    else
        ys = -1;
    if (target.z > pos.z)
        zs = 1;
    else
        zs = -1;

    // Driving axis is X-axis"
    if (dx >= dy && dx >= dz) {
        int p1 = 2 * dy - dx;
        int p2 = 2 * dz - dx;
        while (x0 != x1) {
            x0 += xs;
            if (p1 >= 0) {
                y0 += ys;
                p1 -= 2 * dx;
            }
            if (p2 >= 0) {
                z0 += zs;
                p2 -= 2 * dx;
            }
            p1 += 2 * dy;
            p2 += 2 * dz;

            chunckX = x0 / CHUNCK_SIZE;
            chunckY = z0 / CHUNCK_SIZE;

            if (chunckX < 0 || chunckX >= MAP_SIZE || chunckY < 0 || chunckY >= MAP_SIZE) return;
            
            coordsInChunck.x = x0 % CHUNCK_SIZE;
            coordsInChunck.y = y0 % CHUNCK_HEIGHT;
            coordsInChunck.z = z0 % CHUNCK_SIZE;

            chunckIdx = To1D(chunckX, chunckY);
            if (chuncks.at(chunckIdx).GetData(coordsInChunck.x, coordsInChunck.y, coordsInChunck.z) != EMPTY) {

                if (replaceWith == EMPTY) {
                    chuncks.at(chunckIdx).SetChunckData(coordsInChunck, data);

                    if (chunckX > 0) chuncks.at(To1D(chunckX - 1, chunckY)).GenerateMesh(deviceRes, chuncks);
                    if (chunckY > 0) chuncks.at(To1D(chunckX, chunckY - 1)).GenerateMesh(deviceRes, chuncks);
                    if (chunckX < MAP_SIZE - 1) chuncks.at(To1D(chunckX + 1, chunckY)).GenerateMesh(deviceRes, chuncks);
                    if (chunckY < MAP_SIZE - 1) chuncks.at(To1D(chunckX, chunckY + 1)).GenerateMesh(deviceRes, chuncks);
                }
                else {
                    coordsInChunck.x = xl % CHUNCK_SIZE;
                    coordsInChunck.y = yl % CHUNCK_HEIGHT;
                    coordsInChunck.z = zl % CHUNCK_SIZE;
                    chuncks.at(chunckIdx).SetChunckData(coordsInChunck, data);
                }

                chuncks.at(chunckIdx).GenerateMesh(deviceRes, chuncks);

                return;
            }

            xl = x0;
            yl = y0;
            zl = z0;
        }

        // Driving axis is Y-axis"
    }
    else if (dy >= dx && dy >= dz) {
        int p1 = 2 * dx - dy;
        int p2 = 2 * dz - dy;
        while (y0 != y1) {
            y0 += ys;
            if (p1 >= 0) {
                x0 += xs;
                p1 -= 2 * dy;
            }
            if (p2 >= 0) {
                z0 += zs;
                p2 -= 2 * dy;
            }
            p1 += 2 * dx;
            p2 += 2 * dz;

            chunckX = x0 / CHUNCK_SIZE;
            chunckY = z0 / CHUNCK_SIZE;

            if (chunckX < 0 || chunckX >= MAP_SIZE || chunckY < 0 || chunckY >= MAP_SIZE) return;

            coordsInChunck.x = x0 % CHUNCK_SIZE;
            coordsInChunck.y = y0 % CHUNCK_HEIGHT;
            coordsInChunck.z = z0 % CHUNCK_SIZE;

            chunckIdx = To1D(chunckX, chunckY);
            if (chuncks.at(chunckIdx).GetData(coordsInChunck.x, coordsInChunck.y, coordsInChunck.z) != EMPTY) {
                if (replaceWith == EMPTY) {
                    chuncks.at(chunckIdx).SetChunckData(coordsInChunck, data);

                    if (chunckX > 0) chuncks.at(To1D(chunckX - 1, chunckY)).GenerateMesh(deviceRes, chuncks);
                    if (chunckY > 0) chuncks.at(To1D(chunckX, chunckY - 1)).GenerateMesh(deviceRes, chuncks);
                    if (chunckX < MAP_SIZE - 1) chuncks.at(To1D(chunckX + 1, chunckY)).GenerateMesh(deviceRes, chuncks);
                    if (chunckY < MAP_SIZE - 1) chuncks.at(To1D(chunckX, chunckY + 1)).GenerateMesh(deviceRes, chuncks);
                }
                else {
                    coordsInChunck.x = xl % CHUNCK_SIZE;
                    coordsInChunck.y = yl % CHUNCK_HEIGHT;
                    coordsInChunck.z = zl % CHUNCK_SIZE;
                    chuncks.at(chunckIdx).SetChunckData(coordsInChunck, data);
                }

                chuncks.at(chunckIdx).GenerateMesh(deviceRes, chuncks);
                return;
            }

            xl = x0;
            yl = y0;
            zl = z0;
        }

        // Driving axis is Z-axis"
    }
    else {
        int p1 = 2 * dy - dz;
        int p2 = 2 * dx - dz;
        while (z0 != z1) {
            z0 += zs;
            if (p1 >= 0) {
                y0 += ys;
                p1 -= 2 * dz;
            }
            if (p2 >= 0) {
                x0 += xs;
                p2 -= 2 * dz;
            }
            p1 += 2 * dy;
            p2 += 2 * dx;

            chunckX = x0 / CHUNCK_SIZE;
            chunckY = z0 / CHUNCK_SIZE;

            if (chunckX < 0 || chunckX >= MAP_SIZE || chunckY < 0 || chunckY >= MAP_SIZE) return;

            coordsInChunck.x = x0 % CHUNCK_SIZE;
            coordsInChunck.y = y0 % CHUNCK_HEIGHT;
            coordsInChunck.z = z0 % CHUNCK_SIZE;

            chunckIdx = To1D(chunckX, chunckY);
            if (chuncks.at(chunckIdx).GetData(coordsInChunck.x, coordsInChunck.y, coordsInChunck.z) != EMPTY) {
                if (replaceWith == EMPTY) {
                    chuncks.at(chunckIdx).SetChunckData(coordsInChunck, data);

                    if (chunckX > 0) chuncks.at(To1D(chunckX - 1, chunckY)).GenerateMesh(deviceRes, chuncks);
                    if (chunckY > 0) chuncks.at(To1D(chunckX, chunckY - 1)).GenerateMesh(deviceRes, chuncks);
                    if (chunckX < MAP_SIZE - 1) chuncks.at(To1D(chunckX + 1, chunckY)).GenerateMesh(deviceRes, chuncks);
                    if (chunckY < MAP_SIZE - 1) chuncks.at(To1D(chunckX, chunckY + 1)).GenerateMesh(deviceRes, chuncks);
                }
                else {
                    coordsInChunck.x = xl % CHUNCK_SIZE;
                    coordsInChunck.y = yl % CHUNCK_HEIGHT;
                    coordsInChunck.z = zl % CHUNCK_SIZE;
                    chuncks.at(chunckIdx).SetChunckData(coordsInChunck, data);
                }

                chuncks.at(chunckIdx).GenerateMesh(deviceRes, chuncks);
                return;
            }

            xl = x0;
            yl = y0;
            zl = z0;
        }
    }

}



BlockId World::GetCube(int gx, int gy, int gz) {
    int cx = gx / CHUNCK_SIZE;
    int cz = gz / CHUNCK_SIZE;

    if (cx < 0 || cx >= MAP_SIZE || cz < 0 || cz >= MAP_SIZE || gy < 0 || gy >= CHUNCK_HEIGHT) return EMPTY;

    int lx = gx % CHUNCK_SIZE;
    int ly = gy % CHUNCK_HEIGHT;
    int lz = gz % CHUNCK_SIZE;

    Chunck& chunk = chuncks[To1D(cx, cz)];
    return chunk.GetData(lx, ly, lz);
}

float sign(float v) {
    if (v < 0) return -1;
    else return 1;
}

std::vector<std::array<int, 3>> World::Raycast(Vector3 pos, Vector3 dir, float maxDist) {
    std::map<float, std::array<int, 3>> cubes;

    if (dir.x != 0) {
        float deltaYX = dir.y / dir.x;
        float deltaZX = dir.z / dir.x;
        float offsetYX = pos.y - pos.x * deltaYX;
        float offsetZX = pos.z - pos.x * deltaZX;

        float cubeX = (dir.x > 0) ? ceil(pos.x) : floor(pos.x);
        do {
            Vector3 collision(cubeX, deltaYX * cubeX + offsetYX, deltaZX * cubeX + offsetZX);
            float dist = Vector3::Distance(pos, collision);
            if (dist > maxDist) break;

            cubes[dist] = {
                (int)floor(cubeX - ((dir.x < 0) ? 1 : 0)),
                (int)floor(collision.y),
                (int)floor(collision.z)
            };
            cubeX = cubeX + sign(dir.x);
        } while (true);
    }
    if (dir.y != 0) {
        float deltaXY = dir.x / dir.y;
        float deltaZY = dir.z / dir.y;
        float offsetXY = pos.x - pos.y * deltaXY;
        float offsetZY = pos.z - pos.y * deltaZY;

        float cubeY = (dir.y > 0) ? ceil(pos.y) : floor(pos.y);
        do {
            Vector3 collision(deltaXY * cubeY + offsetXY, cubeY, deltaZY * cubeY + offsetZY);
            float dist = Vector3::Distance(pos, collision);
            if (dist > maxDist) break;

            cubes[dist] = {
                (int)floor(collision.x),
                (int)floor(cubeY - ((dir.y < 0) ? 1 : 0)),
                (int)floor(collision.z)
            };
            cubeY = cubeY + sign(dir.y);
        } while (true);
    }
    if (dir.z != 0) {
        float deltaXZ = dir.x / dir.z;
        float deltaYZ = dir.y / dir.z;
        float offsetXZ = pos.x - pos.z * deltaXZ;
        float offsetYZ = pos.y - pos.z * deltaYZ;

        float cubeZ = (dir.z > 0) ? ceil(pos.z) : floor(pos.z);
        do {
            Vector3 collision(deltaXZ * cubeZ + offsetXZ, deltaYZ * cubeZ + offsetYZ, cubeZ);
            float dist = Vector3::Distance(pos, collision);
            if (dist > maxDist) break;

            cubes[dist] = {
                (int)floor(collision.x),
                (int)floor(collision.y),
                (int)floor(cubeZ - ((dir.z < 0) ? 1 : 0)),
            };
            cubeZ = cubeZ + sign(dir.z);
        } while (true);
    }

    std::vector<std::array<int, 3>> res;
    std::transform(
        cubes.begin(), cubes.end(),
        std::back_inserter(res),
        [](auto& v) { return v.second; });
    return res;
}