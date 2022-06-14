#include "stdafx.h"
#include "..\public\ScrechedBlock.h"
#include "Terrain.h"
#include "Player.h"



CScrechedBlock::CScrechedBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CScrechedBlock::CScrechedBlock(const CScrechedBlock & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CScrechedBlock::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CScrechedBlock::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);


		memcpy(&m_tDesc, pArg, sizeof(BREAKEDGAZBODESC));

	}

	FAILED_CHECK(SetUp_Components());

	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
		m_pTransformCom->Scaled_All(_float3(pUtilMgr->RandomFloat(0.5f, 0.8f)));
	}



	m_vRotAxis = (pUtilMgr->RandomFloat3(-10000.f, 10000.f)).Get_Nomalize();;
	m_fRandPower = (pUtilMgr->RandomFloat(10, 15.f));

	m_fJumpPower = pUtilMgr->RandomFloat(2, 4.f);
	m_fMaxTimer = pUtilMgr->RandomFloat(1, 2.f);

	m_pTransformCom->Rotation_CW(m_vRotAxis.XMVector(),XMConvertToRadians(pUtilMgr->RandomFloat(0, 360)));


	m_fStartTimer = 0;


	return S_OK;
}

_int CScrechedBlock::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	if (m_bIsSpout)
	{
		m_fStartTimer += fDeltaTime;
		
		m_pTransformCom->Turn_CW(m_vRotAxis.XMVector(), fDeltaTime);

		m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), m_fRandPower,fDeltaTime);

		_float3 OldPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

		_float EasedHeight = g_pGameInstance->Easing_Return(TYPE_SinOut, TYPE_SinIn, m_tDesc.vPosition.y, m_tDesc.vPosition.y + m_fJumpPower, (_float)m_fStartTimer, (_float)m_fMaxTimer);

		OldPos.y = EasedHeight;

		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, OldPos);

		if (m_fStartTimer > m_fMaxTimer)
		{
			Set_IsDead();
			return 0;
		}
	}

	//m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 15 , fDeltaTime);


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	if (m_bIsOnScreen)
	{
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

		g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

	}


	return _int();
}

_int CScrechedBlock::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();



	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CScrechedBlock::Render()
{
	if (__super::Render() < 0)
		return -1;



	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader_ApplyPivot(m_pShaderCom, "g_WorldMatrix"));

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



	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 1, i));
	}


	return _int();
}

_int CScrechedBlock::LightRender()
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

void CScrechedBlock::Set_Spout()
{
	{ m_bIsSpout = true; m_fStartTimer = 0;

	SOUNDDESC tSoundDesc;

	tSoundDesc.pTransform = m_pTransformCom;
	tSoundDesc.vMinMax = _float2(5, 35);
	tSoundDesc.fTargetSound = 0.08f;
	wstring SoundTrack = L"";
	SoundTrack = L"Executioner_spin_destrcut0" + to_wstring(rand() % 12 + 1) + L".ogg";

	//SoundTrack = L"MapObject_shrinkflower_open.ogg";

	g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);

	};
}

void CScrechedBlock::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
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

HRESULT CScrechedBlock::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(COMPONENTPROTOTYPEID(m_tDesc.MeshKinds)), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(0.06f, 0.08f);
	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(720,1080));
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0.3f, 0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	///* For.Com_AABB */
	//ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : 0.030000f , -10.630148f , -10.410143f , 1
	//size  : 6.080047f , 3.000000f , 3.000000f  
	ColliderDesc.vScale = _float3(4, 1, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));






	return S_OK;
}



CScrechedBlock * CScrechedBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CScrechedBlock*	pInstance = new CScrechedBlock(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CScrechedBlock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CScrechedBlock::Clone(void * pArg)
{
	CScrechedBlock*	pInstance = new CScrechedBlock(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CScrechedBlock");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScrechedBlock::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	
}
