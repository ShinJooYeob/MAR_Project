#pragma once

#include "Scene.h"

BEGIN(Client)
class CScene_Stage2 final : public CScene
{
private:
	explicit CScene_Stage2(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_Stage2() = default;

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

	HRESULT Ready_Layer_StaticMapObj(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SteamPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_HiddenPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_EscalatorPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SlidePad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_ButtonPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_ShapeMemPad(const _tchar* pLayerTag);
	HRESULT Ready_Layer_ShapeMemBtn(const _tchar* pLayerTag);
	HRESULT Ready_Layer_PresserObj(const _tchar* pLayerTag);



private:
	class CCamera_Main*				m_pMainCam = nullptr;
public:
	static CScene_Stage2* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};

END