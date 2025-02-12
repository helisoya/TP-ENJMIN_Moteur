#include "pch.h"

#include "Player.h"

Vector3 collisionPoints[] = {
	{ 0.3f,     0,     0},
	{-0.3f,     0,     0},
	{    0,     0,  0.3f},
	{    0,     0, -0.3f},
	{    0, -0.5f,     0},
	{ 0.3f,  1.0f,     0},
	{-0.3f,  1.0f,     0},
	{    0,  1.0f,  0.3f},
	{    0,  1.0f, -0.3f},
	{    0,  1.5f,     0},
};

Player::Player(Vector3 pos) : camera(60,1)
{
	position = pos;
	camera.SetPosition(position + Vector3(0, 1.25f, 0));
}

void Player::Update(float dt, DirectX::Keyboard::State kb, Mouse::State ms)
{
	trackerMouse.Update(ms);
	trackerKeyboard.Update(kb);

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	Vector3 move = Vector3::TransformNormal(delta, camera.GetInverseViewMatrix());
	move.y = 0.0;
	move.Normalize();
	position += move * dt * walkSpeed;

	Quaternion camRot = camera.GetRotation();
	camRot *= Quaternion::CreateFromAxisAngle(camera.Right(), -ms.y * dt * 0.25f);
	camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -ms.x * dt * 0.25f);



	velocityY += -30 * dt;

	Vector3 nextPos = position + Vector3(0, velocityY, 0) * dt;
	BlockId downBlock = world->GetCube(floor(nextPos.x + 0.5f), floor(nextPos.y + 0.5f), floor(nextPos.z + 0.5f));
	if (downBlock != EMPTY) {
		velocityY = -5 * dt;
		if (kb.Space)
			velocityY = 10.0f;
	}

	position += Vector3(0, velocityY, 0) * dt;

	for (Vector3 colPoint : collisionPoints) {
		Vector3 colPos = position + colPoint + Vector3(0.5f, 0.5f, 0.5f);

		BlockId block = world->GetCube(floor(colPos.x), floor(colPos.y), floor(colPos.z));

		if (block != EMPTY) {
			if (colPoint.x != 0)
				position.x += round(colPos.x) - colPos.x;
			if (colPoint.z != 0)
				position.z += round(colPos.z) - colPos.z;
			if (colPoint.y != 0 && colPoint.x == 0 && colPoint.z == 0)
				position.y += round(colPos.y) - colPos.y;
		}
	}

	camera.SetRotation(camRot);
	camera.SetPosition(position + Vector3(0, 1.25f, 0));


	if (kb.D1) currentIdxBlock = 0;
	if (kb.D2) currentIdxBlock = 1;
	if (kb.D3) currentIdxBlock = 2;
	if (kb.D4) currentIdxBlock = 3;
	if (kb.D5) currentIdxBlock = 4;
	if (kb.D6) currentIdxBlock = 5;
	if (kb.D7) currentIdxBlock = 6;
	if (kb.D8) currentIdxBlock = 7;



	if (trackerMouse.leftButton) {
		if (!isBuilding) {
			isBuilding = true;

			world->SearchForBlock(camera.GetPosition(), camera.Forward(), blocksToPlace[currentIdxBlock]);
		}
	}
	else {
		isBuilding = false;
	}

	/*
	float camSpeedRot = 0.10f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix viewInverse = view.Invert();

	Vector3 delta;
	if (kb.Z) delta += Vector3::Forward;
	if (kb.S) delta += Vector3::Backward;
	if (kb.Q) delta += Vector3::Left;
	if (kb.D) delta += Vector3::Right;
	camPos += Vector3::TransformNormal(delta, viewInverse) * camSpeed * dt;


	if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;

		lastMouseX += dx;
		lastMouseY += dy;

		if (mstate.rightButton) {
			Vector3 deltaMouse;
			if (kb.LeftShift || kb.RightShift)
				deltaMouse = Vector3(0, 0, dy);
			else
				deltaMouse = Vector3(-dx, dy, 0);
			camPos += Vector3::TransformNormal(deltaMouse, viewInverse) * camSpeed * dt;
			// TP Translate camera a partir de dx/dy
		}
		else if (mstate.leftButton) {
			camRot *= Quaternion::CreateFromAxisAngle(Vector3::TransformNormal(Vector3::Right, viewInverse), -dy * dt);
			camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -dx * dt);
		}
		else {
			mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
	}
	else if (mstate.rightButton || mstate.leftButton) {
		mouse->SetMode(Mouse::MODE_RELATIVE);
	}

	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);

	view = Matrix::CreateLookAt(camPos, camPos + newForward, newUp);

	BoundingFrustum::CreateFromMatrix(frustum, projection, true);
	frustum.Transform(frustum, view.Invert());


	if (kb.D1) currentIdxBlock = 0;
	if (kb.D2) currentIdxBlock = 1;
	if (kb.D3) currentIdxBlock = 2;
	if (kb.D4) currentIdxBlock = 3;
	if (kb.D5) currentIdxBlock = 4;
	if (kb.D6) currentIdxBlock = 5;
	if (kb.D7) currentIdxBlock = 6;
	if (kb.D8) currentIdxBlock = 7;

	if (kb.P) {
		if (!isBuilding) {
			isBuilding = true;

			Matrix viewProjInvert = (view * projection).Invert().Transpose();

			Vector3 nearVec(lastMouseX, -lastMouseY, 0);
			Vector3 farVec(lastMouseX, -lastMouseY, 1);

			Vector3 rayOrigin = XMVector3TransformCoord(nearVec, viewProjInvert);
			Vector3 rayEnd = XMVector3TransformCoord(farVec, viewProjInvert);
			Vector3 rayDir = (rayEnd - rayOrigin);
			rayDir.Normalize();

			world.SearchForBlock(camPos, newForward, blocksToPlace[currentIdxBlock]);
		}
	}
	else {
		isBuilding = false;
	}*/
}

PerspectiveCamera* Player::GetCamera()
{
	return &camera;
}
