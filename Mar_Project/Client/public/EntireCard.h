#pragma once
#include "MonsterWeapon.h"

BEGIN(Client)

class CEntireCard final :public CMonsterWeapon
{
public:
	typedef struct tagEntireCardDesc
	{
		_float3 vStartPos;
		_float3 vMonsterPos;

	}ENTIRECARDDESC;



private:
	CEntireCard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEntireCard(const CEntireCard& rhs);
	virtual ~CEntireCard() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	void Spout_Piece();


private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_ModelArr[4] = {nullptr};


	
private:
	class CPlayer*		m_pPlayer= nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	_bool				m_bIsPlayerCloser = false;
	_float3				m_vTurningVector ;
	_double				m_PatternChangeTime = 0;

	ENTIRECARDDESC		m_tDesc;

private:
	HRESULT SetUp_Components();

public:
	static CEntireCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END