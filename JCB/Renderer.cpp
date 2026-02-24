#include "pch.h"
#include "Renderer.h"

namespace JCB
{
    void Renderer::Init(HWND hwnd)
    {
        _hwnd = hwnd;

        image.ReadFromFile("11.png");

        ///*for (int i = 0; i < image._width * image._height; i++)
        //{
        //    image._pixels[i].x = std::clamp(image._pixels[i].x * 0.5f, 0.0f, 1.0f);
        //    image._pixels[i].y = std::clamp(image._pixels[i].y * 0.5f, 0.0f, 1.0f);
        //    image._pixels[i].z = std::clamp(image._pixels[i].z * 0.5f, 0.0f, 1.0f);
        //}*/

        /*for (int i = 0; i < 5; i++)
        {
            image.GaussianBlur5();
        }*/



        image.WritePNG("result.png");

        CreateDeviceAndSwapChain(); // _device, _deviceContext, _swapChain 생성
        CreateRenderTargetView(); // 백버퍼를 렌더링 대상으로 설정, gpu -> renderTargetView -> 백버퍼
        SetViewPort(); // 화면 설정

        CreateGeometry(); // cpu에서 도형(정점, 인덱스) 설정
        CreateVertexBuffer(); // cpu에 있는 정점 데이터를 vertexBuffer로 넘겨준다
        CreateIndexBuffer(); // cpu에 있는 인덱스 데이터를 indexBuffer로 넘겨준다

        CreateVertexShader(); // 셰이더 파일에서 버텍스 쉐이더 부분을 컴파일하여 vsBlob에 바이너리 파일로 저장한다
        CreateInputLayout(); // 정점 데이터를 어떻게 읽을지 알려준다 

        CreateRasterizerState(); // 도형 그리는 방식 설정

        CreatePixelShader(); // 셰이더 파일에서 픽셀 쉐이더 부분 컴파일해서 psBlob에 바이너리 파일로 저장한다
        CreateShaderResourceView(); // 텍스처 로드
        CreateSamplerState(); // 텍스처 샘플링 방식 설정
    }

    void Renderer::Update()
    {
        for (int i = 0; i < 5; i++)
        {
            image.GaussianBlur5();
        }

        D3D11_MAPPED_SUBRESOURCE subResource;
        ZeroMemory(&subResource, sizeof(subResource));

        // GPU 상수버퍼를 CPU가 쓸 수 있도록 열어준다

        //_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

        // CPU의 _transformData를 GPU 메모리 subResource로 복사한다
        // GPU가 새로운 offset 값 알게 된다
        //::memcpy(subResource.pData, &data, sizeof(data));

        // 데이터를 GPU로 옮긴다
        //_deviceContext->Unmap(_constantBuffer.Get(), 0);
    }

    void Renderer::Render() // 바이닝 : gpu 파이프라인의 슬롯에 연결
    {
        PreRender();

        uint32 stride = sizeof(VertexTexture); // 정점 하나의 크기
        uint32 offset = 0; // 버퍼 시작 오프셋

        //IA
        _deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
        _deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        _deviceContext->IASetInputLayout(_inputLayout.Get());
        _deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        //VS
        _deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);

        //RS
        _deviceContext->RSSetState(_rasterizerState.Get());

        //PS
        _deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
        _deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
        _deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf());

        //OM
        _deviceContext->DrawIndexed(static_cast<uint32>(_indices.size()), 0, 0);
        PostRender();
    }

    void Renderer::CreateDeviceAndSwapChain()
    {
        DXGI_SWAP_CHAIN_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        // 백버퍼 설정
        desc.BufferDesc.Width = screenWidth;
        desc.BufferDesc.Height = screenHeight;
        desc.BufferDesc.RefreshRate.Numerator = 60;       // 60fps
        desc.BufferDesc.RefreshRate.Denominator = 1;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 픽셀 포맷 R8G8B8A8
        desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        // 멀티샘플링 (AA) - 1,0 이면 끈 것
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        // 백버퍼 용도
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;        // 더블버퍼링이면 2

        desc.OutputWindow = _hwnd;
        desc.Windowed = TRUE;     // FALSE면 풀스크린
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        desc.Flags = 0;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,                    // 어댑터 (nullptr = 기본 GPU)
            D3D_DRIVER_TYPE_HARDWARE,   // GPU 사용 (SOFTWARE로 바꾸면 CPU로 렌더링)
            nullptr,                    // 소프트웨어 드라이버 핸들 (위에서 HARDWARE니까 nullptr)
            0,                      // 디버그 플래그
            nullptr,                    // 피처레벨 배열 (nullptr = 최고 레벨 자동 선택)
            0,                          // 피처레벨 배열 크기
            D3D11_SDK_VERSION,
            &desc,
            _swapChain.GetAddressOf(),
            _device.GetAddressOf(),
            nullptr,                    // 실제 선택된 피처레벨 반환 (필요없으면 nullptr)
            _deviceContext.GetAddressOf()
        );

        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateRenderTargetView()
    {
        ComPtr<ID3D11Texture2D> backBuffer;
        HRESULT hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
        assert(SUCCEEDED(hr));

        hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::SetViewPort()
    {
        _viewPort.TopLeftX = 0.f;
        _viewPort.TopLeftY = 0.f;
        _viewPort.Width = static_cast<float>(screenWidth);
        _viewPort.Height = static_cast<float>(screenHeight);
        _viewPort.MinDepth = 0.f;  // 가장 가까운 깊이
        _viewPort.MaxDepth = 1.f;  // 가장 먼 깊이
    }

    void Renderer::CreateGeometry()
    {
        _vertices.resize(4);

        //  1
        //0 2

        //vertex
        {
            _vertices[0].position = { -1.f, -1.f, 0.f };
            _vertices[0].uv = { 0.f,1.f };

            _vertices[1].position = { -1.f, 1.f, 0.f };
            _vertices[1].uv = { 0.f,0.f };

            _vertices[2].position = { 1.f, -1.f, 0.f };
            _vertices[2].uv = { 1.f,1.f };

            _vertices[3].position = { 1.f, 1.f, 0.f };
            _vertices[3].uv = { 1.f,0.f };
        }

        //13
        //02
        //index
        {
            _indices = { 0,1,2,2,1,3 };
        }
    }

    void Renderer::CreateVertexBuffer()
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = static_cast<uint32>(sizeof(VertexTexture) * _vertices.size());
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = _vertices.data();

        HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateIndexBuffer()
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = static_cast<uint32>(sizeof(uint32) * _indices.size());
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = _indices.data();

        HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateVertexShader()
    {
        LoadShaderFromFile(L"DefaultShader.hlsl", "VS", "vs_5_0", _vsBlob);

        HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateInputLayout()
    {
        vector<D3D11_INPUT_ELEMENT_DESC> layout =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        const int32 size = layout.size();
        HRESULT hr = _device->CreateInputLayout(layout.data(), size, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateRasterizerState()
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = false;
        desc.DepthClipEnable = true;

        HRESULT hr = _device->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreatePixelShader()
    {
        LoadShaderFromFile(L"DefaultShader.hlsl", "PS", "ps_5_0", _psBlob);

        HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateShaderResourceView()
    {
        ScratchImage image;
        ZeroMemory(&image, sizeof(image));

        TexMetadata data;
        ZeroMemory(&data, sizeof(data));


        //HRESULT hr = LoadFromWICFile(L"image_1.jpg", WIC_FLAGS_NONE, &data, image);
        HRESULT hr = LoadFromWICFile(L"result.png", WIC_FLAGS_NONE, &data, image);
        assert(SUCCEEDED(hr));

        hr = ::CreateShaderResourceView(_device.Get(), image.GetImages(), image.GetImageCount(), data, _shaderResourceView.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::CreateSamplerState()
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD = 0.0f;
        desc.MaxLOD = D3D11_FLOAT32_MAX;



        HRESULT hr = _device->CreateSamplerState(&desc, _samplerState.GetAddressOf());
        assert(SUCCEEDED(hr));
    }

    void Renderer::PreRender()
    {
        // clearColor로 화면 정리
        _deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);

        _deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);

        _deviceContext->RSSetViewports(1, &_viewPort);
    }

    void Renderer::PostRender()
    {
        _swapChain->Present(1, 0);
    }

    void Renderer::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
    {
        const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

        HRESULT hr = ::D3DCompileFromFile(
            path.c_str(),
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            name.c_str(),
            version.c_str(),
            compileFlag,
            0,
            blob.GetAddressOf(),
            nullptr);

        assert(SUCCEEDED(hr));
    }
}