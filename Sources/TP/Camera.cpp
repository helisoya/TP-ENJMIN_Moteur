#include "pch.h"

#include "Camera.h"
#include "World.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


Camera::Camera()
{
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}


void PerspectiveCamera::UpdateAspectRatio(float aspectRatio) {
	// TP updater matrice proj
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse, World& world) {
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

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	Vector3 newForward = Vector3::Transform(Vector3::Forward, camRot);
	Vector3 newUp = Vector3::Transform(Vector3::Up, camRot);

	view = Matrix::CreateLookAt(camPos, camPos + newForward, newUp);

	BoundingFrustum::CreateFromMatrix(frustum, projection, true);
	frustum.Transform(frustum, view.Invert());
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!cbCamera) {
		cbCamera = new ConstantBuffer<MatrixData>();
		cbCamera->Create(deviceRes);
	}

	// TP envoyer data

	MatrixData data = {};
	data.mView = view.Transpose();
	data.mProjection = projection.Transpose();

	cbCamera->SetData(data, deviceRes);

	cbCamera->ApplyToVS(deviceRes, 1);
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio) : fov(fov)
{
	view = Matrix::CreateLookAt(Vector3(0, 0, 5), Vector3::Zero, Vector3::Up);
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}
