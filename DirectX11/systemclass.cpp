#include "systemclass.h"

SystemClass::SystemClass()
	: m_Input(nullptr)
	, m_Application(nullptr)
{
	
}

SystemClass::SystemClass(const SystemClass&)
{
	
}

SystemClass::~SystemClass()
{
	
}

bool SystemClass::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	bool result = false;

	InitializeWindows(OUT screenWidth, OUT screenHeight);

	m_Input = new InputClass;

	m_Input->Initialize();

	m_Application = new ApplicationClass;

	result = m_Application->Initialize(screenWidth, screenHeight, m_hWnd);

	return result;
}

void SystemClass::Shutdown()
{
	if(m_Application)
	{
		m_Application->Shutdown();
		delete m_Application;
		m_Application = nullptr;
	}

	if(m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done = false;
	bool result = false;

	ZeroMemory(&msg, sizeof(msg));

	while(!done)
	{
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result = false;

	if(m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_Application->Frame();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT SystemClass::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		m_Input->KeyDown(static_cast<unsigned int>(wParam));
		return 0;
	}
	case WM_KEYUP:
	{
		m_Input->KeyUp(static_cast<unsigned int>(wParam));
		return 0;
	}

	default:
		{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}

void SystemClass::InitializeWindows(OUT int& screenWidth, OUT int& screenHeight)
{
	DEVMODE dmScreenSettings;
	int posX = 0;
	int posY = 0;

	ApplicationHandle = this;

	m_hInstance = GetModuleHandle(nullptr);

	m_applicationName = L"Engine";

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(m_hInstance, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, 
		posX, posY, screenWidth, screenHeight, 
		nullptr, nullptr, m_hInstance, nullptr);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;

	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;

	ApplicationHandle = nullptr;

	return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
		{
		PostQuitMessage(0);
		return 0;
		}
	case WM_CLOSE:
		{
		PostQuitMessage(0);
		return 0;
		}

	default:
		{
		return ApplicationHandle->MessageHandler(hWnd, msg, wParam, lParam);
		}
	}
}