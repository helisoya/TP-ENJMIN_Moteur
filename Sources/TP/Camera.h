#pragma once

#include "TP/Buffer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera {
	float fov;
	float nearPlane = 0.01f;
	float farPlane = 500.0f;

	Vector3 camPos = Vector3(0, 0, 0);
	Quaternion camRot = Quaternion::Identity;
	Matrix projection;
	Matrix view;

	int lastMouseX = 0;
	int lastMouseY = 0;

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
	};
	ConstantBuffer<MatrixData>* cbCamera = nullptr;
public:
	Camera(float fov, float aspectRatio);
	~Camera();


	Vector3& GetPosition();

	void UpdateAspectRatio(float aspectRatio);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse* ms);

	void ApplyCamera(DeviceResources* deviceRes);
};