#include <windows.h>
#include <stdio.h>

#include <d3d11.h>	

#include <dwrite.h>
#include <dinput.h>
#include <d3dcompiler.h>			

#pragma warning( disable : 4838)
#include "xnamath.h"
#include "WICTextureLoader.h"

#pragma comment (lib, "d3d11.lib")	
#pragma comment (lib, "D3dcompiler.lib")	
#pragma comment (lib,"DXGI.lib")
#pragma comment (lib, "DirectXTK.lib")	


#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "dinput8.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variables
FILE *gpfile_SB = NULL;
char gszLogFileName_SB[] = "log.txt";

HWND ghwnd_SB = NULL;
DWORD dwStyle_SB;
WINDOWPLACEMENT wpPrev_SB = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow_SB = false;
bool gbEscapeKeyIsPressed_SB = false;
bool gbFullScreen_SB = false;
bool gbLight_SB = false;
bool gbRotation = true;

float gClearColor[4];
float angle_cube = 0.0f;
float ZAxis = 10.0f;
float blendValue = 1.0f;

//Camera setup

float time = 0.0f;
float speed = 1.0f;

XMMATRIX camRotationMatrix;

XMMATRIX camView;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;
XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;

//end cam setup
IDXGISwapChain *gpIDXGISwapChain_SB = NULL;
ID3D11Device *gpID3D11Device_SB = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext_SB = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView_SB = NULL;

//CUBE shaders
ID3D11VertexShader *gpID3D11VertexShaderCube1 = NULL;
ID3D11PixelShader *gpID3D11PixelShaderCube1 = NULL;

ID3D11VertexShader *gpID3D11VertexShaderCube2 = NULL;
ID3D11PixelShader *gpID3D11PixelShaderCube2 = NULL;

ID3D11VertexShader *gpID3D11VertexShaderCube3 = NULL;
ID3D11PixelShader *gpID3D11PixelShaderCube3 = NULL;

ID3D11VertexShader *gpID3D11VertexShaderCube4 = NULL;
ID3D11PixelShader *gpID3D11PixelShaderCube4 = NULL;



ID3D11Buffer *gpID3D11Buffer_VertexBuffer = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_Cube1 = NULL;
ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_Cube2 = NULL;
ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_Cube3 = NULL;
ID3D11ShaderResourceView *gpID3D11ShaderResourceView_Texture_Cube4 = NULL;

ID3D11SamplerState *gpID3D11SamplerState_Texture_Cube = NULL;

ID3D11BlendState* gpID3D11AlphaEnableBlendingState;
ID3D11BlendState* gpID3D11AlphaDisableBlendingState;

//Mouse

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;
IDirectInputDevice8* DIMouse;


struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR La;
	XMVECTOR Ld;
	XMVECTOR Ls;

	XMVECTOR Ka;
	XMVECTOR Kd;
	XMVECTOR Ks;

	XMVECTOR LightPosition;

	float MaterialShininess;

	unsigned int KeyPressed;


	float blend;

};

float cubeVertices[] =
{


	-1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
	// triangle 2 
	-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
	1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,



	1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
	1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,

	-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,
	1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,	1.0f, 1.0f,


	-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
	1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,

	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
	1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
	1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 1.0f,


	1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,

	-1.0f, -1.0f, 1.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,


	-1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,

	-1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
	-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,


	1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
	1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,

	1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
	1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
};

float LightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float LightPosition[] = { 100.0f, 100.0f, -100.0f, 1.0f };
float MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Shininess = 128.0f;

ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
ID3DBlob *pID3DBlob_Error = NULL;

XMMATRIX gPerspectiveProjectionMatrix;


void UpdateCamera();

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	bool InitDirectInput(HINSTANCE hInstance);

	// variable declarations
	WNDCLASSEX wndClass_SB;
	HWND hwnd_SB;
	MSG msg_SB;
	TCHAR szClassName_SB[] = TEXT("Direct3D11");
	bool bDone_SB = false;

	// code
	// create log file
	if (fopen_s(&gpfile_SB, gszLogFileName_SB, "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Cannot Be Created\n, Exitting From Application\n"), TEXT("ERROR!!!"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpfile_SB, "Log File is Created Successfully\n");
		fclose(gpfile_SB);
	}

	// initialize WNDCLASSEX structure
	wndClass_SB.cbSize = sizeof(WNDCLASSEX);
	wndClass_SB.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass_SB.cbClsExtra = 0;
	wndClass_SB.cbWndExtra = 0;
	wndClass_SB.lpfnWndProc = WndProc;
	wndClass_SB.lpszClassName = szClassName_SB;
	wndClass_SB.lpszMenuName = NULL;
	wndClass_SB.hInstance = hInstance;
	wndClass_SB.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass_SB.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass_SB.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass_SB.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register WNDCLASSEX structure
	RegisterClassEx(&wndClass_SB);

	// create window
	hwnd_SB = CreateWindow(szClassName_SB,
		TEXT("Direct3D11"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd_SB = hwnd_SB;

	ShowWindow(hwnd_SB, iCmdShow);
	SetForegroundWindow(hwnd_SB);
	SetFocus(hwnd_SB);

	// initialize D3D
	HRESULT hr;
	bool mouseInit = InitDirectInput(hInstance);
	if (!mouseInit)
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitDirectInput() failed.\n");
		fclose(gpfile_SB);
	}

	hr = initialize();
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "initialize() failed. Exitting Now...\n");
		fclose(gpfile_SB);
		DestroyWindow(hwnd_SB);
		hwnd_SB = NULL;
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "initialize() Successful.\n");
		fclose(gpfile_SB);
	}

	// message loop
	while (bDone_SB == false)
	{
		if (PeekMessage(&msg_SB, NULL, 0, 0, PM_REMOVE))
		{
			if (msg_SB.message == WM_QUIT)
				bDone_SB = true;
			else
			{
				TranslateMessage(&msg_SB);
				DispatchMessage(&msg_SB);
			}
		}
		else
		{
			// render
			if (gbActiveWindow_SB == true)
			{
				if (gbEscapeKeyIsPressed_SB == true)
					bDone_SB = true;
				// update should be called here
				update();

			}
			display();
		}
	}

	// clean up
	uninitialize();

	return((int)msg_SB.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declarations
	HRESULT resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);


	// variable declarations
	HRESULT hr;
	// code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)		//if 0, window is active
			gbActiveWindow_SB = true;
		else						// if non zero, window is inactive
			gbActiveWindow_SB = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		if (gpID3D11DeviceContext_SB)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
				fprintf_s(gpfile_SB, "resize() failed.\n");
				fclose(gpfile_SB);
				return(hr);
			}
			else
			{
				fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
				fprintf_s(gpfile_SB, "resize() Successful.\n");
				fclose(gpfile_SB);
			}
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{

		case VK_UP:
			blendValue += 0.05f;
			if (blendValue >= 1.0f)
			{
				blendValue = 1.0f;
			}
			break;

		case VK_DOWN:
			blendValue -= 0.05f;
			if (blendValue <= 0.0f)
			{
				blendValue = 0.0f;
			}
			break;

		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed_SB == false)
				gbEscapeKeyIsPressed_SB = true;
			break;

		case 0X46:
			if (gbFullScreen_SB == false)
			{
				ToggleFullscreen();
				gbFullScreen_SB = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullScreen_SB = false;
			}
			break;
			// A
		case 'A':
		case 'a':
			speed = 1.0f * time;

			moveLeftRight -= speed;
			break;

			// S
		case 'S':
		case 's':
			speed = 1.0f * time;
			//float speed = 15.0f * time;
			moveBackForward -= speed;
			break;

			// W
		case 'W':
		case 'w':
			speed = 1.0f * time;
			//float speedW = 15.0f * time;
			moveBackForward += speed;
			break;

			// D
		case 'D':
		case 'd':
			speed = 1.0f * time;
			//float speedD = 15.0f * time;
			moveLeftRight += speed;
			break;



		default:
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{

			case 'r':
			case 'R':
				gbRotation = !gbRotation;
				break;
			case 'l':
			case 'L':
				if (gbLight_SB == true)
					gbLight_SB = false;
				else
					gbLight_SB = true;
				break;
			default:
				break;
		}
		break;

	case WM_MOUSEMOVE:
	{
		DIMOUSESTATE mouseCurrState;

		DIMouse->Acquire();

		DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

		if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
		{
			camYaw += mouseLastState.lX * 0.001f;

			camPitch += mouseCurrState.lY * 0.001f;

			mouseLastState = mouseCurrState;
		}


		UpdateCamera();
	}
	break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CLOSE:
		uninitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// variable declarations
	MONITORINFO mi;
	if (gbFullScreen_SB == false)
	{
		dwStyle_SB = GetWindowLong(ghwnd_SB, GWL_STYLE);
		if (dwStyle_SB &WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd_SB, &wpPrev_SB) && GetMonitorInfo(MonitorFromWindow(ghwnd_SB, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd_SB, GWL_STYLE, dwStyle_SB & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd_SB, HWND_TOP, mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd_SB, GWL_STYLE, dwStyle_SB | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd_SB, &wpPrev_SB);
		SetWindowPos(ghwnd_SB, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

HRESULT initialize(void)
{
	// function declarations
	void uninitialize(void);
	HRESULT resize(int, int);
	HRESULT LoadD3DTexture(const wchar_t *, ID3D11ShaderResourceView **);

	HRESULT InitCube1Shader();
	HRESULT InitCube2Shader();
	HRESULT InitCube3Shader();
	HRESULT InitCube4Shader();

	// variable declarations
	HRESULT hr;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{ D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;
	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1;


	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd_SB;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];

		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			d3dDriverType,
			NULL,
			createDeviceFlags,
			&d3dFeatureLevel_required,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain_SB,
			&gpID3D11Device_SB,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext_SB
		);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3D11CreateDeviceAndSwapChain() failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3D11CreateDeviceAndSwapChain() Successful.\n");

		fprintf_s(gpfile_SB, "Chosen Driver Is Of ");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpfile_SB, "Hardware Type.\n ");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpfile_SB, "Warp Type.\n ");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpfile_SB, "Reference Type.\n ");
		}
		else
		{
			fprintf_s(gpfile_SB, "Unknown Type.\n ");
		}

		fprintf_s(gpfile_SB, "The Supported Highest Feature Level Is \n ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpfile_SB, "11.0 \n ");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpfile_SB, "10.1 \n ");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpfile_SB, "10.0 \n ");
		}
		else
		{
			fprintf_s(gpfile_SB, "Unknown Type.\n ");
		}
		fclose(gpfile_SB);
	}

	////// Init all Cube shaders ///////////
	//1st
	hr = InitCube1Shader();
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube1Shader Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube1Shader Succeeded.\n");
		fclose(gpfile_SB);
	}

	//2nd
	hr = InitCube2Shader();
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube2Shader Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube2Shader Succeeded.\n");
		fclose(gpfile_SB);
	}

	//3rd
	hr = InitCube3Shader();
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube3Shader Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube3Shader Succeeded.\n");
		fclose(gpfile_SB);
	}

	//4th 
	hr = InitCube4Shader();
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube4Shader Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "InitCube4Shader Succeeded.\n");
		fclose(gpfile_SB);
	}

	/////////////// create and set input layout //////////////////////////
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[4];

	// zero out the structure
	ZeroMemory(inputElementDesc, 4 * sizeof(D3D11_INPUT_ELEMENT_DESC));


	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InstanceDataStepRate = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	inputElementDesc[1].SemanticName = "COLOR";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InstanceDataStepRate = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].InputSlot = 2;
	inputElementDesc[2].AlignedByteOffset = 0;
	inputElementDesc[2].InstanceDataStepRate = 0;
	inputElementDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	inputElementDesc[3].SemanticName = "TEXCOORD";
	inputElementDesc[3].SemanticIndex = 0;
	inputElementDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[3].InputSlot = 3;
	inputElementDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[3].InstanceDataStepRate = 0;
	inputElementDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


	hr = gpID3D11Device_SB->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11DEVICE::CreateInputLayout() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11DEVICE::CreateInputLayout() Succeeded.\n");
		fclose(gpfile_SB);
	}


	gpID3D11DeviceContext_SB->IASetInputLayout(gpID3D11InputLayout);


	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;




	// create vertex buffer
	D3D11_BUFFER_DESC bufferDesc_VertexBuffer;


	ZeroMemory(&bufferDesc_VertexBuffer, sizeof(D3D11_BUFFER_DESC));


	bufferDesc_VertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc_VertexBuffer.ByteWidth = sizeof(float) * ARRAYSIZE(cubeVertices);
	bufferDesc_VertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_VertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	hr = gpID3D11Device_SB->CreateBuffer(&bufferDesc_VertexBuffer, NULL, &gpID3D11Buffer_VertexBuffer);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateBuffer() Failed For Cube Position Vertex Buffer.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateBuffer() Succeeded For Cube Position Vertex Buffer.\n");
		fclose(gpfile_SB);
	}


	D3D11_MAPPED_SUBRESOURCE mappedSubresource;


	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));


	gpID3D11DeviceContext_SB->Map(gpID3D11Buffer_VertexBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,

		&mappedSubresource);
	memcpy(mappedSubresource.pData, cubeVertices, sizeof(cubeVertices));

	gpID3D11DeviceContext_SB->Unmap(gpID3D11Buffer_VertexBuffer, 0);



	// constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;


	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));


	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;


	hr = gpID3D11Device_SB->CreateBuffer(&bufferDesc_ConstantBuffer, 0, &gpID3D11Buffer_ConstantBuffer);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateBuffer() Failed For Constant Buffer.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateBuffer() Succeeded For Constant Buffer.\n");
		fclose(gpfile_SB);
	}

	gpID3D11DeviceContext_SB->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);
	gpID3D11DeviceContext_SB->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);



	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = gpID3D11Device_SB->CreateBlendState(&blendStateDescription, &gpID3D11AlphaEnableBlendingState);
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateBlendState() Failed For blending enable.\n");
		fclose(gpfile_SB);
		return(hr);
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	hr = gpID3D11Device_SB->CreateBlendState(&blendStateDescription, &gpID3D11AlphaDisableBlendingState);
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateBlendState() Failed For blending disable.\n");
		fclose(gpfile_SB);
		return(hr);
	}

	// RASTERIZER STATE 


	D3D11_RASTERIZER_DESC rasterizerDesc;


	ZeroMemory((void *)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));


	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;


	hr = gpID3D11Device_SB->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateRasterizerState() Failed For Culling.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateRasterizerState() Succedeed For Culling.\n");
		fclose(gpfile_SB);
	}


	gpID3D11DeviceContext_SB->RSSetState(gpID3D11RasterizerState);

	//  Texture Resource 
	hr = LoadD3DTexture(L"furr.bmp", &gpID3D11ShaderResourceView_Texture_Cube1);

	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Failed For cube1 texture.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Succeeded For cube1 texture.\n");
		fclose(gpfile_SB);
	}

	//cube2
	hr = LoadD3DTexture(L"mat.bmp", &gpID3D11ShaderResourceView_Texture_Cube2);

	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Failed For cube2 texture.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Succeeded For cube2 texture.\n");
		fclose(gpfile_SB);
	}

	//cube3
	hr = LoadD3DTexture(L"marble.bmp", &gpID3D11ShaderResourceView_Texture_Cube3);

	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Failed For cube3 texture.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Succeeded For cube3 texture.\n");
		fclose(gpfile_SB);
	}

	//cube4

	hr = LoadD3DTexture(L"marble.bmp", &gpID3D11ShaderResourceView_Texture_Cube4);

	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Failed For cube4 texture.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nLoadD3DTexture() Succeeded For cube4 texture.\n");
		fclose(gpfile_SB);
	}

	D3D11_SAMPLER_DESC samplerDesc;


	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));


	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;


	hr = gpID3D11Device_SB->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState_Texture_Cube);

	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nID3D11Device::CreateSamplerState() Failed .\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nID3D11Device::CreateSamplerState() Succeeded .\n");
		fclose(gpfile_SB);
	}


	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	//set projection matrix to identity matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();


	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "resize() failed.\n ");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "resize() successful.\n ");
		fclose(gpfile_SB);
	}

	return(S_OK);
}

HRESULT LoadD3DTexture(const wchar_t *textureFileName, ID3D11ShaderResourceView **ppID3D11ShaderResourceView)
{
	// code
	HRESULT hr;

	// create texture
	hr = DirectX::CreateWICTextureFromFile(gpID3D11Device_SB, gpID3D11DeviceContext_SB, textureFileName, NULL, ppID3D11ShaderResourceView);

	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "\nCreateWICTextureFromFile() failed.\n ");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "CreateWICTextureFromFile() successful.\n ");
		fclose(gpfile_SB);
	}

	return(hr);
}

HRESULT resize(int width, int height)
{
	// code
	HRESULT hr = S_OK;

	// free any sizedependent resources
	if (gpID3D11RenderTargetView_SB)
	{
		gpID3D11RenderTargetView_SB->Release();
		gpID3D11RenderTargetView_SB = NULL;
	}

	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}


	gpIDXGISwapChain_SB->ResizeBuffers(1, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);


	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain_SB->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	hr = gpID3D11Device_SB->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView_SB);
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "CreateRenderTargetView() failed.\n ");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "CreateRenderTargetView() successful.\n ");
		fclose(gpfile_SB);
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC textureDesc;

	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;

	// create texture buffer as depth buffer
	gpID3D11Device_SB->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);

	// create depth stencil view from above depth stencil buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	// zero out the memory
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	// fill the structure members
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	// create depth stencil view
	hr = gpID3D11Device_SB->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc, &gpID3D11DepthStencilView);

	// error checking
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateDepthStencilView() failed.\n ");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device::CreateDepthStencilView() successful.\n ");
		fclose(gpfile_SB);
	}
	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	// set render target view as render target with depth stencil view
	gpID3D11DeviceContext_SB->OMSetRenderTargets(1, &gpID3D11RenderTargetView_SB, gpID3D11DepthStencilView);

	//set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext_SB->RSSetViewports(1, &d3dViewPort);

	// set perspective matrix
	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	return(hr);
}

void update(void)
{
	if (gbRotation)
	{
		angle_cube = angle_cube + 0.001f;
		if (angle_cube > 360.0f)
		{
			angle_cube = 0.0f;
		}
	}
}

void display(void)
{
	void ToggleAlphaState(ID3D11BlendState *m_alphaBlendingState);

	//
	gpID3D11DeviceContext_SB->ClearRenderTargetView(gpID3D11RenderTargetView_SB, gClearColor);


	gpID3D11DeviceContext_SB->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	//set cube 1
	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube1, NULL, NULL);
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube1, NULL, NULL);

	// bind texture and sampler
	gpID3D11DeviceContext_SB->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_Cube1);
	gpID3D11DeviceContext_SB->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_Cube);

	UINT stride = sizeof(float) * 11;
	UINT offset = sizeof(float) * 0;

	gpID3D11DeviceContext_SB->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer, &stride, &offset);

	stride = sizeof(float) * 11;
	offset = sizeof(float) * 3;

	gpID3D11DeviceContext_SB->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer, &stride, &offset);

	stride = sizeof(float) * 11;
	offset = sizeof(float) * 6;

	gpID3D11DeviceContext_SB->IASetVertexBuffers(2, 1, &gpID3D11Buffer_VertexBuffer, &stride, &offset);

	stride = sizeof(float) * 11;
	offset = sizeof(float) * 9;

	gpID3D11DeviceContext_SB->IASetVertexBuffers(3, 1, &gpID3D11Buffer_VertexBuffer, &stride, &offset);

	//Matrix calculations
	XMMATRIX r1 = XMMatrixIdentity();
	XMMATRIX r2 = XMMatrixIdentity();
	XMMATRIX r3 = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();


	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	viewMatrix = camView;

	// select geometry primitive
	gpID3D11DeviceContext_SB->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	translationMatrix = XMMatrixTranslation(-4.0f, 0.0f, ZAxis);
	r1 = XMMatrixRotationX(angle_cube);
	r2 = XMMatrixRotationY(angle_cube);
	r3 = XMMatrixRotationZ(angle_cube);
	rotationMatrix = r1 * r2 * r3;
	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	// load the data into constant buffer
	CBUFFER constantBuffer;

	if (gbLight_SB == true)
	{
		constantBuffer.La = XMVectorSet(LightAmbient[0], LightAmbient[1], LightAmbient[2], LightAmbient[3]);
		constantBuffer.Ld = XMVectorSet(LightDiffuse[0], LightDiffuse[1], LightDiffuse[2], LightDiffuse[3]);
		constantBuffer.Ls = XMVectorSet(LightSpecular[0], LightSpecular[1], LightSpecular[2], LightSpecular[3]);


		constantBuffer.Ka = XMVectorSet(MaterialAmbient[0], MaterialAmbient[1], MaterialAmbient[2], MaterialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(MaterialDiffuse[0], MaterialDiffuse[1], MaterialDiffuse[2], MaterialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(MaterialSpecular[0], MaterialSpecular[1], MaterialSpecular[2], MaterialSpecular[3]);

		constantBuffer.MaterialShininess = Shininess;


		constantBuffer.LightPosition = XMVectorSet(100.0, 100.0, -100.0, 1.0);
		constantBuffer.KeyPressed = 1;

		constantBuffer.blend = 1.0f;

	}
	else
	{
		constantBuffer.KeyPressed = 0;
		constantBuffer.blend = 1.0f;
	}

	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = camView;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext_SB->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
		0,
		NULL,
		&constantBuffer,
		0,
		0);

	// draw vertex buffer
	gpID3D11DeviceContext_SB->Draw(6, 0);	// 6 vertices from 0 to 6
	gpID3D11DeviceContext_SB->Draw(6, 6);	// 6 vertices from 0 to 12
	gpID3D11DeviceContext_SB->Draw(6, 12);	// 6 vertices from 0 to 18
	gpID3D11DeviceContext_SB->Draw(6, 18);	// 6 vertices from 0 to 24
	gpID3D11DeviceContext_SB->Draw(6, 24);	// 6 vertices from 0 to 30
	gpID3D11DeviceContext_SB->Draw(6, 30);	// 6 vertices from 0 to 36

	/////////////// 2nd cube /////////////////////

	//set cube 2
	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube2, NULL, NULL);
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube2, NULL, NULL);

	// bind texture and sampler
	gpID3D11DeviceContext_SB->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_Cube2);
	gpID3D11DeviceContext_SB->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_Cube);

	r1 = XMMatrixIdentity();
	r2 = XMMatrixIdentity();
	r3 = XMMatrixIdentity();
	scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();


	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();

	viewMatrix = camView;

	translationMatrix = XMMatrixTranslation(-1.0f, 0.0f, ZAxis);
	r1 = XMMatrixRotationX(angle_cube);
	r2 = XMMatrixRotationY(angle_cube);
	r3 = XMMatrixRotationZ(angle_cube);
	rotationMatrix = r1 * r2 * r3;
	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = camView;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext_SB->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
		0,
		NULL,
		&constantBuffer,
		0,
		0);

	gpID3D11DeviceContext_SB->Draw(6, 0);	// 6 vertices from 0 to 6
	gpID3D11DeviceContext_SB->Draw(6, 6);	// 6 vertices from 0 to 12
	gpID3D11DeviceContext_SB->Draw(6, 12);	// 6 vertices from 0 to 18
	gpID3D11DeviceContext_SB->Draw(6, 18);	// 6 vertices from 0 to 24
	gpID3D11DeviceContext_SB->Draw(6, 24);	// 6 vertices from 0 to 30
	gpID3D11DeviceContext_SB->Draw(6, 30);	// 6 vertices from 0 to 36

	////////////////// 3rd cube ///////////////

	//set cube 3
	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube3, NULL, NULL);
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube3, NULL, NULL);

	// bind texture and sampler
	gpID3D11DeviceContext_SB->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_Cube3);
	gpID3D11DeviceContext_SB->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_Cube);

	r1 = XMMatrixIdentity();
	r2 = XMMatrixIdentity();
	r3 = XMMatrixIdentity();
	scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();


	worldMatrix = XMMatrixIdentity();
	//viewMatrix = XMMatrixIdentity();

	viewMatrix = camView;

	translationMatrix = XMMatrixTranslation(2.0f, 0.0f, ZAxis);
	r1 = XMMatrixRotationX(angle_cube);
	r2 = XMMatrixRotationY(angle_cube);
	r3 = XMMatrixRotationZ(angle_cube);
	rotationMatrix = r1 * r2 * r3;
	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = camView;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext_SB->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
		0,
		NULL,
		&constantBuffer,
		0,
		0);

	gpID3D11DeviceContext_SB->Draw(6, 0);	// 6 vertices from 0 to 6
	gpID3D11DeviceContext_SB->Draw(6, 6);	// 6 vertices from 0 to 12
	gpID3D11DeviceContext_SB->Draw(6, 12);	// 6 vertices from 0 to 18
	gpID3D11DeviceContext_SB->Draw(6, 18);	// 6 vertices from 0 to 24
	gpID3D11DeviceContext_SB->Draw(6, 24);	// 6 vertices from 0 to 30
	gpID3D11DeviceContext_SB->Draw(6, 30);	// 6 vertices from 0 to 36

	/////////////// 4th cube //////////////////////////////////////////////////

	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube4, NULL, NULL);
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube4, NULL, NULL);

	ToggleAlphaState(gpID3D11AlphaEnableBlendingState);

	r1 = XMMatrixIdentity();
	r2 = XMMatrixIdentity();
	r3 = XMMatrixIdentity();
	scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();


	worldMatrix = XMMatrixIdentity();
	//viewMatrix = XMMatrixIdentity();

	viewMatrix = camView;

	// bind texture and sampler
	//gpID3D11DeviceContext_SB->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture_Cube2);
	//gpID3D11DeviceContext_SB->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture_Cube);


	//update constant buffer
	//CBUFFER constantBuffer;

	if (gbLight_SB == true)
	{
		constantBuffer.La = XMVectorSet(LightAmbient[0], LightAmbient[1], LightAmbient[2], LightAmbient[3]);
		constantBuffer.Ld = XMVectorSet(LightDiffuse[0], LightDiffuse[1], LightDiffuse[2], LightDiffuse[3]);
		constantBuffer.Ls = XMVectorSet(LightSpecular[0], LightSpecular[1], LightSpecular[2], LightSpecular[3]);


		constantBuffer.Ka = XMVectorSet(MaterialAmbient[0], MaterialAmbient[1], MaterialAmbient[2], MaterialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(MaterialDiffuse[0], MaterialDiffuse[1], MaterialDiffuse[2], MaterialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(MaterialSpecular[0], MaterialSpecular[1], MaterialSpecular[2], MaterialSpecular[3]);

		constantBuffer.MaterialShininess = Shininess;


		constantBuffer.LightPosition = XMVectorSet(100.0, 100.0, -100.0, 1.0);
		constantBuffer.KeyPressed = 1;
	}
	else
	{
		constantBuffer.KeyPressed = 0;
	}

	translationMatrix = XMMatrixTranslation(5.0f, 0.0f, ZAxis);
	r1 = XMMatrixRotationX(angle_cube);
	r2 = XMMatrixRotationY(angle_cube);
	r3 = XMMatrixRotationZ(angle_cube);
	rotationMatrix = r1 * r2 * r3;
	scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	constantBuffer.blend = blendValue;
	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = camView;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	gpID3D11DeviceContext_SB->UpdateSubresource(gpID3D11Buffer_ConstantBuffer,
												0,
												NULL,
												&constantBuffer,
												0,
												0);

	gpID3D11DeviceContext_SB->Draw(6, 0);	// 6 vertices from 0 to 6
	gpID3D11DeviceContext_SB->Draw(6, 6);	// 6 vertices from 0 to 12
	gpID3D11DeviceContext_SB->Draw(6, 12);	// 6 vertices from 0 to 18
	gpID3D11DeviceContext_SB->Draw(6, 18);	// 6 vertices from 0 to 24
	gpID3D11DeviceContext_SB->Draw(6, 24);	// 6 vertices from 0 to 30
	gpID3D11DeviceContext_SB->Draw(6, 30);	// 6 vertices from 0 to 36


	//disable the blending
	ToggleAlphaState(gpID3D11AlphaDisableBlendingState);
	// switch between front and back buffers
	gpIDXGISwapChain_SB->Present(0, 0);

	
	time += 0.0001f;
	if (time >= 10000.0f)
	{
		time -= 0.0001f;
	}

	if (time < 0.0f)
	{
		time += 0.0001f;		
	}

	UpdateCamera();
}

void uninitialize(void)
{

	if (gpID3D11SamplerState_Texture_Cube)
	{
		gpID3D11SamplerState_Texture_Cube->Release();
		gpID3D11SamplerState_Texture_Cube = NULL;
	}

	//cube1
	if (gpID3D11ShaderResourceView_Texture_Cube1)
	{
		gpID3D11ShaderResourceView_Texture_Cube1->Release();
		gpID3D11ShaderResourceView_Texture_Cube1 = NULL;
	}

	//cube2
	if (gpID3D11ShaderResourceView_Texture_Cube2)
	{
		gpID3D11ShaderResourceView_Texture_Cube2->Release();
		gpID3D11ShaderResourceView_Texture_Cube2 = NULL;
	}

	//cube3
	if (gpID3D11ShaderResourceView_Texture_Cube3)
	{
		gpID3D11ShaderResourceView_Texture_Cube3->Release();
		gpID3D11ShaderResourceView_Texture_Cube3 = NULL;
	}

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer)
	{
		gpID3D11Buffer_VertexBuffer->Release();
		gpID3D11Buffer_VertexBuffer = NULL;
	}

	//cube1
	if (gpID3D11PixelShaderCube1)
	{
		gpID3D11PixelShaderCube1->Release();
		gpID3D11PixelShaderCube1 = NULL;
	}

	if (gpID3D11VertexShaderCube1)
	{
		gpID3D11VertexShaderCube1->Release();
		gpID3D11VertexShaderCube1 = NULL;
	}

	//Cube2
	if (gpID3D11PixelShaderCube2)
	{
		gpID3D11PixelShaderCube2->Release();
		gpID3D11PixelShaderCube2 = NULL;
	}

	if (gpID3D11VertexShaderCube2)
	{
		gpID3D11VertexShaderCube2->Release();
		gpID3D11VertexShaderCube2 = NULL;
	}

	//Cube3
	if (gpID3D11PixelShaderCube3)
	{
		gpID3D11PixelShaderCube3->Release();
		gpID3D11PixelShaderCube3 = NULL;
	}

	if (gpID3D11VertexShaderCube3)
	{
		gpID3D11VertexShaderCube3->Release();
		gpID3D11VertexShaderCube3 = NULL;
	}


	if (gpID3D11RenderTargetView_SB)
	{
		gpID3D11RenderTargetView_SB->Release();
		gpID3D11RenderTargetView_SB = NULL;
	}

	if (gpIDXGISwapChain_SB)
	{
		gpIDXGISwapChain_SB->Release();
		gpIDXGISwapChain_SB = NULL;
	}

	if (gpID3D11DeviceContext_SB)
	{
		gpID3D11DeviceContext_SB->Release();
		gpID3D11DeviceContext_SB = NULL;
	}

	if (gpID3D11Device_SB)
	{
		gpID3D11Device_SB->Release();
		gpID3D11Device_SB = NULL;
	}

	if (gpfile_SB)
	{

		fclose(gpfile_SB);
	}
}


//Init cube1
HRESULT InitCube1Shader()
{

	HRESULT hr;

	// Vertex Shader Source Code
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"   float blend;" \
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float4 color : COLOR, float4 normal : NORMAL, float2 texcoord : TEXCOORD)" \
		"{																" \
		"	vertex_output output;"	\
		"   pos.w = blend;" \
		"	if(keyPressed == 1)"	\
		"	{"	\
		"		float4 eyeCoordinates = mul(worldMatrix, pos);"	\
		"		eyeCoordinates = mul(viewMatrix, eyeCoordinates);"	\
		"		output.tn = (mul((float3x3)worldMatrix, (float3)normal));"	\
		"		output.ld = (float3)(lightPosition - eyeCoordinates);"	\
		"		output.vv = (-eyeCoordinates.xyz);"	\
		"	}"	\
		"	float4 Position = mul(worldMatrix, pos);"	\
		"	Position = mul(viewMatrix, Position);"	\
		"	Position = mul(projectionMatrix, Position);"	\
		"	output.position = Position;" \
		"	output.color = color;" \
		"	output.texcoord = texcoord;" \
		"	return(output);											" \
		"}																";


	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_Error = NULL;


	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
			fprintf_s(gpfile_SB, "D3DCompile() Failed For Vertex Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpfile_SB);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Vertex Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create vertex shader object
	hr = gpID3D11Device_SB->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShaderCube1);

	// Create vertex shader error checking
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Succeeded.\n");
		fclose(gpfile_SB);
	}


	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube1, NULL, NULL);



	// Pixel shader Source code
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};"
		"	Texture2D myTexture2D;" \
		"	SamplerState mySamplerState;" \
		"	float4 main(vertex_output input) : SV_TARGET				" \
		"	{											" \
		"		float4 phong_ads_light; " \
		"		if (keyPressed == 1)" \
		"		{" \
		"			float3 norm_tn = normalize(input.tn);" \
		"			float3 norm_ld = normalize(input.ld);" \
		"			float3 norm_vv = normalize(input.vv);" \
		"			float3 ref_vec = normalize(reflect(-norm_ld, norm_tn));" \
		"			float tn_dot_ld = max(dot(norm_tn, norm_ld), 0.0);" \
		"			float rv_dot_vv = max(dot(norm_vv, ref_vec), 0.0);" \
		"			float4 ambient = la * ka;" \
		"			float4 diffuse = ld * kd * tn_dot_ld;"
		"			float4 specular = ls * ks * pow(rv_dot_vv, materialShininess);"
		"			phong_ads_light = ambient+diffuse+specular;" \
		"		}"
		"		else" \
		"		{" \
		"			phong_ads_light = float4(1.0,1.0,1.0,1.0);" \
		"		}" \
		"		float4 texcolor = myTexture2D.Sample(mySamplerState, input.texcoord);				" \
		"		return(texcolor  * phong_ads_light);	" \
		"	}											";

	// variables to hold errors and source code
	pID3DBlob_PixelShaderCode;
	pID3DBlob_Error = NULL;

	// Compile Pixel shader
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Failed For Pixel Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpfile_SB);
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Pixel Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create Pixel shader object
	hr = gpID3D11Device_SB->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShaderCube1);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Succeeded.\n");
		fclose(gpfile_SB);
	}

	// Set pixel shader to the pipeline
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube1, 0, 0);

	return hr;
}

//Init cube2
HRESULT InitCube2Shader()
{
	HRESULT hr;

	// Vertex Shader Source Code
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float4 color : COLOR, float4 normal : NORMAL, float2 texcoord : TEXCOORD)" \
		"{																" \
		"	vertex_output output;"	\
		"	if(keyPressed == 1)"	\
		"	{"	\
		"		float4 eyeCoordinates = mul(worldMatrix, pos);"	\
		"		eyeCoordinates = mul(viewMatrix, eyeCoordinates);"	\
		"		output.tn = (mul((float3x3)worldMatrix, (float3)normal));"	\
		"		output.ld = (float3)(lightPosition - eyeCoordinates);"	\
		"		output.vv = (-eyeCoordinates.xyz);"	\
		"	}"	\
		"	float4 Position = mul(worldMatrix, pos);"	\
		"	Position = mul(viewMatrix, Position);"	\
		"	Position = mul(projectionMatrix, Position);"	\
		"	output.position = Position;" \
		"	output.color = color;" \
		"	output.texcoord = texcoord;" \
		"	return(output);											" \
		"}																";


	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_Error = NULL;


	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
			fprintf_s(gpfile_SB, "D3DCompile() Failed For Vertex Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpfile_SB);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Vertex Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create vertex shader object
	hr = gpID3D11Device_SB->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShaderCube2);

	// Create vertex shader error checking
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Succeeded.\n");
		fclose(gpfile_SB);
	}


	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube2, NULL, NULL);



	// Pixel shader Source code
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};"
		"	Texture2D myTexture2D;" \
		"	SamplerState mySamplerState;" \
		"	float4 main(vertex_output input) : SV_TARGET				" \
		"	{											" \
		"		float4 phong_ads_light; " \
		"		if (keyPressed == 1)" \
		"		{" \
		"			float3 norm_tn = normalize(input.tn);" \
		"			float3 norm_ld = normalize(input.ld);" \
		"			float3 norm_vv = normalize(input.vv);" \
		"			float3 ref_vec = normalize(reflect(-norm_ld, norm_tn));" \
		"			float tn_dot_ld = max(dot(norm_tn, norm_ld), 0.0);" \
		"			float rv_dot_vv = max(dot(norm_vv, ref_vec), 0.0);" \
		"			float4 ambient = la * ka;" \
		"			float4 diffuse = ld * kd * tn_dot_ld;"
		"			float4 specular = ls * ks * pow(rv_dot_vv, materialShininess);"
		"			phong_ads_light = ambient+diffuse+specular;" \
		"		}"
		"		else" \
		"		{" \
		"			phong_ads_light = float4(1.0,1.0,1.0,1.0);" \
		"		}" \
		"		float4 texcolor = myTexture2D.Sample(mySamplerState, input.texcoord);				" \
		"		return(texcolor * phong_ads_light);	" \
		"	}											";

	// variables to hold errors and source code
	pID3DBlob_PixelShaderCode;
	pID3DBlob_Error = NULL;

	// Compile Pixel shader
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Failed For Pixel Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpfile_SB);
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Pixel Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create Pixel shader object
	hr = gpID3D11Device_SB->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShaderCube2);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Succeeded.\n");
		fclose(gpfile_SB);
	}

	// Set pixel shader to the pipeline
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube2, 0, 0);

	return hr;
}

//Init cube3
HRESULT InitCube3Shader()
{
	HRESULT hr;

	// Vertex Shader Source Code
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float4 color : COLOR, float4 normal : NORMAL, float2 texcoord : TEXCOORD)" \
		"{																" \
		"	vertex_output output;"	\
		"	if(keyPressed == 1)"	\
		"	{"	\
		"		float4 eyeCoordinates = mul(worldMatrix, pos);"	\
		"		eyeCoordinates = mul(viewMatrix, eyeCoordinates);"	\
		"		output.tn = (mul((float3x3)worldMatrix, (float3)normal));"	\
		"		output.ld = (float3)(lightPosition - eyeCoordinates);"	\
		"		output.vv = (-eyeCoordinates.xyz);"	\
		"	}"	\
		"	float4 Position = mul(worldMatrix, pos);"	\
		"	Position = mul(viewMatrix, Position);"	\
		"	Position = mul(projectionMatrix, Position);"	\
		"	output.position = Position;" \
		"	output.color = color;" \
		"	output.texcoord = texcoord;" \
		"	return(output);											" \
		"}																";


	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_Error = NULL;


	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
			fprintf_s(gpfile_SB, "D3DCompile() Failed For Vertex Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpfile_SB);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Vertex Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create vertex shader object
	hr = gpID3D11Device_SB->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShaderCube3);

	// Create vertex shader error checking
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Succeeded.\n");
		fclose(gpfile_SB);
	}


	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube3, NULL, NULL);



	// Pixel shader Source code
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};"
		"	Texture2D myTexture2D;" \
		"	SamplerState mySamplerState;" \
		"	float4 main(vertex_output input) : SV_TARGET				" \
		"	{											" \
		"		float4 phong_ads_light; " \
		"		if (keyPressed == 1)" \
		"		{" \
		"			float3 norm_tn = normalize(input.tn);" \
		"			float3 norm_ld = normalize(input.ld);" \
		"			float3 norm_vv = normalize(input.vv);" \
		"			float3 ref_vec = normalize(reflect(-norm_ld, norm_tn));" \
		"			float tn_dot_ld = max(dot(norm_tn, norm_ld), 0.0);" \
		"			float rv_dot_vv = max(dot(norm_vv, ref_vec), 0.0);" \
		"			float4 ambient = la * ka;" \
		"			float4 diffuse = ld * kd * tn_dot_ld;"
		"			float4 specular = ls * ks * pow(rv_dot_vv, materialShininess);"
		"			phong_ads_light = ambient+diffuse+specular;" \
		"		}"
		"		else" \
		"		{" \
		"			phong_ads_light = float4(1.0,1.0,1.0,1.0);" \
		"		}" \
		"		float4 texcolor = myTexture2D.Sample(mySamplerState, input.texcoord);				" \
		"		return(texcolor * input.color * phong_ads_light);	" \
		"	}											";

	// variables to hold errors and source code
	pID3DBlob_PixelShaderCode;
	pID3DBlob_Error = NULL;

	// Compile Pixel shader
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Failed For Pixel Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpfile_SB);
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Pixel Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create Pixel shader object
	hr = gpID3D11Device_SB->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShaderCube3);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Succeeded.\n");
		fclose(gpfile_SB);
	}

	// Set pixel shader to the pipeline
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube3, 0, 0);

	return hr;
}


//Init transparentCube
HRESULT InitCube4Shader()
{
	HRESULT hr;

	// Vertex Shader Source Code
	const char *vertexShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"   float blend;" \

		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};" \
		"vertex_output main(float4 pos : POSITION, float4 color : COLOR, float4 normal : NORMAL, float2 texcoord : TEXCOORD)" \
		"{																" \
		"	vertex_output output;"	\
		"	if(keyPressed == 1)"	\
		"	{"	\
		"		float4 eyeCoordinates = mul(worldMatrix, pos);"	\
		"		eyeCoordinates = mul(viewMatrix, eyeCoordinates);"	\
		"		output.tn = (mul((float3x3)worldMatrix, (float3)normal));"	\
		"		output.ld = (float3)(lightPosition - eyeCoordinates);"	\
		"		output.vv = (-eyeCoordinates.xyz);"	\
		"	}"	\
		"	float4 Position = mul(worldMatrix, pos);"	\
		"	Position = mul(viewMatrix, Position);"	\
		"	Position = mul(projectionMatrix, Position);"	\
		"	output.position = Position;" \
		"	output.color = color;" \
		"	output.texcoord = texcoord;" \
		"	return(output);											" \
		"}																";


	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_Error = NULL;


	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
			fprintf_s(gpfile_SB, "D3DCompile() Failed For Vertex Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
			fclose(gpfile_SB);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Vertex Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create vertex shader object
	hr = gpID3D11Device_SB->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShaderCube4);

	// Create vertex shader error checking
	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreateVertexShader() Succeeded.\n");
		fclose(gpfile_SB);
	}


	gpID3D11DeviceContext_SB->VSSetShader(gpID3D11VertexShaderCube4, NULL, NULL);



	// Pixel shader Source code
	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer											" \
		"{																" \
		"	float4x4 worldMatrix;							" \
		"	float4x4 viewMatrix;							" \
		"	float4x4 projectionMatrix;" \
		"	float4 la;"	\
		"	float4 ld;"	\
		"	float4 ls;"	\
		"	float4 ka;"	\
		"	float4 kd;"	\
		"	float4 ks;"	\
		"	float4 lightPosition;"	\
		"	float materialShininess;"	\
		"	uint keyPressed;"	\
		"   float blend;" \
		"}" \
		"struct vertex_output	" \
		"{"\
		"	float4 position : SV_POSITION;" \
		"	float4 color : COLOR;" \
		"	float2 texcoord : TEXCOORD;"
		"	float3 tn : NORMAL0;" \
		"	float3 ld : NORMAL1;" \
		"	float3 vv : NORMAL2;" \
		"};"
		"	Texture2D myTexture2D;" \
		"	SamplerState mySamplerState;" \
		"	float4 main(vertex_output input) : SV_TARGET				" \
		"	{											" \
		"		float4 phong_ads_light; " \
		"		if (keyPressed == 1)" \
		"		{" \
		"			float3 norm_tn = normalize(input.tn);" \
		"			float3 norm_ld = normalize(input.ld);" \
		"			float3 norm_vv = normalize(input.vv);" \
		"			float3 ref_vec = normalize(reflect(-norm_ld, norm_tn));" \
		"			float tn_dot_ld = max(dot(norm_tn, norm_ld), 0.0);" \
		"			float rv_dot_vv = max(dot(norm_vv, ref_vec), 0.0);" \
		"			float4 ambient = la * ka;" \
		"			float4 diffuse = ld * kd * tn_dot_ld;"
		"			float4 specular = ls * ks * pow(rv_dot_vv, materialShininess);"
		"			phong_ads_light = ambient+diffuse+specular;" \
		"		}"
		"		else" \
		"		{" \
		"			phong_ads_light = float4(1.0,1.0,1.0,1.0);" \
		"		}" \
		"		float4 texcolor = myTexture2D.Sample(mySamplerState, input.texcoord);" \
		"		float4 color = input.color * phong_ads_light ;" \
		"		color.a = blend; " \
		"		return(color);	" \
		"	}											";

	// variables to hold errors and source code
	pID3DBlob_PixelShaderCode;
	pID3DBlob_Error = NULL;

	// Compile Pixel shader
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Failed For Pixel Shader: %s. \n", (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpfile_SB);
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "D3DCompile() Succeeded For Pixel Shader Compilation\n");
		fclose(gpfile_SB);
	}

	// Create Pixel shader object
	hr = gpID3D11Device_SB->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShaderCube4);


	if (FAILED(hr))
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Failed.\n");
		fclose(gpfile_SB);
		return(hr);
	}
	else
	{
		fopen_s(&gpfile_SB, gszLogFileName_SB, "a+");
		fprintf_s(gpfile_SB, "ID3D11Device_SB::CreatePixelShader() Succeeded.\n");
		fclose(gpfile_SB);
	}

	// Set pixel shader to the pipeline
	gpID3D11DeviceContext_SB->PSSetShader(gpID3D11PixelShaderCube4, 0, 0);

	return hr;
}

void ToggleAlphaState(ID3D11BlendState *m_alphaBlendingState)
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	gpID3D11DeviceContext_SB->OMSetBlendState(m_alphaBlendingState, blendFactor, 0xffffffff);

}


bool InitDirectInput(HINSTANCE hInstance)
{
	HRESULT hr;
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);


	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);


	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	//hr = DIMouse->SetCooperativeLevel(ghwnd_SB, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

//////CAMERA/////////
void UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);


	// Free-Look Camera
	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);
	///////////////**************new**************////////////////////

	camPosition += moveLeftRight * camRight;
	camPosition += moveBackForward * camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;


	//this is the final view matrix right?
	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}
