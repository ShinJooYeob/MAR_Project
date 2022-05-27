#include "stdafx.h"
#include "..\public\WaspArrow.h"



CWaspArrow::CWaspArrow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CWaspArrow::CWaspArrow(const CWaspArrow & rhs)
	: CMonster(rhs)
{
}

HRESULT CWaspArrow::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CWaspArrow::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	m_fHP = m_fMaxHP = 192;


	return S_OK;
}

_int CWaspArrow::Update(_double fDeltaTime)
{


	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));

	if (m_bIsOnScreen)
	{
		m_pColliderCom->Update_Transform(0, m_pTransformCom->Get_WorldMatrix());

		for (_uint i = 1; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		{

			_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr[i - 1].pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr[i - 1].pDefaultPivotMat);

			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());
		}


		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
	}
	return _int();
}

_int CWaspArrow::LateUpdate(_double fDeltaTime)
{



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CWaspArrow::Render()
{
	if (__super::Render() < 0)
		return -1;


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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}
	return _int();
}

_int CWaspArrow::LightRender()
{
	return _int();
}

HRESULT CWaspArrow::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_WaspArrow), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));

	m_tCollisionAttachPtr[0] = m_pModel->Find_AttachMatrix_InHirarchyNode("WaspSpine2");
	NULL_CHECK_RETURN(m_tCollisionAttachPtr[0].pDefaultPivotMat, E_FAIL);

	m_tCollisionAttachPtr[1] = m_pModel->Find_AttachMatrix_InHirarchyNode("WaspBone01Bone01");
	NULL_CHECK_RETURN(m_tCollisionAttachPtr[1].pDefaultPivotMat, E_FAIL);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	//Pivot  : 0.000000f , 1.539999f , 0.000000f , 1
	//size  : 3.539998f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(3.539998f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 1.539999f, 0.000000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));


	//Pivot  : 0.000000f , -0.400000f , -1.799999f , 1
	//size  : 1.000000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, -0.400000f, -1.799999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();


	//Pivot  : -0.070000f , 0.690000f , -1.519999f , 1
	//size  : 1.150000f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.150000f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.070000f, 0.690000f, -1.519999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(0, 5.f, 0));

	return S_OK;
}

CWaspArrow * CWaspArrow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CWaspArrow*	pInstance = new CWaspArrow(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CWaspArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWaspArrow::Clone(void * pArg)
{
	CWaspArrow*	pInstance = new CWaspArrow(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CWaspArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaspArrow::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	

}
