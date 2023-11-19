#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SystemClass* System = nullptr;
	bool result = false;

	System = new SystemClass;

	result = System->Initialize();
	if(result)
	{
		System->Run();
	}

	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}