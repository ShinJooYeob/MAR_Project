#include "stdafx.h"
#include "..\public\HyperVoice.h"
#include "Player.h"




CHyperVoice::CHyperVoice(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CHyperVoice::CHyperVoice(const CHyperVoice & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHyperVoice::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CHyperVoice::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	if (pArg != nullptr)
	{
		memcpy(&m_tDesc, pArg, sizeof(CIRCLETORNADODESC));
	}

	FAILED_CHECK(SetUp_Components());


	m_pTransformCom->LookDir(m_tDesc.vLook.XMVector());
	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
	m_pTransformCom->Scaled_All(_float3(GetSingle(CUtilityMgr)->RandomFloat(1.f, 1.3f)).XMVector() * m_tDesc.fSize);


	m_TargetTime = _double(GetSingle(CUtilityMgr)->RandomFloat(2.f, 2.5f));
	m_PassedTime = 0;
	m_iRand = rand();



	{


		SOUNDDESC tSoundDesc;
		tSoundDesc.pTransform = m_pTransformCom;
		tSoundDesc.vMinMax = _float2(0, 35);
		tSoundDesc.fTargetSound = 0.75f;


		wstring SoundTrack = L"";
		SoundTrack = L"DollMaker_hands_girl_fs_scrape_b.ogg";


		//SoundTrack = L"MapObject_shrinkflower_open.ogg";

		g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
	}
	return S_OK;
}

_int CHyperVoice::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	m_PassedTime += fDeltaTime;

	m_pTransformCom->Move_Forward(fDeltaTime);

	if (m_iRand % 2)
		m_pTransformCom->Turn_CW(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), fDeltaTime);
	else
		m_pTransformCom->Turn_CCW(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), fDeltaTime);

	if (m_PassedTime < m_TargetTime *0.25f)
	{
		m_pTransformCom->Scaling(CTransform::STATE_UP, fDeltaTime);
		m_pTransformCom->Scaling(CTransform::STATE_RIGHT, fDeltaTime);
	}

	if (m_PassedTime > m_TargetTime)Set_IsDead();



	for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
		m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

	g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

#ifdef _DEBUG
	FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

	return _int();
}

_int CHyperVoice::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this));


	return 0;
}

_int CHyperVoice::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(SetUp_ConstTable());
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vMixColor", &_float4(1, 0.6f, 1, 1), sizeof(_float4)));


	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 0));
	FAILED_CHECK(m_pShaderCom->Set_Texture("g_BackBufferTexture", g_pGameInstance->Get_SRV(L"Target_AfterDefferred")));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 11, i));
	}

	return _int();
}

_int CHyperVoice::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;




	return _int();
}

void CHyperVoice::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Alice"))
		{
			pConflictedCollider->Set_Conflicted();
			((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);
		}

	}
	break;
	case Engine::CollisionType_Terrain:
		break;

	default:
		break;
	}
}

HRESULT CHyperVoice::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"BlurEffect");

	CTransform::TRANSFORMDESC tDesc = {};

	CUtilityMgr* pUtil = GetSingle(CUtilityMgr);
	tDesc.fMovePerSec = m_tDesc.Power;
	tDesc.fRotationPerSec = XMConvertToRadians(pUtil->RandomFloat(1080, 1440));
	tDesc.fScalingPerSec = 3.f;
	//tDesc.vPivot = _float3(0, 0.3f, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	_Vector RotAxis = (pUtil->RandomFloat3(-100, 100)).Get_Nomalize();
	_float Angle = pUtil->RandomFloat(-10, 10);

	m_pTransformCom->Rotation_CW(RotAxis, XMConvertToRadians(Angle));
	Angle = pUtil->RandomFloat(0, 360);
	m_pTransformCom->Turn_CW(XMVectorSet(0,1,0,0), XMConvertToRadians(Angle));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_CircleMesh), TAG_COM(Com_Model), (CComponent**)&m_pModel));




	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	//Pivot  : -0.025000f , -0.020000f , -3.594998f , 1
	//size  : 7.320073f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(1.f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));




	return S_OK;
}

HRESULT CHyperVoice::SetUp_ConstTable()
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

CHyperVoice * CHyperVoice::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CHyperVoice*	pInstance = new CHyperVoice(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CHyperVoice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHyperVoice::Clone(void * pArg)
{
	CHyperVoice*	pInstance = new CHyperVoice(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CHyperVoice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHyperVoice::Free()
{
	__super::Free();

	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

	
}
