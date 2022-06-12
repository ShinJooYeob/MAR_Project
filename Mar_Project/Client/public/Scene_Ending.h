#pragma once

#include "Scene.h"

BEGIN(Client)
class CScene_Ending final : public CScene
{
private:
	explicit CScene_Ending(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_Ending() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


	virtual _int Change_to_NextScene() override;

private:
	HRESULT Ready_Light();
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

	HRESULT Ready_Layer_EndingAlice(const _tchar* pLayerTag);


	HRESULT Ready_Layer_StaticMapObj(const _tchar* pLayerTag);

	HRESULT Ready_Layer_TriggerCollider(const _tchar* pLayerTag);


private:
	class CCamera_Main*				m_pMainCam = nullptr;
	_bool			m_bStartChecker = false;
	_double			m_fTimer = 0;
	_uint			m_iChecker = 0;


	vector<CAMACTDESC>		 m_vecCamPositions;
	vector<CAMACTDESC>		 m_vecLookPostions;
	HRESULT Load_ActionCam(const _tchar* szPath);

public:
	static CScene_Ending* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};

END