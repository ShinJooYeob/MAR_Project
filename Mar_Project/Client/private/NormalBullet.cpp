#include "stdafx.h"
#include "..\public\NormalBullet.h"




CNormalBullet::CNormalBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CBullet(pDevice,pDeviceContext)
{
}

CNormalBullet::CNormalBullet(const CNormalBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CNormalBullet::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CNormalBullet::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_TeaBullet), TAG_COM(Com_Model), (CComponent**)&m_pModel));



	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_fTotalLifeTime = pUtilMgr->RandomFloat(1.f, 1.5f);
	m_vTargetDir = m_vTargetDir.XMVector() + pUtilMgr->RandomFloat3(-0.1f, 0.1f).XMVector();



	return S_OK;
}

_int CNormalBullet::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	

	//_float EasedValue = g_pGameInstance->Easing(TYPE_Linear, 20, -100, m_fLifeTime, m_fTotalLifeTime);
	_float EasedValue = g_pGameInstance->Easing(TYPE_QuinticIn, 0, -5, m_fLifeTime, m_fTotalLifeTime);

	_Vector vNewLook = (m_vTargetDir.XMVector() + XMVectorSet(0, 1, 0, 0) * EasedValue);
	m_pTransformCom->LookDir(vNewLook);
	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1, 0, 0), EasedValue, fDeltaTime);

	return _int();
}

_int CNormalBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;

	if (g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS)))
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CNormalBullet::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}


	return _int();
}

_int CNormalBullet::LateRender()
{

	return _int();
}

CNormalBullet * CNormalBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNormalBullet*	pInstance = new CNormalBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CNormalBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNormalBullet::Clone(void * pArg)
{
	CNormalBullet*	pInstance = new CNormalBullet(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CNormalBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNormalBullet::Free()
{
	__super::Free();



}
