#include "stdafx.h"
#include "..\public\Terrain.h"



CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CTerrain::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

_int CTerrain::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;






	return _int();
}

_int CTerrain::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	return _int();
}

_int CTerrain::Render()
{
	if (__super::Render() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	_float4x4		ViewFloat4x4 = pInstance->Get_Transform_Float4x4_TP(PLM_VIEW);
	_float4x4		ProjFloat4x4 = pInstance->Get_Transform_Float4x4_TP(PLM_PROJ);
	_float3			vCamPos  = pInstance->Get_TargetPostion_float3(PLV_CAMERA);
	_float3			vCamLookDir = pInstance->Get_TargetPostion_float3(PLV_CAMLOOK);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewFloat4x4, sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjFloat4x4, sizeof(_float4x4)));


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &vCamPos, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &vCamLookDir, sizeof(_float3)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDir", &vLightDir, sizeof(_float3)));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));



	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CTerrain::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

_float3 CTerrain::PutOnTerrain(_bool* pbIsObTerrain,_fVector ObjectWorldPos)
{
	_float CaculatedY = m_pVIBufferCom->Caculate_TerrainY(pbIsObTerrain, 
		XMVector3TransformCoord(ObjectWorldPos, m_pTransformCom->Get_InverseWorldMatrix()));
	_float3 vResult = ObjectWorldPos;

	if (*pbIsObTerrain)
	{
		vResult.y = CaculatedY;
		return vResult;
	}

	return vResult;
}

HRESULT CTerrain::SetUp_Components()
{


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_VIBuffer_Terrain_1), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Texture_Terrain), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));


	CTransform::TRANSFORMDESC tDesc = {};

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CTerrain*	pInstance = new CTerrain(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	
}
