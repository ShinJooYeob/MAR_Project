#pragma once



static const int g_iWinCX = 1280;
static const int g_iWinCY = 720;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern _double g_fDeltaTime;

#ifdef USE_IMGUI
#define GETIMGUI GetSingle(CImguiMgr)
#endif // USE_IMGUI

#define FPS		1.f/60.f

namespace Client {};
using namespace Client;

#include "Client_Enum.h"
#include "Client_Struct.h"





