#pragma once

#include "Scene.h"

BEGIN(Client)
class CScene_Stage1 final : public CScene
{
private:
	explicit CScene_Stage1(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_Stage1() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;



private:
	HRESULT Ready_Light();
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_JumpPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SteamPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_HiddenPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_TeethObj(const _tchar* pLayerTag);
	HRESULT Ready_Layer_RoseObj(const _tchar* pLayerTag);

	HRESULT Ready_Layer_StaticMapObj(const _tchar* pLayerTag);

	HRESULT Ready_Layer_Executor(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Eyepot(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Grunt(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WaspArrow(const _tchar* pLayerTag);
	HRESULT Ready_Layer_WaspInk(const _tchar* pLayerTag);



private:
	class CCamera_Main*				m_pMainCam = nullptr;
public:
	static CScene_Stage1* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};

END