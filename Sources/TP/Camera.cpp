#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : fov(fov) {
	// TP initialiser matrices
	camPos = Vector3(0, 0, 5);
	view = Matrix::CreateLookAt(Vector3(0, 0, 5), Vector3::Zero, Vector3::Up);
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() {
	if (cbCamera) delete cbCamera;
	cbCamera = nullptr;
}

Vector3& Camera::GetPosition()
{
	return camPos;
}

void Camera::UpdateAspectRatio(float aspectRatio) {
	// TP updater matrice proj
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse) {
	float camSpeedRot = 10.0f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix im = view.Invert();

	// TP: deplacement par clavier 

	float mx = kb.D - kb.Q;
	float mz = kb.Z - kb.S;

	Vector3 front = Vector3::TransformNormal(Vector3::Forward, im);
	Vector3 right = Vector3::TransformNormal(Vector3::Right, im);
	Vector3 up = Vector3::TransformNormal(Vector3::Up, im);

	camPos += front * mz * camSpeed * dt + right * mx * camSpeed * dt;

	if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;

		lastMouseX = 0;
		lastMouseY = 0;

		if (mstate.rightButton) {
			// TP Translate camera a partir de dx/dy
			camPos += right * dx * camSpeedMouse * dt - up * dy * camSpeedMouse * dt;

		}
		else if (mstate.leftButton) {
			// TP Rotate camera a partir de dx/dy
			lastMouseX = dx * dt * camSpeedRot;
			lastMouseY = dy * dt * camSpeedRot;
			//view *= Matrix::CreateRotationX(XMConvertToRadians(dx * dt * camSpeedRot)) * Matrix::CreateRotationY(XMConvertToRadians(dy * dt * camSpeedRot));

		}
		else {
			mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
	}
	else if (mstate.rightButton || mstate.leftButton) {
		mouse->SetMode(Mouse::MODE_RELATIVE);
	}

	// TP updater matrice view

	view = Matrix::CreateLookAt(camPos, camPos+front, up) 
		* Matrix::Matrix::CreateRotationY(XMConvertToRadians(lastMouseX)) * Matrix::CreateRotationX(XMConvertToRadians(lastMouseY));
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