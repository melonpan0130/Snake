// SnakeGame.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "SnakeGame.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND&); // 함수가 바뀌었으니 선언문도 수정
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// about 함수 지움('정보' 메뉴의 관련 함수)

LPDIRECT3D9			g_pD3D;
LPDIRECT3DDEVICE9	g_pD3DDevice;
D3DCOLOR g_ClearColor = D3DCOLOR_XRGB(0, 0, 255); // 컬러키 생성

LPD3DXSPRITE g_Sprite;
LPDIRECT3DTEXTURE9 g_Texture;

D3DXVECTOR3 HeadPos = D3DXVECTOR3(100, 100, 0);
D3DXVECTOR3 g_Dir = D3DXVECTOR3(1, 0, 0);
D3DXVECTOR3 g_BlockPos;

D3DXVECTOR3 g_BodyPos[100];
D3DXVECTOR3 g_BodyDir[100];
int g_TailIndex = 0;
int g_turn = 0;

bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();
void DrawSprite(D3DXVECTOR3& center, D3DXVECTOR3& position);
void Render();

// Sprite
LPD3DXSPRITE CreateSprite(LPDIRECT3DDEVICE9 pD3DDevice);
void ReleaseSprite(LPD3DXSPRITE Sprite);

// Texture
LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pD3DDevice, int width, int height);
void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SNAKEGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	HWND hWnd = NULL; // 함수에 들어갈 제물(HWND) 생성

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow, hWnd))
    {
        return FALSE;
    }

	InitDirect3D(hWnd); // 문제점: 현재 가져올 hWnd 가 없다. -> InitInstance 함수를 수정해 hWnd 의 값을 가져옴ㅎ
	g_Sprite = CreateSprite(g_pD3DDevice); // direct가 생성되고 난 다음에 sprite 생성
	g_Texture = CreateTexture(g_pD3DDevice, 20, 20);

	srand((unsigned int)time(NULL));

	g_BlockPos = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);

    MSG msg;
	ZeroMemory(&msg, sizeof(msg)); // &msg 위치에 msg크기만큼 0으로 초기화

	g_BodyPos[0] = D3DXVECTOR3(100, 100, 0);
	float start = GetTickCount64();
	float turnTime = GetTickCount64();
	while(msg.message != WM_QUIT)
    {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			HeadPos = HeadPos + g_Dir;

			if (turnTime - start > 2000) {
				start = GetTickCount64();
				if (g_turn > g_TailIndex)
					g_turn = 0;
				g_BodyDir[g_turn] = g_BodyDir[g_turn - 1];
				g_turn++;
			}
			else {
				turnTime = GetTickCount64();
			}

			for (int i = 0; i <= g_TailIndex; i++) {
				if (i == 0) {
					g_BodyPos[i] = HeadPos - g_Dir * 20;
					g_BodyDir[i] = g_Dir;
				}
				else {
					g_BodyPos[i] = g_BodyPos[i - 1] - g_Dir * 20;
					
				}
			}

			if (g_BlockPos.x - 20.f < HeadPos.x && HeadPos.x < g_BlockPos.x + 20.f
				&& g_BlockPos.y - 20.f < HeadPos.y && HeadPos.y < g_BlockPos.y + 20.f) {
				// g_ClearColor = D3DCOLOR_XRGB(0, 255, 0);
				g_BlockPos = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);
				g_TailIndex ++;
				// g_PosTail[g_PosTailIndex] = g_Pos - g_Dir;
			}
 			else
				g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);

			if (HeadPos.x > 640.f - 10.f)
				HeadPos.x = 640.f - 10.f;
			else if (HeadPos.x < 10.f)
				HeadPos.x = 10.f;

			if (HeadPos.y > 480.f - 10.f)
				HeadPos.y = 480.f - 10.f;
			else if (HeadPos.y < 10.f)
				HeadPos.y = 10.f;

			Render();
		}
        
    }
	ReleaseTexture(g_Texture);
	ReleaseSprite(g_Sprite); // 디바이스가 삭제되기 이전에 삭제
	ReleaseDirect3D();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKEGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_SNAKEGAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& rhWnd) // 파라미터 하나 추가(rhWnd)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);
   // 화면 조정 (640, 480)

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   rhWnd = hWnd;

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    // case WM_COMMAND: 지움
    // case WM_PAINT: 지움
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT)
			g_Dir = D3DXVECTOR3(-1, 0, 0);
		if (wParam == VK_RIGHT)
			g_Dir = D3DXVECTOR3(1, 0, 0);
		if (wParam == VK_UP)
			g_Dir = D3DXVECTOR3(0, -1, 0);
		if (wParam == VK_DOWN)
			g_Dir = D3DXVECTOR3(0, 1, 0);
	}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool InitDirect3D(HWND hwnd)
{
	// DX 오브젝트 생성
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL)
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // 0으로 모두 리셋

	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = hwnd; // 핸들 가져옴
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ?
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // ?
	d3dpp.BackBufferCount = 1; // 백버퍼 갯수
	d3dpp.BackBufferWidth = 640; // 백버퍼 가로 길이
	d3dpp.BackBufferHeight = 480; // 백버퍼 세로 길이

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, hwnd
		, D3DCREATE_HARDWARE_VERTEXPROCESSING
		, &d3dpp
		, &g_pD3DDevice) == E_FAIL)
		return false;
	
	return true;
}

void ReleaseDirect3D()
{
	// 삭제는 생성한 반대로
	if (g_pD3DDevice != NULL)
		g_pD3DDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();

	g_pD3DDevice = NULL;
	g_pD3D = NULL;
}

void DrawSprite(D3DXVECTOR3& center, D3DXVECTOR3& position)
{
	/*
	D3DXMATRIX mat;
	D3DXVECTOR2 pos(position.x, position.y);
	D3DXVECTOR2 scale(1.f, 1.f);
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTransformation2D(&mat, NULL, 0.f, &scale, NULL, 0.7f, &pos);
	*/

	g_Sprite->Begin(D3DXSPRITE_ALPHABLEND);// alpha blending이 되게 함
	// g_Sprite->SetTransform(&mat);
	g_Sprite->Draw(g_Texture, NULL, &center, &position, D3DCOLOR_XRGB(255, 255, 255));
	g_Sprite->End();
}

void Render()
{
	if (g_pD3DDevice == NULL)
		return;

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		D3DXVECTOR3 center(10, 10, 0);

		// Sprite 그리기
		
		DrawSprite(center, HeadPos); // pc
		if (g_TailIndex >= 0)
			for (int i = 0; i <= g_TailIndex; i++)
				DrawSprite(center, g_BodyPos[i]); // pc

		DrawSprite(center, g_BlockPos); // apple

		g_pD3DDevice->EndScene(); // 무대가 끝남을 알림.
	}

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

LPD3DXSPRITE CreateSprite(LPDIRECT3DDEVICE9 pD3DDevice)
{
	LPD3DXSPRITE pSpr;
	// auto : 함수의 반환값을 모를 때 auto 사용
	HRESULT hr = D3DXCreateSprite(pD3DDevice, &pSpr);

	if (FAILED(hr))
		return NULL;

	return pSpr;
}

void ReleaseSprite(LPD3DXSPRITE Sprite)
{
	if (Sprite != NULL)
		Sprite->Release();
}

LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pD3DDevice, int width, int height)
{
	LPDIRECT3DTEXTURE9 tex;
	pD3DDevice->CreateTexture(width, height, 1, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, 0);

	D3DLOCKED_RECT rect;
	tex->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
	DWORD* pColor = (DWORD*)rect.pBits;
	
	D3DCOLOR lineColor = D3DCOLOR_ARGB(255, 255, 255, 0);;

	for (int i = 0; i < width * height; i++)
		pColor[i] = D3DCOLOR_ARGB(100, 255, 0, 0);

	// 가로선
	for (int x = 0; x < width; x++)
	{
		pColor[x] = lineColor;
		pColor[x + 2 * width] = lineColor;
		pColor[x + 1 * width] = lineColor;

		pColor[x + (height - 3) * width] = lineColor;
		pColor[x + (height - 2) * width] = lineColor;
		pColor[x + (height - 1) * width] = lineColor;
	}

	// 세로선
	for (int y = 0; y < height; y++)
	{
		pColor[y * width] = lineColor;
		pColor[1 + y * width] = lineColor;
		pColor[2 + y * width] = lineColor;

		pColor[width - 1 + y * width] = lineColor;
		pColor[width - 2 + y * width] = lineColor;
		pColor[width - 3 + y * width] = lineColor;
	}

	tex->UnlockRect(0);

	return tex;
}

void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture)
{
	if (Texture != NULL)
		Texture->Release();
}
