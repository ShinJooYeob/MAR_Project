#pragma once

#include "GameObject.h"

//BEGIN(Engine)
//class CCamera;
//END


BEGIN(Client)
class CWeapon;

class CStageSelectAlice final : public CGameObject
{

private:
	CStageSelectAlice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CStageSelectAlice(const CStageSelectAlice& rhs);
	virtual ~CStageSelectAlice() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;


public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModel = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

private:
	class CCamera_Main*			m_pMainCamera = nullptr;
	_float3						m_CamDegreeAngle = _float3(20.f,0,-5.f);


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();

	HRESULT Input_Keyboard(_double fDeltaTime);
	HRESULT Move_Update(_double fDeltaTime, CGameInstance* pInstance);
	HRESULT Jump_Update(_double fDeltaTime, CGameInstance* pInstance);

	HRESULT Set_Player_On_Terrain();
	HRESULT Set_Camera_On_Player(_double fDeltaTime);


public:
	static CStageSelectAlice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END