#include "stdafx.h"
#include "..\public\WireTerrain.h"



CWireTerrain::CWireTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CWireTerrain::CWireTerrain(const CWireTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWireTerrain::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CWireTerrain::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

_int CWireTerrain::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	//m_InverseWorldMat = m_pTransformCom->Get_InverseWorldMatrix();





	return _int();
}

_int CWireTerrain::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	return _int();
}

_int CWireTerrain::Render()
{
	if (__super::Render() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	/*
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


		const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
		NULL_CHECK_RETURN(pLightDesc, -1);


		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));
	*/


	//FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));

	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 2));

	return _int();
}

_int CWireTerrain::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

_float3 CWireTerrain::PutOnTerrain(_bool* pbIsObTerrain,_fVector ObjectWorldPos, _fVector ObjectOldWorldPos, _float3* vOutPlaneNormalVec)
{
	if (XMVectorGetY(ObjectOldWorldPos) < XMVectorGetY(ObjectWorldPos))
		return ObjectWorldPos;

	_Matrix InverMat = m_InverseWorldMat.XMatrix();

	_float3 CaculatedFloat3 = m_pVIBufferCom->Caculate_TerrainY(pbIsObTerrain,
		(XMVector3TransformCoord(ObjectWorldPos, InverMat)), (XMVector3TransformCoord(ObjectOldWorldPos, InverMat)), vOutPlaneNormalVec);


	if (*pbIsObTerrain)
	{
		if (vOutPlaneNormalVec != nullptr)
			*vOutPlaneNormalVec = XMVector3TransformNormal(vOutPlaneNormalVec->XMVector(), m_pTransformCom->Get_WorldMatrix());

		return CaculatedFloat3;
	}

	return ObjectWorldPos;
}

HRESULT CWireTerrain::SetUp_Components()
{


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_EDIT, TAG_CP(Prototype_VIBuffer_Terrain_Edit), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	//FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Texture_Terrain), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));


	CTransform::TRANSFORMDESC tDesc = {};

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	m_InverseWorldMat = m_pTransformCom->Get_InverseWorldMatrix();
	

	return S_OK;
}

CWireTerrain * CWireTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CWireTerrain*	pInstance = new CWireTerrain(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CWireTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWireTerrain::Clone(void * pArg)
{
	CWireTerrain*	pInstance = new CWireTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CWireTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWireTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	//Safe_Release(m_pTextureCom);
	
}
