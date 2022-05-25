#pragma once


#include "GameObject.h"


BEGIN(Client)

class CDustWind :public CGameObject
{
public:
	typedef struct tagDustWindDesc
	{
		_float3 vPosition;
		_float3  StartScale = _float3(1.f);
		_float3  TargetScale = _float3(1.f);
		_uint	eEasingType = TYPE_Linear;
		_double	ToTalLifeTime = 1;
		_float4 vColor = _float4(0.56470588f, 0.3058823f, 0.0470588f, 1);

		_bool	bNotAttackPlayer = false;

	}DUSTWINDDESC;


private:
	CDustWind(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDustWind(const CDustWind& rhs);
	virtual ~CDustWind() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModel = nullptr;

private:
	class CPlayer*		m_pPlayer = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;

	DUSTWINDDESC		m_tDesc;

	_uint				m_iRand = 0;
	_double				m_PassedTime = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstTable();

public:
	static CDustWind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


END
