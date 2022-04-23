#include "stdafx.h"
#include "..\public\SteamPad.h"
#include "Player.h"



CSteamPad::CSteamPad(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMapObject(pDevice,pDeviceContext)
{
}

CSteamPad::CSteamPad(const CSteamPad & rhs)
	: CMapObject(rhs)
{
}

HRESULT CSteamPad::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CSteamPad::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		_float3 Pos = *(_float3*)pArg;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, Pos);
		m_fMaxHight = (*(_float4*)pArg).w;
	}
	m_fRangeRadius = 0.514f;

	return S_OK;
}

_int CSteamPad::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_fAngle += 1080 * _float(fDeltaTime);

	_Vector vPlayer = m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS);
	_Vector vObjet = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
	_float Height = XMVectorGetY(vPlayer) - XMVectorGetY(vObjet);
	if (Height < 0 || Height > m_fMaxHight || 
		XMVectorGetX(XMVector3Length(XMVectorSetY(vObjet,0) - XMVectorSetY(vPlayer, 0))) > m_fRangeRadius )
	{
		m_bIsPlayerCloser = false;
		return _int();
	}

	m_pPlayer->Add_JumpForce(m_fMaxHight);


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	return _int();
}

_int CSteamPad::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();


	//if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CSteamPad::Render()
{
	if (__super::Render() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(__super::SetUp_ConstTable(m_pShaderCom));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < 1; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i));
	}



	//ÇÁ·ÎÆç¶ó
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);
	FAILED_CHECK(Set_ProPellaTransform());
	FAILED_CHECK(SetUp_SubConstTable());


	FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderSubCom, 1, aiTextureType_DIFFUSE, "g_DiffuseTexture"));

	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderSubCom, 2));

	return _int();
}

_int CSteamPad::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;

	return _int();
}

HRESULT CSteamPad::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_ShaderSub), (CComponent**)&m_pShaderSubCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));
	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_SteamPad), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(0.06f, 0.08f);
	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(50, 70));
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, -2.16f, 0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));

	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);


	return S_OK;
}

HRESULT CSteamPad::SetUp_SubConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderSubCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderSubCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	//FAILED_CHECK(pShader->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	//FAILED_CHECK(pShader->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	//const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	//NULL_CHECK_RETURN(pLightDesc, -1);

	//FAILED_CHECK(pShader->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	//FAILED_CHECK(pShader->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	//FAILED_CHECK(pShader->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	//FAILED_CHECK(pShader->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));

	return S_OK;
}

HRESULT CSteamPad::Set_ProPellaTransform()
{

	_Matrix		ShaderWorldMatrix = XMMatrixRotationX(XMConvertToRadians(90))* XMMatrixRotationY(XMConvertToRadians(m_fAngle)) * m_pTransformCom->Get_WorldMatrix();

	ShaderWorldMatrix.r[3] += m_pTransformCom->Get_Pivot();

	_float4x4 ShaderWorldMatrix4x4 = XMMatrixTranspose(ShaderWorldMatrix);

	return m_pShaderSubCom->Set_RawValue("g_WorldMatrix", &ShaderWorldMatrix4x4, sizeof(_float4x4));
}

CSteamPad * CSteamPad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CSteamPad*	pInstance = new CSteamPad(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CSteamPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSteamPad::Clone(void * pArg)
{
	CSteamPad*	pInstance = new CSteamPad(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CSteamPad");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSteamPad::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pModel);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShaderSubCom);
	
	
}
