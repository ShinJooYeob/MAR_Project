#pragma once

#include "Scene.h"

BEGIN(Client)
class CScene_Boss final : public CScene
{
private:
	explicit CScene_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_Boss() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;



private:
	HRESULT Ready_Light();
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

	HRESULT Ready_Layer_StaticMapObj(const _tchar* pLayerTag);

	HRESULT Ready_Layer_DollMaker(const _tchar* pLayerTag);
	HRESULT Ready_Layer_HandyBoy(const _tchar* pLayerTag);
	HRESULT Ready_Layer_HandyGirl(const _tchar* pLayerTag);

	HRESULT Ready_Layer_TriggerCollider(const _tchar* pLayerTag);


private:
	class CCamera_Main*				m_pMainCam = nullptr;
public:
	static CScene_Boss* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};

END