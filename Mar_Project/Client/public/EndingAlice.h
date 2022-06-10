#pragma once

#include "Monster.h"

BEGIN(Client)

class CEndingAlice final : public CMonster
{
private:
	CEndingAlice(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEndingAlice(const CEndingAlice& rhs);
	virtual ~CEndingAlice() = default;


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


	_uint				m_iOldAnimIndex = INT_MAX;
	_uint				m_iAdjMovedIndex = 0;

	_bool				m_bSpwanAnimStart = false;
	_double				m_SpwanPassedTime = 0;

	_bool				 m_bDeathAnimStart = false;
	_double				m_DeathAnimPassedTime = 0;

private:
	HRESULT SetUp_Components();

	virtual HRESULT Set_Monster_On_Terrain(CTransform* pTransform, _double fDeltaTime)override;
	HRESULT Adjust_AnimMovedTransform(_double fDeltatime);


public:
	static CEndingAlice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};


END