#pragma once
#pragma warning(disable : 4251)

#ifdef _DEBUG
#pragma comment (lib,"Effects11d.lib")
#else
#pragma comment (lib,"Effects11.lib")
#endif // DEBUG

#include <d3d11.h>

#include <DirectXMath.h>
#include <typeinfo>


#include <d3dcompiler.h>
#include <d3dx11effect.h>

using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <process.h>
#include <time.h>

#include <string>
#include <fstream>
#include <tchar.h>

#include <math.h>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <algorithm>

using namespace std;

/////////비디오 포함헤더
#include <Vfw.h>
#pragma comment(lib, "vfw32.lib")

////////사운드 포함 헤더/////////////
#include <io.h>
#include "fmod.h"
//#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")






#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"
#include "Engine_Description.h"

using namespace Engine;

