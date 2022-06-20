#pragma once

#include "Scene.h"

BEGIN(Client)
class CScene_StageSelect final : public CScene
{
private:
	explicit CScene_StageSelect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual ~CScene_StageSelect() = default;

public:
	virtual HRESULT Initialize();
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	//¿∫«ı¿Ã∞° ∞«µÎ
	virtual _int Change_to_NextScene() override;

private:


	HRESULT Ready_Light();
	HRESULT Ready_Layer_MainCamera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);

	HRESULT Ready_Layer_StaticMapObj(const _tchar* pLayerTag);


	HRESULT Ready_Layer_TriggerCollider(const _tchar* pLayerTag);
	

private:
	class CCamera_Main*				m_pMainCam = nullptr;
public:
	static CScene_StageSelect* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual void Free() override;
};
END
