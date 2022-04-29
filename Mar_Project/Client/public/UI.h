#pragma once

#include "GameObject.h"

BEGIN(Client)

class CUI abstract : public CGameObject
{
protected:
	explicit CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CUI(const CUI&  rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg)override;
	virtual HRESULT Initialize_Clone(void* pArg)override;

public:
	virtual _float	Get_RenderSortValue() override;
	virtual _float	Compute_RenderSortValue() override;

	void Set_UIDepth(_float fDepth) { m_fDepth = fDepth; };
	_float Get_UIDepth() { return m_fDepth; };

	void Set_Angle(_float DegreeAngle);
	_float Get_Angle() { return m_fAngle; };

	void Set_UIDescX(_float fX) { m_UIDesc.fX = fX; };
	void Set_UIDescY(_float fY) { m_UIDesc.fY = fY; };
	UIDESC Get_UIDesc() { return m_UIDesc; };

	void Set_TextureLayerIndex(_uint iIndex);
	void Set_TextureLayerIndex_UntilTo(_uint iIndex, _uint iTargetIndex,_double ChangingTime = 1);


	void Set_IsDraw(_bool IsDraw) { m_bIsDraw = IsDraw; };
	_uint Get_NowTextureLayerIndex();

public:
	virtual _int Update(_double fDeltaTime)override;
	virtual _int LateUpdate(_double fDeltaTime)override;
	virtual _int Render()override;
	virtual _int LateRender()override;


	HRESULT Change_TextureLayer(const _tchar* tagTextureLayer, _double FrameTime = 6.0);
	HRESULT Bind_Transform_OnShader(class CShader* pShader, const char* pValueName);
	
	UIDESC Convert_Rect_To_UI(FLOATRECT tRect);
	FLOATRECT Convert_UI_To_Rect(UIDESC tUIDesc);


	UIDESC Apply_Rect_To_MemberValue(FLOATRECT tRect);
	FLOATRECT Apply_UI_To_MemberValue(UIDESC tUIDesc);


	HRESULT Apply_UIDesc_To_Transform();
	HRESULT Apply_Rect_To_Transform();

	void Set_DrawingValueIsUIDesc(_bool bBool) { m_bIsDrawByDesc = bBool; };
protected:
	_uint			m_iTextureLayerIndex = 0;
	_uint			m_iTargetTextureLayerIndex = 0;
	_double			m_fPassedTime = 0;
	_double			m_fTotalUntilTime = 0;
	_bool			m_bIsUntilTo = false;

	_bool			m_bIsDraw = true;
	_bool			m_bIsDrawByDesc = false;


	_float			m_fDepth = 0;
	_float			m_fAngle = 0;
	UIDESC			m_UIDesc;
	FLOATRECT		m_UIRect;
	_float4x4		m_ProjMatrix;
	CTransform*		m_pTransformCom = nullptr;

public:
	virtual void Free()override;

};
END

