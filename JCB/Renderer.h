#pragma once
#include "pch.h"

namespace JCB
{
	class Renderer
	{
	public:
		void Init(HWND hwnd);
		void Update();
		void Render();
		void PreRender();
		void PostRender();

		void CreateDeviceAndSwapChain();
		void CreateRenderTargetView();
		void SetViewPort();

		void CreateGeometry();
		void CreateVertexBuffer();
		void CreateIndexBuffer();

		void CreateVertexShader();
		void CreateInputLayout();

		void CreateRasterizerState();

		void CreatePixelShader();
		//void CreateShaderResourceView();
		void CreateSamplerState();

		void CreateTexture2d();

		void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);


	private:
		HWND _hwnd;

		ComPtr<ID3D11Device> _device;
		ComPtr<ID3D11DeviceContext> _deviceContext;
		ComPtr<IDXGISwapChain> _swapChain;
		ComPtr<ID3D11RenderTargetView> _renderTargetView;
		float _clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		D3D11_VIEWPORT _viewPort;

		// IA, VS, RS, PS, OM
		vector<VertexTexture> _vertices;
		vector<uint32> _indices;

		ComPtr<ID3D11Buffer> _vertexBuffer;
		ComPtr<ID3D11Buffer> _indexBuffer;

		ComPtr<ID3D11VertexShader> _vertexShader;
		ComPtr<ID3DBlob> _vsBlob;
		ComPtr<ID3D11InputLayout> _inputLayout;

		ComPtr<ID3D11RasterizerState> _rasterizerState;

		ComPtr<ID3D11PixelShader> _pixelShader;
		ComPtr<ID3DBlob> _psBlob;
		ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
		ComPtr<ID3D11SamplerState> _samplerState;

		ComPtr<ID3D11Texture2D> _canvasTexture;
		ComPtr<ID3D11RenderTargetView> _canvasRenderTargetView;

		Image image;
		shared_ptr<Circle> _circle;
	};

}