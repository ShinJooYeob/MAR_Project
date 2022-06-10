#include "stdafx.h"
#include "..\public\DirTornadoSwirl.h"




CDirTornadoSwirl::CDirTornadoSwirl(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDirTornadoSwirl::CDirTornadoSwirl(const CDirTornadoSwirl & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDirTornadoSwirl::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CDirTornadoSwirl::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(DIRSWIRLDESC));
	}
	

	FAILED_CHECK(SetUp_Components());


	
	m_TargetTime = _double(GetSingle(CUtilityMgr)->RandomFloat(0.3f, 0.5f));
	m_PassedTime = 0;
	m_iRand = rand();
	return S_OK;
}

_int CDirTornadoSwirl::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	m_PassedTime += fDeltaTime;

	if (m_iRand % 2)
		m_pTransformCom->Turn_CW(m_tDesc.vRotateDir.XMVector(), fDeltaTime);
	else
		m_pTransformCom->Turn_CCW(m_tDesc.vRotateDir.XMVector(), fDeltaTime);

	m_pTransformCom->Scaling(CTransform::STATE_LOOK, fDeltaTime);
	m_pTransformCom->Scaling(CTransform::STATE_RIGHT, fDeltaTime);

	if (m_PassedTime > m_TargetTime)Set_IsDead();


	return _int();
}

_int CDirTornadoSwirl::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this));
	return 0;
}

_int CDirTornadoSwirl::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(SetUp_ConstTable());
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vMixColor", &m_tDesc.vColor, sizeof(_float4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 5, i));
	}

	return _int();
}

_int CDirTornadoSwirl::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;




	return _int();
}

HRESULT CDirTornadoSwirl::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	CTransform::TRANSFORMDESC tDesc = {};

	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);
	tDesc.fMovePerSec = 1;
	tDesc.fRotationPerSec = XMConvertToRadians(pUtil->RandomFloat(1080, 1440));
	tDesc.fScalingPerSec = 0.8f;
	//tDesc.vPivot = _float3(0, 0.3f, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);

	//m_pTransformCom->LookAt(m_tDesc.vPosition.XMVector() + m_tDesc.vLookDir.XMVector());
	//_Vector RotAxis = (pUtil->RandomFloat3(-100, 100)).Get_Nomalize();
	//_float Angle = pUtil->RandomFloat(-10, 10);
	//
	m_pTransformCom->Rotation_CW(XMVector3Cross(XMVectorSet(0,1,0,0),m_tDesc.vRotateDir.XMVector()), XMConvertToRadians(90));
	//Angle = pUtil->RandomFloat(0, 360);
	//m_pTransformCom->Turn_CW(XMVectorSet(0,1,0,0), XMConvertToRadians(Angle));
	m_pTransformCom->Scaled_All(_float3(m_tDesc.fScale* pUtil->RandomFloat(0.7f, 1)));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(Prototype_Mesh_Tornado1 + 2+ rand() % 2)), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	return S_OK;
}

HRESULT CDirTornadoSwirl::SetUp_ConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	//const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	//NULL_CHECK_RETURN(pLightDesc, -1);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));


	return S_OK;
}

CDirTornadoSwirl * CDirTornadoSwirl::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CDirTornadoSwirl*	pInstance = new CDirTornadoSwirl(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CDirTornadoSwirl");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDirTornadoSwirl::Clone(void * pArg)
{
	CDirTornadoSwirl*	pInstance = new CDirTornadoSwirl(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CDirTornadoSwirl");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDirTornadoSwirl::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
}
