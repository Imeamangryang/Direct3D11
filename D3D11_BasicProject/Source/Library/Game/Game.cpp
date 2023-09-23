#include "Game.h"

// --------------------------------------------------------------------------------
// ���� ����
// --------------------------------------------------------------------------------
HWND						g_hWnd = nullptr;
HINSTANCE					g_hInstance = nullptr;

LPCWSTR						g_pszWindowClassName = L"GGPWindowClass";
LPCWSTR						g_pszWindowName = L"Assignment 01";

D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device*               g_pd3dDevice = nullptr;
ID3D11Device1*              g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*        g_pImmediateContext = nullptr;
ID3D11DeviceContext1*       g_pImmediateContext1 = nullptr;

IDXGISwapChain*             g_pSwapChain = nullptr;
IDXGISwapChain1*            g_pSwapChain1 = nullptr;

ID3D11RenderTargetView*     g_pRenderTargetView = nullptr;

ID3D11VertexShader*         g_pVertexShader = nullptr;
ID3D11PixelShader*          g_pPixelShader = nullptr;

ID3D11Buffer*               g_pVertexBuffer = nullptr;
ID3D11Buffer*               g_pIndexBuffer = nullptr;

ID3D11InputLayout*          g_pVertexLayout = nullptr;

ID3D11Buffer*               g_pConstantBuffer = nullptr;

XMMATRIX					g_worldMatrix;
XMMATRIX					g_viewMatrix;
XMMATRIX					g_projectionMatrix;

ID3D11Texture2D*            g_pDepthStencil = nullptr;
ID3D11DepthStencilView*     g_pDepthStencilView = nullptr;

XMVECTOR at; // direction
XMVECTOR eye; // position
XMVECTOR up; // up

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
TODO: ���� ���� �߰�
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
InputDirections g_inputDirections;
MouseRelativeMovement g_mouseRelativeMovement;
BOOL g_mouseRightClick;

XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);;
XMVECTOR up_vector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);;

XMMATRIX camRotationMatrix;

XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR DefaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

float camYaw = 0.0f;
float camPitch = 0.0f;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;
float moveDownUp = 0.0f;

float speed = 5.0f;

RECT rc2;
POINT p1, p2;

// --------------------------------------------------------------------------------
// Resister window class, Create window, and Show window
// --------------------------------------------------------------------------------
HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
{
    WNDCLASSEX wcex =
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = LoadIcon(hInstance, (LPCTSTR) IDI_APPLICATION),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
        .lpszMenuName = nullptr,
        .lpszClassName = g_pszWindowClassName,
        .hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR) IDI_APPLICATION),
    };

    if (!RegisterClassEx(&wcex))
    {
        DWORD dwError = GetLastError();

        MessageBox(
            nullptr,
            L"Call to RegisterClassEx failed!",
            L"Game Graphics Programming",
            NULL
        );

        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
        {
            return HRESULT_FROM_WIN32(dwError);
        }

        return E_FAIL;
    }

    g_hInstance = hInstance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hWnd = CreateWindow(
        g_pszWindowClassName,
        g_pszWindowName,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,

        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hWnd)
    {
        DWORD dwError = GetLastError();

        MessageBox(
            nullptr,
            L"Call to CreateWindow failed!",
            L"Game Graphics Programming",
            NULL
        );

        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
        {
            return HRESULT_FROM_WIN32(dwError);
        }

        return E_FAIL;
    }

    ShowWindow(g_hWnd, nCmdShow);

    GetClientRect(g_hWnd, &rc);

    p1.x = rc.left;
    p1.y = rc.top;
    p2.x = rc.right;
    p2.y = rc.bottom;

    ClientToScreen(g_hWnd, &p1);
    ClientToScreen(g_hWnd, &p2);

    rc2.left = p1.x;
    rc2.top = p1.y;
    rc2.right = p2.x;
    rc2.bottom = p2.y;

    ClipCursor(&rc2);

    return S_OK;
}

// --------------------------------------------------------------------------------
// Called every time the application recevies a message
// --------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);

        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
     TODO: WM_INPUT case ���� (���콺 ��ġ)
     -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    case WM_INPUT:
        UINT dataSize;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first

        if (dataSize > 0)
        {
            std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
            {
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
                if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    //mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
                    g_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                    g_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
                }
            }
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    TODO: Ű����, ���콺 �� Ŭ�� DOWN/UP case ����
    -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    case WM_KEYDOWN:
        switch (wParam)
        {
            case 'W':
                g_inputDirections.bFront = true;
                return 0;

            case 'S':
                g_inputDirections.bBack = true;
                return 0;

            case 'A':
                g_inputDirections.bLeft = true;
                return 0;

            case 'D':
                g_inputDirections.bRight = true;
                return 0;

            case 'E':
                g_inputDirections.bUp = true;
                return 0;

            case 'Q':
                g_inputDirections.bDown = true;
                return 0;
        }

    case WM_KEYUP:
        switch (wParam)
        {
        case 'W':
            g_inputDirections.bFront = false;
            return 0;

        case 'S':
            g_inputDirections.bBack = false;
            return 0;

        case 'A':
            g_inputDirections.bLeft = false;
            return 0;

        case 'D':
            g_inputDirections.bRight = false;
            return 0;

        case 'E':
            g_inputDirections.bUp = false;
            return 0;

        case 'Q':
            g_inputDirections.bDown = false;
            return 0;

        }

    case WM_RBUTTONDOWN:
        g_mouseRightClick = true;
        return 0;

    case WM_RBUTTONUP:
        g_mouseRightClick = false;
        return 0;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// --------------------------------------------------------------------------------
// Create D3D devices and swap chain
// --------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    // D2D�� D3D�� ��ȣ ��� �����ϰ� ����.
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // Direct X 11.0������ D3D_FEATURE_LEVEL_11_1�� �ν����� ���� �� ����. �� ��� �ٽ� �������.

            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // DXGI Device�� ���� DXGI Factory�� �޾ƿ�.
    IDXGIFactory1* dxgiFactory = nullptr;
    IDXGIDevice* dxgiDevice = nullptr;

    hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (SUCCEEDED(hr))
    {
        IDXGIAdapter* adapter = nullptr;
        hr = dxgiDevice->GetAdapter(&adapter);
        if (SUCCEEDED(hr))
        {
            hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
            adapter->Release();
        }
        dxgiDevice->Release();
    }

    if (FAILED(hr))
        return hr;

    // Swap chain�� ����� ��.
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));

        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    }

    dxgiFactory->Release();
    
    if (FAILED(hr))
        return hr;
    
    // Back Buffer ����
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    // Viewport ����
    D3D11_VIEWPORT vp;
    {
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
    }
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Vertex Shader ������
    ID3DBlob* pVertexShaderBlob = nullptr;

    hr = CompileShaderFromFile(L"../Library/Ass01.fx", "VS", "vs_5_0", &pVertexShaderBlob);

    if (FAILED(hr))
    {
        MessageBox(
            nullptr,
            L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file.",
            L"Error",
            MB_OK
        );
        return hr;
    }

    // Vertex Shader ����
    hr = g_pd3dDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &g_pVertexShader);

    if (FAILED(hr))
    {
        pVertexShaderBlob->Release();
        return hr;
    }

    // input layout object ����
    D3D11_INPUT_ELEMENT_DESC layouts[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT uNumElements = ARRAYSIZE(layouts);

    hr = g_pd3dDevice->CreateInputLayout(
        layouts,
        uNumElements,
        pVertexShaderBlob->GetBufferPointer(),
        pVertexShaderBlob->GetBufferSize(),
        &g_pVertexLayout
    );

    pVertexShaderBlob->Release();

    if (FAILED(hr))
        return hr;

    // input layout object ���ε�
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

    // Pixel Shader ������
    ID3DBlob* pPixelShaderBlob = nullptr;

    hr = CompileShaderFromFile(L"../Library/Ass01.fx", "PS", "ps_5_0", &pPixelShaderBlob);
    if (FAILED(hr))
    {
        MessageBox(
            nullptr,
            L"The FX file cannot be compiled. Please run this executable from the directory that contains the FX file.",
            L"Error",
            MB_OK
        );
        return hr;
    }

    // Pixel Shader ����
    hr = g_pd3dDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    pPixelShaderBlob->Release();

    if (FAILED(hr))
        return hr;

    // Vertex buffer ����
    SimpleVertex sVertices[] =
    {
       {.Pos = XMFLOAT3(-1.0f,  1.0f, -1.0f),
          .Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        {.Pos = XMFLOAT3(1.0f,  1.0f, -1.0f),
          .Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        {.Pos = XMFLOAT3(1.0f,  1.0f,  1.0f),
          .Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
        {.Pos = XMFLOAT3(-1.0f,  1.0f,  1.0f),
          .Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        {.Pos = XMFLOAT3(-1.0f, -1.0f, -1.0f),
          .Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
        {.Pos = XMFLOAT3(1.0f, -1.0f, -1.0f),
          .Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        {.Pos = XMFLOAT3(1.0f, -1.0f,  1.0f),
          .Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        {.Pos = XMFLOAT3(-1.0f, -1.0f,  1.0f),
          .Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
    };

    D3D11_BUFFER_DESC bd = {};
    {
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex) * 8;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0; 
    }

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = sVertices;

    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);

    if (FAILED(hr))
        return hr;

    // Vertex buffer ���ε�
    UINT uStride = sizeof(SimpleVertex);
    UINT uOffset = 0;
    
    g_pImmediateContext->IASetVertexBuffers(
        0,
        1,
        &g_pVertexBuffer,
        &uStride,
        &uOffset
    );

    // Index buffer ����
    WORD sIndices[] = {
        3,1,0,
        2,1,3,
        0,5,4,
        1,5,0,
        3,4,7,
        0,4,3,
        1,6,5,
        2,6,1,
        2,7,6,
        3,7,2,
        6,4,5,
        7,4,6,
    };

    bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    initData = {};
    initData.pSysMem = sIndices;

    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pIndexBuffer);

    if (FAILED(hr))
        return hr;

    // Index buffer ���ε�
    g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // primitive type ����
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 

    // constant buffer ����
    bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);

    if (FAILED(hr))
        return hr;

    // world, view, projection Matrix ����
    g_worldMatrix = XMMatrixIdentity();

    eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    g_viewMatrix = XMMatrixLookAtLH(eye, at, up);
    g_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

    // Depth Stencil buffer ����
    D3D11_TEXTURE2D_DESC descDepth = {};
    {
        descDepth.Width = width;
        descDepth.Height = height;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
    }

    hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);

    if (FAILED(hr))
        return hr;

    // Depth Stencil buffer ���ε�
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    TODO: RAWINPUTDEVICE ����
    -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    static bool row_input_initialized = true;
    if (row_input_initialized == true)
    {
        RAWINPUTDEVICE rid;

        rid.usUsagePage = 0x01;
        rid.usUsage = 0x02;
        rid.dwFlags = 0;
        rid.hwndTarget = NULL;

        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        {
            exit(-1);
        }
    }

    return S_OK;
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
TODO: HandleInput() �Լ� ����
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void HandleInput(FLOAT deltaTime)
{
    if (g_inputDirections.bFront == true) {
        moveBackForward += speed * deltaTime;
    }
    
    if (g_inputDirections.bBack == true) {
        moveBackForward -= speed * deltaTime;
    }

    if (g_inputDirections.bRight == true) {
        moveLeftRight += speed * deltaTime;
    }

    if (g_inputDirections.bLeft == true) {
        moveLeftRight -= speed * deltaTime;
    }

    if (g_inputDirections.bUp == true) {
        moveDownUp += speed * deltaTime;
    }

    if (g_inputDirections.bDown == true) {
        moveDownUp -= speed * deltaTime;
    }

    if (g_mouseRightClick == true) {
        if (g_mouseRelativeMovement.X != 0)
        {
            camYaw += g_mouseRelativeMovement.X * deltaTime;
        }

        if (g_mouseRelativeMovement.Y != 0)
        {
            camPitch += g_mouseRelativeMovement.Y * deltaTime;
        }
    }
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
TODO: Update() �Լ� ����
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void Update(FLOAT deltaTime)
{
    // Rotate matrix
    camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    
    // Z���� ��� X
    at = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    at = XMVector3Normalize(at);

    up = XMVector3TransformCoord(DefaultUp, camRotationMatrix);
    up = XMVector3Normalize(up);

    XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(camYaw);

    // adapt rotate matrix
    right = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    up_vector = XMVector3TransformCoord(up_vector, RotateYTempMatrix);
    forward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    // convert position
    eye += moveLeftRight * right;
    eye += moveBackForward * forward;
    eye += moveDownUp * up_vector;

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;
    moveDownUp = 0.0f;

    // convert lookat
    at = eye + at;

    // update view matrix
    g_viewMatrix = XMMatrixLookAtLH(eye, at, up);
}

void Render()
{
    // Render Target View Clear
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);

    // Depth Stencil View Clear
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Constant Buffer ����
    ConstantBuffer cb1;
    cb1.World = XMMatrixTranspose(g_worldMatrix);
    cb1.View = XMMatrixTranspose(g_viewMatrix);
    cb1.Projection = XMMatrixTranspose(g_projectionMatrix);
    g_pImmediateContext->UpdateSubresource( g_pConstantBuffer,0, nullptr, &cb1,  0, 0);

    // Shader ����
    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
    g_pImmediateContext->DrawIndexed(36, 0, 0);

    // Present
    g_pSwapChain->Present(0, 0);
}

void CleanupDevice()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain1) g_pSwapChain1->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext1) g_pImmediateContext1->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice1) g_pd3dDevice1->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();

    if (g_pConstantBuffer) g_pConstantBuffer->Release();

    if (g_pDepthStencil) g_pDepthStencil->Release();
    if (g_pDepthStencilView) g_pDepthStencilView->Release();
}

HRESULT CompileShaderFromFile(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel, _Outptr_ ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined (DEBUG) || defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    
    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(pszFileName, nullptr, nullptr, pszEntryPoint, pszShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<PCSTR>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}