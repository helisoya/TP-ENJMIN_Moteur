#pragma once

#include "Camera.h"
#include "World.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Player {
	
private:
	World* world;
	Vector3 position;

	float velocityY = 0;
	float walkSpeed = 10.0f;

	Mouse::ButtonStateTracker trackerMouse;
	Keyboard::KeyboardStateTracker trackerKeyboard;

	bool isBuilding = false;
	int currentIdxBlock = 0;

	BlockId blocksToPlace[8] = { EMPTY, DIRT, STONE,COBBLESTONE,WOOD,GLASS,CRAFTING_TABLE,DIAMOND_BLOCK };

	PerspectiveCamera camera = PerspectiveCamera(75, 1);
public:

	void SetWorld(World* world) { this->world = world; }

	Player(Vector3 pos);
	void Update(float dt, DirectX::Keyboard::State kb, Mouse::State ms);

	PerspectiveCamera* GetCamera();

};