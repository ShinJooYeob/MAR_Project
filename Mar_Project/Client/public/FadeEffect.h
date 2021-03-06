#pragma once

#include "UI.h"

BEGIN(Client)

class CFadeEffect : public CUI
{
public:
	enum FadeID
	{
		FadeID_FadeIn,
		FadeID_FadeOut,
		FadeID_FadeInOut,
		FadeID_FadeOutIn,
		FadeID_FlikeringIn,
		FadeID_End,
	};

protected:
	explicit CFadeEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CFadeEffect(const CFadeEffect&  rhs);
	virtual ~CFadeEffect() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LightRender()override;

	
	_bool	Start_FadeEffect(FadeID eFadeType,_double Duration, _float4 TargetColor = _float4(0, 0, 0, 1));
	void Add_RenderGroup_ForSceneChanging();

	void Chage_TextureIndex(_uint iIndex = 0);


	 
protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;


	_bool				m_bNeedToDraw = false;
	FadeID				m_eKindsFade = FadeID_End;
	_double				m_PassedTime = FLT_MAX;
	_double				m_TotalTime = 0;
	_float				m_fAlphaTestValue = 0.1f;
	_float4				m_vTargetColor = _float4(0,0,0,0);

protected:
	HRESULT SetUp_Components();

public:
	static CFadeEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free()override;

};
END

