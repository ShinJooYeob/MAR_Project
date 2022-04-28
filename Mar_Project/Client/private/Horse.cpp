#include "stdafx.h"
#include "..\public\Horse.h"




CHorse::CHorse(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CHorse::CHorse(const CHorse & rhs)
	: CWeapon(rhs)
{
}

HRESULT CHorse::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CHorse::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	
	return S_OK;
}

_int CHorse::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	



	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));


	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();


	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, TransformMatrix);


	g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);

	return _int();
}

_int CHorse::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;

	
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CHorse::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG



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

_int CHorse::LateRender()
{

	return _int();
}

HRESULT CHorse::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Horse), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.4f, 1.4f, 1.4f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	ColliderDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	ColliderDesc.vPosition = _float4(-0.61f, -0.01f, -1.32f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vPosition = _float4(-0.85f, 0.08f, -1.305f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();


	ColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	ColliderDesc.vPosition = _float4(0,0,0, 1.f);
	//-0.180000, 0.210000, 0.150000,
	ColliderDesc.vPosition = _float4(-1.03f, 0.29f, -1.155f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	//Pivot: -0.180000, 0.120000, 0.030000,
	//Pivot: -0.480000, 0.180000, 0.030000,


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

CHorse * CHorse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CHorse*	pInstance = new CHorse(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CHorse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHorse::Clone(void * pArg)
{
	CHorse*	pInstance = new CHorse(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHorse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHorse::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	
}
