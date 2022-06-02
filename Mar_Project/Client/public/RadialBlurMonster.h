#pragma once

#include "UI.h"

BEGIN(Client)

class CRadialBlurMonster : public CUI
{

protected:
	explicit CRadialBlurMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CRadialBlurMonster(const CRadialBlurMonster&  rhs);
	virtual ~CRadialBlurMonster() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	void Chage_TextureIndex(_uint iIndex = 0);

	void Set_TargetPos(_float3 vPos);
	void Set_IsOn(_bool bBool);
	 
protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;



	_bool				m_bIsOn = false;


	_bool				m_bNeedToDraw = false;
	_double				m_PassedTime = FLT_MAX;
	_double				m_TotalTime = 0;
	_float2				m_TargetPos = _float2(0.5f, 0.5f);
	_float				m_ZoomSize = 0;
	_float				m_ZoomPower = 0;
	_float4				m_vTargetColor = _float4(0,0,0,0);

protected:
	HRESULT SetUp_Components();

public:
	static CRadialBlurMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};
END

