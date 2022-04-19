#include "stdafx.h"
#include "..\public\UI.h"
#include "Texture.h"


CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CUI::CUI(const CUI & rhs)
	:CGameObject(rhs)
{

}

HRESULT CUI::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));

	return S_OK;
}

HRESULT CUI::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(_int(g_iWinCX), _int(g_iWinCY), 0.f, 1.f)));


	return S_OK;
}

_float CUI::Get_RenderSortValue()
{
	return  m_fRenderSortValue = m_fDepth;
}

_float CUI::Compute_RenderSortValue()
{
	return  m_fRenderSortValue = m_fDepth;
}

HRESULT CUI::Change_TextureLayer(const _tchar* tagTextureLayer, _double FrameTime)
{
	CTexture* pTexture = (CTexture*)(Find_Components(TAG_COM(Com_Texture)));
	NULL_CHECK_RETURN(pTexture,E_FAIL);

	

	return pTexture->Change_TextureLayer(tagTextureLayer, FrameTime);;
}

_int CUI::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	return _int();
}

_int CUI::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	return _int();
}

_int CUI::Render()
{
	if (__super::Render( ) < 0)
		return -1;

	return _int();
}

_int CUI::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CUI::Bind_Transform_OnShader(CShader * pShader, const char * pValueName)
{
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	return m_pTransformCom->Bind_OnShader(pShader,pValueName);
}



UIDESC CUI::Convert_Rect_To_UI(FLOATRECT tRect)
{
	UIDESC Result;

	Result.fCX = tRect.right - tRect.left;
	Result.fCY = tRect.bottom - tRect.top;
	Result.fX = tRect.left + Result.fCX * 0.5f;
	Result.fY = tRect.top + Result.fCY * 0.5f;

	return Result;
}

FLOATRECT CUI::Convert_UI_To_Rect(UIDESC tUIDesc)
{
	FLOATRECT Result;

	Result.left = tUIDesc.fX - tUIDesc.fCX * 0.5f;
	Result.top = tUIDesc.fY - tUIDesc.fCY * 0.5f;
	Result.right = tUIDesc.fX + tUIDesc.fCX * 0.5f;
	Result.bottom = tUIDesc.fY + tUIDesc.fCY * 0.5f;


	return Result;
}

UIDESC CUI::Apply_Rect_To_MemberValue(FLOATRECT tRect)
{
	m_UIRect = tRect;


	m_UIDesc.fCX = m_UIRect.right - m_UIRect.left;
	m_UIDesc.fCY = m_UIRect.bottom - m_UIRect.top;
	m_UIDesc.fX = m_UIRect.left + m_UIDesc.fCX * 0.5f;
	m_UIDesc.fY = m_UIRect.top + m_UIDesc.fCY * 0.5f;


	return m_UIDesc;
}

FLOATRECT CUI::Apply_UI_To_MemberValue(UIDESC tUIDesc)
{
	m_UIDesc = tUIDesc;

	m_UIRect.left = m_UIDesc.fX - m_UIDesc.fCX * 0.5f;
	m_UIRect.top = m_UIDesc.fY - m_UIDesc.fCY * 0.5f;
	m_UIRect.right = m_UIDesc.fX + m_UIDesc.fCX * 0.5f;
	m_UIRect.bottom = m_UIDesc.fY + m_UIDesc.fCY * 0.5f;


	return m_UIRect;
}

void CUI::Set_Angle(_float DegreeAngle)
{
	m_fAngle = DegreeAngle;
}

void CUI::Set_TextureLayerIndex(_uint iIndex)
{
	m_iTextureLayerIndex = iIndex;

	m_iTargetTextureLayerIndex = 0;
	m_fPassedTime = 0;
	m_fTotalUntilTime = 0;
	m_bIsUntilTo = false;


}

void CUI::Set_TextureLayerIndex_UntilTo(_uint iIndex, _uint iTargetIndex, _double ChangingTime)
{
	m_iTextureLayerIndex = iIndex;

	m_iTargetTextureLayerIndex = iTargetIndex;
	m_fPassedTime = 0;
	m_fTotalUntilTime = ChangingTime;
	m_bIsUntilTo = true;
}

_uint CUI::Get_NowTextureLayerIndex()
{

	if (m_bIsUntilTo)
	{
		return _uint(g_pGameInstance->Easing(TYPE_Linear, _float(m_iTextureLayerIndex), _float(m_iTargetTextureLayerIndex), _float(m_fPassedTime), _float(m_fTotalUntilTime)));

	}
	else
	{
		return m_iTextureLayerIndex;
	}

}

HRESULT CUI::Apply_UIDesc_To_Transform()
{
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	m_pTransformCom->Set_Matrix(XMMatrixIdentity());
	
	m_pTransformCom->Scaled(CTransform::STATE_RIGHT, m_UIDesc.fCX);
	m_pTransformCom->Scaled(CTransform::STATE_UP, m_UIDesc.fCY);

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(m_UIDesc.fX - g_iWinCX * 0.5f, -m_UIDesc.fY + g_iWinCY * 0.5f, 0.f));

	if (m_fAngle != 0)
		m_pTransformCom->Rotation_CW(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(m_fAngle));

	return S_OK;
}

HRESULT CUI::Apply_Rect_To_Transform()
{
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	m_pTransformCom->Set_Matrix(XMMatrixIdentity());


	m_pTransformCom->Scaled(CTransform::STATE_RIGHT, m_UIRect.right - m_UIRect.left);
	m_pTransformCom->Scaled(CTransform::STATE_UP, m_UIRect.bottom - m_UIRect.top);


	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, 
		_float3((m_UIRect.right + m_UIRect.left) * 0.5f - g_iWinCX * 0.5f,
			-(m_UIRect.bottom + m_UIRect.top) * 0.5f + g_iWinCY * 0.5f, 0.f));


	if (m_fAngle != 0)
		m_pTransformCom->Rotation_CCW(XMVectorSet(0, 0, 1, 0), XMConvertToRadians(m_fAngle));
	return S_OK;
}


void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
