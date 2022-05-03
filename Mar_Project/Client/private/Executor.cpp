#include "stdafx.h"
#include "..\public\Executor.h"
#include "Scythe.h"



CExecutor::CExecutor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CExecutor::CExecutor(const CExecutor & rhs)
	: CMonster(rhs)
{
}

HRESULT CExecutor::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CExecutor::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	m_fHP = m_fMaxHP = 192;


	FAILED_CHECK(SetUp_Weapon());

	return S_OK;
}

_int CExecutor::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
		m_pModel->Change_AnimIndex(0);
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
		m_pModel->Change_AnimIndex(8);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
		m_pModel->Change_AnimIndex(9);
	if (g_pGameInstance->Get_DIKeyState(DIK_4)&DIS_Down)
		m_pModel->Change_AnimIndex(10);
	if (g_pGameInstance->Get_DIKeyState(DIK_5)&DIS_Down)
		m_pModel->Change_AnimIndex(11);
	if (g_pGameInstance->Get_DIKeyState(DIK_6)&DIS_Down)
		m_pModel->Change_AnimIndex(12);
	if (g_pGameInstance->Get_DIKeyState(DIK_7)&DIS_Down)
		m_pModel->Change_AnimIndex(13);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));




	if (m_bIsOnScreen)
	{
		m_pColliderCom->Update_Transform(0, m_pTransformCom->Get_WorldMatrix());

		_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr.pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr.pDefaultPivotMat);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


		m_pColliderCom->Update_Transform(1, TransformMatrix * m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

		m_vecWeapon[0]->Update(fDeltaTime);
	}




	return _int();
}

_int CExecutor::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)return -1;



	if (m_bIsOnScreen)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
		m_vecWeapon[0]->LateUpdate(fDeltaTime);
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CExecutor::Render()
{
	if (__super::Render() < 0)		return -1;


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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}

	return _int();
	return _int();
}

_int CExecutor::LateRender()
{
	return _int();
}

HRESULT CExecutor::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Executor), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));

	m_tCollisionAttachPtr = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Spine1");
	NULL_CHECK_RETURN(m_tCollisionAttachPtr.pDefaultPivotMat, E_FAIL);



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : 0.000000f , 1.459999f , 0.340000f , 1
	//size  : 3.909997f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(3.909997f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 1.459999f, 0.340000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));



	//Pivot  : 0.000000f , 0.310000f , -2.019999f , 1
	//size  : 2.039999f , 0.450001f , 1.180000f  
	//(x,z,y)
	ColliderDesc.vScale = _float3(1.180000f,0.450001f, 2.039999f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 0.310000f, -2.019999f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();




	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	return S_OK;
}

HRESULT CExecutor::SetUp_Weapon()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	CMonsterWeapon::MONWEAPONDESC tWeaponDesc;
	tWeaponDesc.pModel = m_pModel;
	tWeaponDesc.pParantTransform = m_pTransformCom;
	tWeaponDesc.szHirarchyNodeName = "Bip01-Prop1";

	CMonsterWeapon* pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_Scythe), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);

	m_vecWeapon.push_back(pWeapon);


	return S_OK;
}

CExecutor * CExecutor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CExecutor*	pInstance = new CExecutor(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CExecutor::Clone(void * pArg)
{
	CExecutor*	pInstance = new CExecutor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CExecutor::Free()
{
	__super::Free();


	for (auto& pWeapon : m_vecWeapon)
		Safe_Release(pWeapon);
	

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
