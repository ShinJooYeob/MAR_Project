#pragma once

#ifdef USE_IMGUI

#include "Base.h"

//#include "imgui.h"
//#include "imgui_impl_win32.h"
//#include "imgui_impl_dx11.h"


BEGIN(Client)

class CImguiMgr :public CBase
{
	DECLARE_SINGLETON(CImguiMgr)

private:
	explicit CImguiMgr();
	virtual ~CImguiMgr() = default;

public:
	HRESULT Initialize_ImguiMgr();
	_int	Update_ImguiMgr(_double fDeltatime);
	HRESULT	Render_ImguiMgr();


public:
	virtual void Free()override;

};

END	

#endif // DEBUG