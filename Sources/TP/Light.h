#pragma once

#include "TP/Buffer.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Light {

public:

private:

	Vector3 direction;
	Color ambiant;
	Color diffuse;

	struct LightData {
		Vector3 Direction;
		Color Ambiant;
		Color Diffuse;
		float pad;
	};

	ConstantBuffer<LightData> constantBufferLight;

public:

	Light();

	void Generate(DeviceResources* deviceRes);
	void Apply(DeviceResources* deviceRes);

private:


};