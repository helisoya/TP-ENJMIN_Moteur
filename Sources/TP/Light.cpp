#include "pch.h"

#include "Light.h"

Light::Light()
{
	direction = Vector3::Down + Vector3::Right * 0.25f;
	position = Vector3::Up * 70;
	lookAt = position + direction;
		
	ambiant = Color(0.2f, 0.2f, 0.2f, 1.0f);
	diffuse = Colors::White;

	GenerateViewMatrix();
	GenerateProjectionMatrix(0.01f, 500.0f);
}

void Light::Generate(DeviceResources* deviceRes)
{
	constantBufferLight.Create(deviceRes);
}

void Light::Apply(DeviceResources* deviceRes)
{
	LightData data = {};
	data.LightPosition = Vector4(position.x, position.y, position.z , 0);
	data.Ambiant = ambiant;
	data.Diffuse = diffuse;
	data.Direction = direction;
	data.LightView = view.Transpose();
	data.LightProjection = projection.Transpose();
	data.pad = 0;

	constantBufferLight.SetData(data,deviceRes);
	constantBufferLight.ApplyToVS(deviceRes, 2);
	constantBufferLight.ApplyToPS(deviceRes, 0);
}

void Light::GenerateViewMatrix()
{
	view = Matrix::CreateLookAt(position, lookAt, Vector3::Up);
}

void Light::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView = 3.14159265358979323846f / 2.0f;
	float screenAspect = 1.0f;

	//projection = Matrix::CreateOrthographic(512, 512, screenDepth, screenNear);
	//projection = Matrix::CreatePerspective(512, 512, screenDepth, screenNear);
	projection = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}


