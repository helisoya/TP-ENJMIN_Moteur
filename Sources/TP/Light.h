#pragma once

#include "TP/Buffer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Light {

public:

private:

	Vector3 position;
	Vector3 lookAt;
	Vector3 direction;
	
	Matrix view;
	Matrix projection;

	Color ambiant;
	Color diffuse;

	struct LightData {
		Vector4 LightPosition;
		Vector3 Direction;
		Color Ambiant;
		Color Diffuse;

		Matrix LightView;
		Matrix LightProjection;
		float pad;
	};

	ConstantBuffer<LightData> constantBufferLight;

public:

	Light();

	void Generate(DeviceResources* deviceRes);
	void Apply(DeviceResources* deviceRes);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);

private:


};