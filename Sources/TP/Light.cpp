#include "pch.h"

#include "Light.h"

Light::Light()
{
	direction = Vector3::Down + Vector3::Right * 0.25f;

	ambiant = Color(0.2f, 0.2f, 0.2f, 1.0f);
	diffuse = Colors::White;
}

void Light::Generate(DeviceResources* deviceRes)
{
	constantBufferLight.Create(deviceRes);
}

void Light::Apply(DeviceResources* deviceRes)
{
	LightData data = {};
	data.Ambiant = ambiant;
	data.Diffuse = diffuse;
	data.Direction = direction;
	data.pad = 0;

	constantBufferLight.SetData(data,deviceRes);
	constantBufferLight.ApplyToPS(deviceRes, 0);
}


