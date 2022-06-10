#include "stdafx.h"
#include "..\public\EyepotChainGranade.h"
#include "Terrain.h"
#include "Player.h"
#include "CircleTornado.h"
#include "DirTornadoSwirl.h"



CEyepotChainGranade::CEyepotChainGranade(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonsterWeapon(pDevice,pDeviceContext)
{
}

CEyepotChainGranade::CEyepotChainGranade(const CEyepotChainGranade & rhs)
	: CMonsterWeapon(rhs)
{
}

HRESULT CEyepotChainGranade::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));
	return S_OK;
}

HRESULT CEyepotChainGranade::Initialize_Clone(void * pArg)
{
	//FAILED_CHECK(__super::Initialize_Clone(pArg));

	if (pArg != nullptr)
	{

		m_pPlayer = (CPlayer*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Player)));

		NULL_CHECK_RETURN(m_pPlayer, E_FAIL);
		memcpy(&m_tDesc, pArg, sizeof(ECGDESC));

	}

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.vPosition);
	}

	m_fStartTimer = 0;
	m_bGonnabeDie = false;
	m_DyingTime = 0;
	CUtilityMgr* pUtilMgr = GetSingle(CUtilityMgr);

	if (m_tDesc.MeshKinds)
	{
		m_RandHeight = pUtilMgr->RandomFloat(2.5f, 3.5f);
	}
	else
	{
		m_RandHeight = pUtilMgr->RandomFloat(2.5f, 3.5f);
	}


	for (_uint i = 0 ; i< 20; i++)
	{
		m_PassedPositionList.push_back(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
	}


	FAILED_CHECK(Ready_ParticleDesc());
	m_fParticleTimer = 1.f;
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	return S_OK;
}

_int CEyepotChainGranade::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;

	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	//m_pTransformCom->MovetoDir_bySpeed(m_tDesc.MoveDir.XMVector(), 15 , fDeltaTime);
	if (m_bGonnabeDie)
	{
		m_DyingTime += fDeltaTime;
		_float EasedScale = g_pGameInstance->Easing_Return(TYPE_Linear, TYPE_Linear, 1, 4,(_float) m_DyingTime, 0.35f);
		m_pTransformCom->Scaled_All(_float3(EasedScale));
		if (m_DyingTime > 0.35f)
		{
			Set_IsDead();
		}
	}
	else
	{



		m_fStartTimer += fDeltaTime;
		m_fParticleTimer += fDeltaTime;

		if (m_fParticleTimer > 0.2f)
		{

			GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 5);
			m_fParticleTimer = 0;
		}

		if (m_tDesc.MeshKinds)
		{
			m_pTransformCom->MovetoDir(m_tDesc.MoveDir.XMVector(), fDeltaTime);

			_float3 vPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_float Height = 0;

			if (m_fStartTimer < 0.25f)
			{
				Height = g_pGameInstance->Easing(TYPE_QuadOut, m_tDesc.vPosition.y, m_tDesc.vPosition.y + m_RandHeight, (_float)m_fStartTimer, 0.45f);
				vPos.y = Height;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
			}
			else if (m_fStartTimer < 2.f)
			{
				Height = g_pGameInstance->Easing(TYPE_QuadIn, m_tDesc.vPosition.y + m_RandHeight, m_tDesc.vPosition.y - m_RandHeight * 0.5f, (_float)m_fStartTimer - 0.25f, 0.25f);
				vPos.y = Height;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);

			}
			else
			{


				CUtilityMgr* pUtil = GetSingle(CUtilityMgr);


				m_tParticleDesc.FixedTarget = m_pColliderCom->Get_ColliderPosition();
				pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


				pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
				pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



				CCircleTornado::CIRCLETORNADODESC tDesc2;
				tDesc2.vLook = _float3(0.00000001f, 1.f, 0);
				tDesc2.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.1f, 0, 0);
				tDesc2.fSize = 1.5f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc2);
				tDesc2.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
				tDesc2.fSize = 1;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc2);


				m_bGonnabeDie = true;
				m_DyingTime = 0;
			}


		}
		else
		{
			m_pTransformCom->MovetoDir(m_tDesc.MoveDir.XMVector(), fDeltaTime);

			_float3 vPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			_float Height = 0;

			if (m_fStartTimer < 0.25f)
			{
				Height = g_pGameInstance->Easing(TYPE_QuadOut, m_tDesc.vPosition.y, m_tDesc.vPosition.y + m_RandHeight, (_float)m_fStartTimer, 0.25f);
				vPos.y = Height;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
			}
			else if (m_fStartTimer < 0.5f)
			{
				Height = g_pGameInstance->Easing(TYPE_QuadIn, m_tDesc.vPosition.y + m_RandHeight, m_tDesc.vPosition.y - (m_RandHeight), (_float)m_fStartTimer - 0.25f, 0.25f);
				vPos.y = Height;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);

			}
			else
			{
				ECGDESC tDesc;

				tDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tDesc.MeshKinds = 1;

				_Vector vLook = XMVector3Normalize(m_tDesc.MoveDir.XMVector());
				_Vector vRight = XMVector3Normalize(_float3(0, 1, 0).Get_Cross(m_tDesc.MoveDir.XMVector()));

				tDesc.MoveDir = XMVector3Normalize(XMVectorSetY(vLook * 0.5f + vRight * 0.5f, 0));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"ProtoType_EyepotChainGranade", &tDesc);

				tDesc.MoveDir = XMVector3Normalize(XMVectorSetY(vLook * 0.5f - vRight * 0.5f, 0));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"ProtoType_EyepotChainGranade", &tDesc);

				tDesc.MoveDir = XMVector3Normalize(XMVectorSetY(vLook * -0.5f + vRight * 0.5f, 0));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"ProtoType_EyepotChainGranade", &tDesc);

				tDesc.MoveDir = XMVector3Normalize(XMVectorSetY(vLook * -0.5f - vRight * 0.5f, 0));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"ProtoType_EyepotChainGranade", &tDesc);




				CUtilityMgr* pUtil = GetSingle(CUtilityMgr);


				m_tParticleDesc.FixedTarget = m_pColliderCom->Get_ColliderPosition();
				pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


				pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
				pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



				CCircleTornado::CIRCLETORNADODESC tDesc2;
				tDesc2.vLook = _float3(0.00000001f, 1.f, 0);
				tDesc2.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.1f, 0, 0);
				tDesc2.fSize = 1.5f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc2);
				tDesc2.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
				tDesc2.fSize = 1;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc2);



				m_bGonnabeDie = true;
				m_DyingTime = 0;
			}

		}

		CDirTornadoSwirl::DIRSWIRLDESC tDirDesc;
		tDirDesc.vRotateDir = XMVector3Normalize(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) - m_vOldPos.XMVector());
		tDirDesc.vLookDir = -m_tDesc.MoveDir.XMVector();
		tDirDesc.fScale = 0.7f;
		tDirDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

		tDirDesc.vColor = _float4(0.231372549019f, GetSingle(CUtilityMgr)->RandomFloat(0.705882352941f, 0.90196078431372f), 1.f, 1);

		g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_DirTornado), &tDirDesc);

	}

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

_int CEyepotChainGranade::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	//if (!m_bIsPlayerCloser) return _int();

	FAILED_CHECK(Set_Bullet_On_Terrain());

	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND_NOLIGHT, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);


	m_PassedPositionList.pop_back();
	m_PassedPositionList.push_front(m_vOldPos);

	
	auto Points0 = m_PassedPositionList.begin();
	auto Points1 = m_PassedPositionList.begin();
	Points1++;
	auto Points2 = m_PassedPositionList.begin();
	Points1++;
	Points2++;
	auto Points3 = m_PassedPositionList.begin();
	Points1++;
	Points2++;
	Points3++;

#define TargetRate 0.5f
	for (_uint i = 1; i < m_PassedPositionList.size() - 2; i++)
	{
		(*Points1) = XMVectorCatmullRom(Points0->XMVector(), Points1->XMVector(), Points2->XMVector(), Points3->XMVector(), TargetRate);

		Points0++;
		Points1++;
		Points2++;
		Points3++;
	}


	return _int();
}

_int CEyepotChainGranade::Render()
{
	if (__super::Render() < 0)
		return -1;



	NULL_CHECK_RETURN(m_pModel, E_FAIL);



	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vMixColor", &_float4(1, 0.6f, 1, 1), sizeof(_float4)));
	_float ShaderTimer = (_float)m_fStartTimer;
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fDeltaTime", &ShaderTimer, sizeof(_float)));



	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_NoiseTexture", 0));
	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture", 6));
	FAILED_CHECK(m_pShaderCom->Set_Texture("g_BackBufferTexture", g_pGameInstance->Get_SRV(L"Target_AfterDefferred")));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	_uint iCount = 1;
	for (auto& vPos : m_PassedPositionList)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, vPos);
		m_pTransformCom->Scaled_All(_float3( (21 -iCount) /20.f ));
		
		FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	
		for (_uint i = 0; i < NumMaterial; i++)
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 13, i));
		}
		iCount++;
	}

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_PassedPositionList.front());
	m_pTransformCom->Scaled_All(_float3(1.f));


	return _int();
}

_int CEyepotChainGranade::LightRender()
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

void CEyepotChainGranade::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
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

HRESULT CEyepotChainGranade::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_BallMesh), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_ScreenEffectUI), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"BallEffect");

	CTransform::TRANSFORMDESC tDesc = {};

	if (m_tDesc.MeshKinds)
		tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(10.f, 15.f);
	else
		tDesc.fMovePerSec = GetSingle(CUtilityMgr)->RandomFloat(20.f, 25.f);

	tDesc.fRotationPerSec = XMConvertToRadians(GetSingle(CUtilityMgr)->RandomFloat(50,70));
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0.3f, 0);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	///* For.Com_AABB */
	//ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : 0.030000f , -10.630148f , -10.410143f , 1
	//size  : 6.080047f , 3.000000f , 3.000000f  
	ColliderDesc.vScale = _float3(2, 1, 1);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0, 0, 0, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));





	return S_OK;
}

HRESULT CEyepotChainGranade::Set_Bullet_On_Terrain()
{
	if (m_bGonnabeDie) return S_FALSE;
	CGameInstance* pInstance = GetSingle(CGameInstance);

	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_uint eNowTile = Tile_End;
	_float3 CaculatedPos = pTerrain->PutOnTerrain_Stage2(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(), nullptr, &eNowTile);



	if (eNowTile == Tile_None || eNowTile == Tile_DynamicNoneTile || bIsOn)
	{

		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		CUtilityMgr* pUtil = GetSingle(CUtilityMgr);


		m_tParticleDesc.FixedTarget = CaculatedPos;
		pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


		pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
		pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



		CCircleTornado::CIRCLETORNADODESC tDesc2;
		tDesc2.vLook = _float3(0.00000001f, 1.f, 0);
		tDesc2.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0,0.1f,0,0);
		tDesc2.fSize = 1.5f;
		g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc2);
		tDesc2.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
		tDesc2.fSize = 1;
		g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc2);



		m_bGonnabeDie = true;
		m_DyingTime = 0;
	}

	return S_OK;
}

HRESULT CEyepotChainGranade::Ready_ParticleDesc()
{
	/////////5///////////////////////////////////////////////////////////////
	m_tParticleDesc = PARTICLEDESC();

	m_tParticleDesc.eParticleTypeID = Particle_Ball;

	m_tParticleDesc.FollowingTarget = nullptr;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Explosion2";
	m_tParticleDesc.iSimilarLayerNum = 1;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(4, 4);

	m_tParticleDesc.TotalParticleTime = 0.1f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;
	m_tParticleDesc.MaxParticleCount = 30;

	m_tParticleDesc.SizeChageFrequency = 1;
	m_tParticleDesc.ParticleSize = _float3(0.15f, 0.15f, 0.15f);
	m_tParticleDesc.ParticleSize2 = _float3(5.f, 5.f, 5.f);

	m_tParticleDesc.ColorChageFrequency = 1;
	m_tParticleDesc.TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
	m_tParticleDesc.TargetColor2 = _float4(0.f);


	m_tParticleDesc.Particle_Power = 4;
	m_tParticleDesc.PowerRandomRange = _float2(0.8f, 1.5f);

	m_tParticleDesc.vUp = _float3(0, 1, 0);

	m_tParticleDesc.MaxBoundaryRadius = 6;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(0.f, 0, 0.f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0, 0, 0);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc.m_iPassIndex = 10;



	return S_OK;
}


CEyepotChainGranade * CEyepotChainGranade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEyepotChainGranade*	pInstance = new CEyepotChainGranade(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CEyepotChainGranade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEyepotChainGranade::Clone(void * pArg)
{
	CEyepotChainGranade*	pInstance = new CEyepotChainGranade(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CEyepotChainGranade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEyepotChainGranade::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);

	
}
