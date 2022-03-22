#pragma once

#include "Scene.h"


#ifdef USE_IMGUI
#include "ImguiMgr.h"
#endif // USE_IMGUI

BEGIN(Client)
class CScene_MapEditor final : public CScene
{
private:
	explicit CScene_MapEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_MapEditor() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


	virtual _int Change_to_NextScene() override;

private:
	HRESULT Update_First_Frame(_double fDeltatime, const char* szFrameBarName);
	_bool	Prevent_Order = false;


	_bool show_demo_window;
	_bool show_another_window;
	ImVec4 clear_color;

private:
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);




public:
	static CScene_MapEditor* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};
END
