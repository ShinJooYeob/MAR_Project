#include "stdafx.h"
#include "..\public\Protein.h"
#include "Monster.h"




CProtein::CProtein(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CProtein::CProtein(const CProtein & rhs)
	: CWeapon(rhs)
{
}

HRESULT CProtein::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CProtein::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());


	//m_pTransformCom->Scaled_All(_float3(0.3f));

	//static _float3 testFloat3 = _float3(-0.685000f, 0.380000f, -1.084999f);
	//static _float3 RotFloat3 = _float3(217.611099f, -37.699951f, -25.999981f);

	_Matrix			TransformMatrix = XMMatrixScaling(0.3f, 0.3f, 0.3f) *
	XMMatrixRotationX(XMConvertToRadians(217.611099f))*
		XMMatrixRotationY(XMConvertToRadians(-37.699951f))*
		XMMatrixRotationZ(XMConvertToRadians(-25.999981f))*
		XMMatrixTranslation(-0.685000f, 0.380000f, -1.084999f);
	
	m_pTransformCom->Set_Matrix(TransformMatrix);


	m_PassedTime = 0;
	return S_OK;
}

_int CProtein::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;

	m_PassedTime += fDeltaTime;

	if (m_PassedTime > 3.5f)
	{
		Set_IsDead();
		return 0;
	}




	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();



	return _int();
}

_int CProtein::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CProtein::Render()
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

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 8, i));
	}


	return _int();
}

_int CProtein::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 9, i));
	}

	return _int();
}

HRESULT CProtein::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Protein), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

CProtein * CProtein::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CProtein*	pInstance = new CProtein(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CProtein");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CProtein::Clone(void * pArg)
{
	CProtein*	pInstance = new CProtein(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CProtein");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProtein::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	
}
