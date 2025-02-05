#include "pch.h"
#include "Cube.h"
#include "Engine/VertexLayout.h"
#include "TP/Buffer.h"
#include "TP/Block.h"

Cube::Cube(Vector3 pos, BlockData& data) : blockData(data)
{
	model = Matrix::CreateTranslation(pos);
}

void Cube::Generate(DeviceResources* deviceResources)
{

	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right, blockData.texIdSide);
	PushFace({ 0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward, blockData.texIdSide);
	PushFace({ 0.5f, -0.5f,-0.5f }, Vector3::Up, Vector3::Left, blockData.texIdSide);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Up, Vector3::Backward, blockData.texIdSide);
	PushFace({ -0.5f,  0.5f, 0.5f }, Vector3::Forward, Vector3::Right, blockData.texIdTop);
	PushFace({ -0.5f, -0.5f,-0.5f }, Vector3::Backward, Vector3::Right, blockData.texIdBottom);

	vertexBuffer.Create(deviceResources);
	indexBuffer.Create(deviceResources);
}

Matrix Cube::GetModel()
{
	return model;
}

void Cube::SetModel(Matrix& matrix)
{
	this->model = matrix;
}

Vector4 ToVec4(const Vector3& v) {
	return Vector4(v.x, v.y, v.z, 1.0f);
}

void Cube::PushFace(Vector3 pos, Vector3 up, Vector3 right, int uvIdx)
{
	int currentIndex = vertexBuffer.Size();
	float uvx =  uvIdx % 16;
	float uvy = uvIdx / 16;

	vertexBuffer.PushVertex({ ToVec4(pos), {uvx, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up), {uvx, uvy} });
	vertexBuffer.PushVertex({ ToVec4(pos + right), {uvx + 1.0f, uvy + 1.0f} });
	vertexBuffer.PushVertex({ ToVec4(pos + up + right), {uvx + 1.0f, uvy} });

	indexBuffer.PushTriangle(currentIndex, currentIndex + 1, currentIndex + 2);
	indexBuffer.PushTriangle(currentIndex + 2, currentIndex + 1, currentIndex + 3);
}

void Cube::Draw(DeviceResources* deviceResources)
{
	vertexBuffer.Apply(deviceResources, 0);
	indexBuffer.Apply(deviceResources);

	deviceResources->GetD3DDeviceContext()->DrawIndexed(indexBuffer.Size(), 0, 0);
}
