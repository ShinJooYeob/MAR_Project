#include "stdafx.h"
#include "..\public\Umbrella.h"




CUmbrella::CUmbrella(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CUmbrella::CUmbrella(const CUmbrella & rhs)
	: CWeapon(rhs)
{
}

HRESULT CUmbrella::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CUmbrella::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	m_szNameTag = L"Umbrella";
	
	return S_OK;
}

_int CUmbrella::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	
	if (!m_TempAttackAble && m_ATABPassedTime < 0.2f)
	{
		m_ATABPassedTime += fDeltaTime;

		if (m_ATABPassedTime > 0.2)
		{
			m_bIsAttackAble = false;
		}
	}


	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));


	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	//m_BoneMatrix = TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();
	//TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();
	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, TransformMatrix);

	if (m_bIsAttackAble)
	{
		g_pGameInstance->Add_CollisionGroup(CollisionType_Player, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

	}

	return _int();
}

_int CUmbrella::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CUmbrella::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);



	_float4x4 ShaderMat = m_BoneMatrix.TransposeXMatrix();
	m_pShaderCom->Set_RawValue("g_AttechMatrix", &ShaderMat, sizeof(_float4x4));



	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 6, i, "g_BoneMatrices"));
	}


	return _int();
}

_int CUmbrella::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 11, i, "g_BoneMatrices"));

	}
	return _int();
}

void CUmbrella::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
}

_float3 CUmbrella::Get_ColliderPosition()
{
	return m_pColliderCom->Get_ColliderPosition();
}

void CUmbrella::Set_AttackAble(_bool bBool)
{
	if (bBool)
	{
		m_TempAttackAble = bBool;
		m_bIsAttackAble = bBool;
		m_ATABPassedTime = 0;
	}
	else
	{
		m_TempAttackAble = bBool;
		m_ATABPassedTime = 0;

	}
}

HRESULT CUmbrella::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Umbrella), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.6f, 1.6f, 1.6f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-1.604f, 0.587f, -0.915f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));




	return S_OK;
}

CUmbrella * CUmbrella::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUmbrella*	pInstance = new CUmbrella(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CUmbrella");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUmbrella::Clone(void * pArg)
{
	CUmbrella*	pInstance = new CUmbrella(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CUmbrella");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUmbrella::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	
}
