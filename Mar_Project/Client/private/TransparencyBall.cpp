#include "stdafx.h"
#include "..\public\TransparencyBall.h"
#include "HandyGirl.h"




CTransparencyBall::CTransparencyBall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTransparencyBall::CTransparencyBall(const CTransparencyBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTransparencyBall::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CTransparencyBall::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));



	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(BALLMESHDESC));

		m_pTransformCom->LookDir(_float3(GetSingle(CUtilityMgr)->RandomFloat(-999.f, 999.f)).Get_Nomalize());
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
		m_pTransformCom->Scaled_All(_float3(0.1f).XMVector());

	}

	m_TargetTime = _double(GetSingle(CUtilityMgr)->RandomFloat(0.3f, 0.5f));
	m_PassedTime = 0;
	m_iRand = rand();

	FAILED_CHECK(Ready_ParticleDesc());

	return S_OK;
}

_int CTransparencyBall::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	m_ShaderTime += fDeltaTime;
	m_PassedTime += fDeltaTime;
	if (m_PassedTime > 0.1f)
	{	
		m_tParticleDesc.FixedTarget = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
		GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);
		m_PassedTime = 0;
	}

	if (m_ShaderTime > 1)m_ShaderTime = 0;

	if (m_iRand % 2)
		m_pTransformCom->Turn_CW(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), fDeltaTime);
	else
		m_pTransformCom->Turn_CCW(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), fDeltaTime);




	if (m_tDesc.HandyGirl->Get_NowModelAnimIndex() == 10)
	{
		_float fScaled = g_pGameInstance->Easing(TYPE_SinInOut, 0.1f, 0.8f, (_float)m_tDesc.HandyGirl->Get_PlayRate(), 1);
		m_pTransformCom->Scaled_All(_float3(fScaled));
	}
	else if (m_tDesc.HandyGirl->Get_NowModelAnimIndex() == 11)
	{
		_float fScaled = g_pGameInstance->Easing(TYPE_ExpoIn, 0.8f, 0.01f, (_float)m_tDesc.HandyGirl->Get_PlayRate(), 1);
		m_pTransformCom->Scaled_All(_float3(fScaled));

		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.HandyGirl->Get_ColliderPosition(8));
		
	}
	else
	{
		Set_IsDead();
	}

	//m_pTransformCom->Scaling(CTransform::STATE_UP, fDeltaTime);
	//m_pTransformCom->Scaling(CTransform::STATE_RIGHT, fDeltaTime);

	//if (m_PassedTime > m_TargetTime)Set_IsDead();


	return _int();
}

_int CTransparencyBall::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this));
	return 0;
}

_int CTransparencyBall::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(SetUp_ConstTable());
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vMixColor", &_float4(1, 0.6f, 1, 1), sizeof(_float4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fDeltaTime", &m_ShaderTime, sizeof(_float)));

	
	
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_NoiseTexture", 0));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 6));
	FAILED_CHECK(m_pShaderCom->Set_Texture("g_BackBufferTexture", g_pGameInstance->Get_SRV(L"Target_AfterDefferred")));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 12, i));
	}

	return _int();
}

_int CTransparencyBall::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;




	return _int();
}

HRESULT CTransparencyBall::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"BallEffect");

	CTransform::TRANSFORMDESC tDesc = {};

	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);
	tDesc.fMovePerSec = 1;
	tDesc.fRotationPerSec = XMConvertToRadians(pUtil->RandomFloat(1080, 1440));
	tDesc.fScalingPerSec = 3.f;
	//tDesc.vPivot = _float3(0, 0.3f, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	_Vector RotAxis = (pUtil->RandomFloat3(-100, 100)).Get_Nomalize();
	_float Angle = pUtil->RandomFloat(-10, 10);

	m_pTransformCom->Rotation_CW(RotAxis, XMConvertToRadians(Angle));
	Angle = pUtil->RandomFloat(0, 360);
	m_pTransformCom->Turn_CW(XMVectorSet(0,1,0,0), XMConvertToRadians(Angle));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_BallMesh), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	return S_OK;
}

HRESULT CTransparencyBall::SetUp_ConstTable()
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

HRESULT CTransparencyBall::Ready_ParticleDesc()
{
	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Ball;

	m_tParticleDesc.FollowingTarget = nullptr;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Petal";
	m_tParticleDesc.iSimilarLayerNum = 3;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(8, 4);

	m_tParticleDesc.TotalParticleTime = 0.1f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;
	m_tParticleDesc.MaxParticleCount = 7;

	m_tParticleDesc.SizeChageFrequency = 0;
	m_tParticleDesc.ParticleSize = _float3(0.25f, 0.25f, 0.25f);
	m_tParticleDesc.ParticleSize2 = _float3(0.5f, 0.5f, 0.5f);

	m_tParticleDesc.ColorChageFrequency = 0;
	m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 1.f);


	m_tParticleDesc.Particle_Power = 2;
	m_tParticleDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	m_tParticleDesc.vUp = _float3(0, 1, 0);

	m_tParticleDesc.MaxBoundaryRadius = 6;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.25f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.25f);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = false;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 3;




	return S_OK;
}

CTransparencyBall * CTransparencyBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CTransparencyBall*	pInstance = new CTransparencyBall(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CTransparencyBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTransparencyBall::Clone(void * pArg)
{
	CTransparencyBall*	pInstance = new CTransparencyBall(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CTransparencyBall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransparencyBall::Free()
{
	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
}
