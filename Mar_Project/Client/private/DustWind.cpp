#include "stdafx.h"
#include "..\public\DustWind.h"
#include "Player.h"



CDustWind::CDustWind(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDustWind::CDustWind(const CDustWind & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDustWind::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CDustWind::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(DUSTWINDDESC));
	}
	
	FAILED_CHECK(SetUp_Components());

	

	m_PassedTime = 0;
	m_iRand = rand();
	return S_OK;
}

_int CDustWind::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	m_PassedTime += fDeltaTime;

	if (m_iRand % 2)
		m_pTransformCom->Turn_CW(XMVectorSet(0,1,0,0), fDeltaTime);
	else
		m_pTransformCom->Turn_CW(XMVectorSet(0, -1, 0, 0), fDeltaTime);


	if (m_PassedTime < m_tDesc.ToTalLifeTime)
	{
		_float3 EasedScale = g_pGameInstance->Easing_Vector(m_tDesc.eEasingType, m_tDesc.StartScale, m_tDesc.TargetScale, (_float)m_PassedTime, (_float)m_tDesc.ToTalLifeTime);
		m_pTransformCom->Scaled_All(EasedScale);

	}
	else {
		Set_IsDead();
	}


	//m_pTransformCom->Scaling(CTransform::STATE_LOOK, fDeltaTime);
	//m_pTransformCom->Scaling(CTransform::STATE_RIGHT, fDeltaTime);
	//
	//if (m_PassedTime > m_TargetTime)Set_IsDead();

	_float DistBtwPlayer = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).Get_Distance(m_pPlayerTransform->Get_MatrixState(CTransform::STATE_POS));
	_float Scale = XMVectorGetX(m_pTransformCom->Get_MatrixScale(CTransform::STATE_RIGHT));

	if (DistBtwPlayer >= 2 * Scale && DistBtwPlayer <= 3 * Scale)
	{
		CCollider* pPlayerCollider =((CCollider*)(m_pPlayer->Get_Component(TAG_COM(Com_Collider))));
		if (pPlayerCollider != nullptr && !pPlayerCollider->Get_Conflicted())
		{
			pPlayerCollider->Set_Conflicted();
			m_pPlayer->Add_Dmg_to_Player(1);
		}
	}


	return _int();
}

_int CDustWind::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this));
	return 0;
}

_int CDustWind::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vMixColor", &m_tDesc.vColor, sizeof(_float4)));
	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(SetUp_ConstTable());


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 5, i));
	}

	return _int();
}

_int CDustWind::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

HRESULT CDustWind::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	CTransform::TRANSFORMDESC tDesc = {};

	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);
	tDesc.fMovePerSec = 1;
	tDesc.fRotationPerSec = XMConvertToRadians(pUtil->RandomFloat(1080, 1440));
	//tDesc.vPivot = _float3(0, 0.3f, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	_Vector RotAxis = (pUtil->RandomFloat3(-100, 100)).Get_Nomalize();
	_float Angle = pUtil->RandomFloat(-10, 10);

	m_pTransformCom->Rotation_CW(RotAxis, XMConvertToRadians(Angle));
	Angle = pUtil->RandomFloat(0, 360);
	m_pTransformCom->Turn_CW(XMVectorSet(0,1,0,0), XMConvertToRadians(Angle));
	m_pTransformCom->Scaled_All(m_tDesc.StartScale);
	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
	

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_DustTornado), TAG_COM(Com_Model), (CComponent**)&m_pModel));



	m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(SCENE_STATIC, TAG_LAY(Layer_Player)));
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
	m_pPlayerTransform = (CTransform*)(m_pPlayer->Get_Component(TAG_COM(Com_Transform)));

	NULL_CHECK_RETURN(m_pPlayerTransform, E_FAIL);



	return S_OK;
}

HRESULT CDustWind::SetUp_ConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	NULL_CHECK_RETURN(pLightDesc, -1);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));


	return S_OK;
}

CDustWind * CDustWind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CDustWind*	pInstance = new CDustWind(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CDustWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDustWind::Clone(void * pArg)
{
	CDustWind*	pInstance = new CDustWind(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CDustWind");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDustWind::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
}
