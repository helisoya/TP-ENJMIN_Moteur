#pragma once

using Microsoft::WRL::ComPtr;

template<typename TVertex>
class VertexBuffer {
public:

private:
	ComPtr<ID3D11Buffer> buffer;
	std::vector<TVertex> data;

public:
	VertexBuffer() {};

	// Add data manip methods

	int PushVertex(TVertex v) {
		data.push_back(v);
		return data.size() - 1;
	}

	int Size() {
		return data.size();
	}


	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(TVertex) * data.size(),
			D3D11_BIND_VERTEX_BUFFER
		);

		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = data.data();


		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	void Apply(DeviceResources *deviceRes, int slot = 0) {
		ID3D11Buffer* vbs[] = { buffer.Get() };
		const UINT strides[] = { sizeof(TVertex) };
		const UINT offsets[] = { 0 };

		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(
			slot,
			1,
			vbs,
			strides,
			offsets
		);
	}

private:


};

class IndexBuffer {
public:

private:
	ComPtr<ID3D11Buffer> buffer;
	std::vector<uint32_t> data;

public:
	IndexBuffer() {};

	// Add data manip methods

	void PushTriangle(uint32_t a, uint32_t b, uint32_t c) {
		data.push_back(a);
		data.push_back(b);
		data.push_back(c);
	}

	int Size() {
		return data.size();
	}


	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(uint32_t) * data.size(),
			D3D11_BIND_INDEX_BUFFER
		);

		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = data.data();


		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	void Apply(DeviceResources* deviceRes) {

		deviceRes->GetD3DDeviceContext()->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

private:

};

template<typename TData>
class ConstantBuffer {
public:

private:
	ComPtr<ID3D11Buffer> buffer;
	TData data;

public:
	ConstantBuffer() {};

	// Add data manip methods

	void SetData(TData d, DeviceResources* deviceRes) {
		data = d;
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}


	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(TData),
			D3D11_BIND_CONSTANT_BUFFER
		);


		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			nullptr,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	void ApplyToVS(DeviceResources* deviceRes, int slot = 0) {
		ID3D11Buffer* cbs[] = { buffer.Get() };
		deviceRes->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
	}

private:


};