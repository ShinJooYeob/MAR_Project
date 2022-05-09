#include "stdafx.h"
#include "..\public\GrenadeBullet.h"
#include "Monster.h"




CGrenadeBullet::CGrenadeBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CBullet(pDevice,pDeviceContext)
{
}

CGrenadeBullet::CGrenadeBullet(const CGrenadeBullet & rhs)
	: CBullet(rhs)
{
}

HRESULT CGrenadeBullet::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CGrenadeBullet::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_OilBullet), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));
	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(0.6f, 0.6f, 0.6f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);
	m_fTotalLifeTime = pUtilMgr->RandomFloat(2.f, 3.f);
	m_vTargetDir = m_vTargetDir.XMVector() + pUtilMgr->RandomFloat3(-0.1f, 0.1f).XMVector();

	return S_OK;
}

void CGrenadeBullet::Set_IsDead()
{
	m_pTransformCom->Set_IsOwnerDead();
	m_bIsDead = true;
}

_int CGrenadeBullet::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	_float EasedValue = g_pGameInstance->Easing(TYPE_Linear, 20, -100, m_fLifeTime, m_fTotalLifeTime);

	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1, 0, 0), EasedValue, fDeltaTime);

	m_pTransformCom->LookDir(m_vTargetDir.XMVector() + XMVectorSet(0, 1, 0, 0)* EasedValue);


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);

	}

	return _int();
}

_int CGrenadeBullet::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CGrenadeBullet::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

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

_int CGrenadeBullet::LateRender()
{

	return _int();
}

void CGrenadeBullet::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Monster:
	{

		pConflictedCollider->Set_Conflicted();
		//GetSingle(CUtilityMgr)->SlowMotionStart();
		((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(15);


	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

CGrenadeBullet * CGrenadeBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGrenadeBullet*	pInstance = new CGrenadeBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CGrenadeBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrenadeBullet::Clone(void * pArg)
{
	CGrenadeBullet*	pInstance = new CGrenadeBullet(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGrenadeBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrenadeBullet::Free()
{
	__super::Free();



}
