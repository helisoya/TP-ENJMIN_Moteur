#pragma once

#include "TP/Buffer.h"
#include "Block.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class World;
class Camera {
protected:
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

	DirectX::BoundingFrustum frustum;
public:
	Camera();
	virtual ~Camera();


	Vector3& GetPosition() { return camPos; }
	Quaternion GetRotation() { return camRot; }
	DirectX::BoundingFrustum GetFrostum() { return frustum; }
	Matrix GetInverseViewMatrix() { return view.Invert(); };

	void SetPosition(Vector3 pos) { camPos = pos; UpdateViewMatrix(); }
	void SetRotation(Quaternion rot) { camRot = rot; UpdateViewMatrix(); }

	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse* ms, World& world);
	void UpdateViewMatrix();

	Vector3 Forward() const { return Vector3::TransformNormal(Vector3::Forward, view.Invert()); };
	Vector3 Up() const { return Vector3::TransformNormal(Vector3::Up, view.Invert()); };
	Vector3 Right() const { return Vector3::TransformNormal(Vector3::Right, view.Invert()); };

	void ApplyCamera(DeviceResources* deviceRes);
};


class PerspectiveCamera : public Camera {
	float fov;

public:
	PerspectiveCamera(float fov, float aspectRatio);
	
	void UpdateAspectRatio(float aspectRatio);
};