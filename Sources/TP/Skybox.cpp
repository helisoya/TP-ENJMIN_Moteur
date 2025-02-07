#include "pch.h"

#include "Skybox.h"

Skybox::Skybox()
{
}

void Skybox::Generate(DeviceResources* deviceResources)
{
	float size = 499.0f;
	PushFace({ -size /2.0f, -size / 2.0f, size / 2.0f }, Vector3::Up * size, Vector3::Right * size, 2);
	PushFace({ size / 2.0f, -size / 2.0f, size / 2.0f }, Vector3::Up * size, Vector3::Forward * size, 2);
	PushFace({ size / 2.0f, -size / 2.0f,-size / 2.0f }, Vector3::Up * size, Vector3::Left * size, 2);
	PushFace({ -size / 2.0f, -size / 2.0f,-size / 2.0f }, Vector3::Up * size, Vector3::Backward * size, 2);
	PushFace({ -size / 2.0f,  size / 2.0f, size / 2.0f }, Vector3::Forward * size, Vector3::Right * size, 1);
	PushFace({ -size / 2.0f, -size / 2.0f,-size / 2.0f }, Vector3::Backward * size, Vector3::Right * size, 3);

	vertexBuffer.Create(deviceResources);
	indexBuffer.Create(deviceResources);
}

Vector4 Skybox::ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}


void Skybox::PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx)
{
	int currentIndex = vertexBuffer.Size();
	float uvx = uvIdx % 2;
	float uvy = uvIdx / 2;

	Vector4 normal = up.Cross(right);

	vertexBuffer.PushVertex({ ToVec4(pos),normal, {uvx, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up),normal, {uvx, uvy} });
	vertexBuffer.PushVertex({ ToVec4(pos + right),normal, {uvx + 1.0f, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up + right),normal, {uvx + 1.0f, uvy} });

	indexBuffer.PushTriangle(currentIndex, currentIndex + 2, currentIndex + 1);
	indexBuffer.PushTriangle(currentIndex + 2, currentIndex + 3, currentIndex + 1);
}

void Skybox::Draw(DeviceResources* deviceResources)
{
	vertexBuffer.Apply(deviceResources, 0);
	indexBuffer.Apply(deviceResources);

	deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBuffer.Size(), 0, 0);
}
