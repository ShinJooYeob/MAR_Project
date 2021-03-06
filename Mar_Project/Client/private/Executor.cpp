#include "stdafx.h"
#include "..\public\Executor.h"
#include "Scythe.h"
#include "Terrain.h"
#include "EntireCard.h"
#include "BreakedGazebo.h"
#include "ExecutorThron.h"
#include "DustWind.h"
#include "DustWindCurvedMove.h"
#include "CardPeice.h"
#include "CircleTornado.h"


CExecutor::CExecutor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CExecutor::CExecutor(const CExecutor & rhs)
	: CMonster(rhs)
{
}

HRESULT CExecutor::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CExecutor::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());


	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	m_fHP = m_fMaxHP = 64;

	m_pModel->Change_AnimIndex(2);


	FAILED_CHECK(SetUp_Weapon());
	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);

	m_bDeathAnimStart = false;



	return S_OK;
}

_int CExecutor::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);
	Update_DmgCalculate(fDeltaTime);

	


	//if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	//	m_pModel->Change_AnimIndex(0);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
		Add_Dmg_to_Monster(10);

	//m_pModel->Change_AnimIndex(16) // ??????
	//walk 2.8 / 0.05f

	//run 5.5 / 0.1
	if (m_bDeathAnimStart)
	{
		m_DeathAnimPassedTime += fDeltaTime;

		{
			m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 7.8f);
			FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime * ((m_bIsBerseked) ? 1.5f : 1.f), m_bIsOnScreen));
			FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

			if (m_bIsOnScreen)
			{

				_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr.pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr.pDefaultPivotMat);

				TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
				TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
				TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

				for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
					m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());

				m_vecWeapon[0]->Update(fDeltaTime);
			}

		}


		if (m_pModel->Get_NowAnimIndex() == 19 && m_pModel->Get_PlayRate() > 0.95f)
		{

			for (_uint i = 0; i <= 20; i++)
			{
				CCardPiece::CARDPIECEDESC tDesc;

				tDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tDesc.MeshKinds = rand() % 4 + Prototype_Mesh_CardPiece1;
				tDesc.MoveDir = _float3(0,1,0);

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_CardPiece), &tDesc);


			}


			PARTICLEDESC m_tParticleDesc;

			m_tParticleDesc.eParticleTypeID = Particle_Cone;

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
			m_tParticleDesc.ParticleSize = _float3(1.5f, 1.5f, 1.5f);
			m_tParticleDesc.ParticleSize2 = _float3(50.f, 50.f, 50.f);

			m_tParticleDesc.ColorChageFrequency = 1;
			m_tParticleDesc.TargetColor = _float4(1.f, 0.643f, 0.141f, 1.f);
			m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.f);


			m_tParticleDesc.Particle_Power = 4;
			m_tParticleDesc.PowerRandomRange = _float2(8.f, 15.0f);

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


			CUtilityMgr* pUtil = GetSingle(CUtilityMgr);


			m_tParticleDesc.FixedTarget = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);






			CCircleTornado::CIRCLETORNADODESC tDesc;
			tDesc.vLook = _float3(0.00000001f, 1.f, 0);

			tDesc.vPosition = m_tParticleDesc.FixedTarget;
			tDesc.fSize = 7.5f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
			tDesc.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
			tDesc.fSize = 5.f;
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);

			pUtil->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition, 6);
			pUtil->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition, 7);
			pUtil->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 6);
			pUtil->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 7);

			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(5, 80);
				tSoundDesc.fTargetSound = 0.5f;
				wstring SoundTrack = L"";
				SoundTrack = L"Executioner_portal01.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}
			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(5, 80);
				tSoundDesc.fTargetSound = 0.5f;
				wstring SoundTrack = L"";
				SoundTrack = L"MapObject_meralice_shock01.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}
			
			Set_IsDead();
		}
	}
	else
	{

		if (m_bSpwanAnimFinished)
		{
			if (m_pModel->Get_NowAnimIndex() != 19 && m_pModel->Get_NowAnimIndex() != 15)
			{

				if (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 20)
				{
					Update_Pattern(fDeltaTime);
				}
				else
				{
					if (!m_pModel->Get_IsUntillPlay())
					{
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);
						FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, (m_bIsBerseked) ? 0.1f : 0.05f));
					}
				}

			}

		}
		else
		{
			////////////////////////////////////////////////////////////////////////////
			//m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(128, 27.39f, 226.782f));
			//m_bSpwanAnimFinished = true;
			//return 0;
			////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////
			m_SpwanPassedTime += fDeltaTime;

			if (m_SpwanPassedTime < 10)
			{
				_double PlayRate = m_pModel->Get_PlayRate();
				static _uint iStartCounter = 0;
				if (PlayRate < 0.1)
				{
					iStartCounter = 0;
				}
				else if (!iStartCounter  && PlayRate > 0.34 && m_pModel->Get_NowAnimIndex() == 2)
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(0.2f));

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(5, 80);
						tSoundDesc.fTargetSound = 0.5f + (_float)m_SpwanPassedTime * 0.1f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}


					iStartCounter++;
				}
				else if (iStartCounter == 1 && PlayRate > 0.84&& m_pModel->Get_NowAnimIndex() == 2)
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(0.2f));
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(5, 80);
						tSoundDesc.fTargetSound = 0.5f + (_float)m_SpwanPassedTime * 0.1f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}


					iStartCounter++;
				}

				_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_Linear, _float3(128, 13.28f, 256), _float3(128, 24.28f, 244), (_float)m_SpwanPassedTime, 10);


				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
				m_pTransformCom->LookDir(XMVectorSet(0, 0, -1, 0));
				//_float3(128, 13.28f, 256)
				//_float3(128, 23.28f, 244)

				//46.79f,236.659f
				static _bool Jump = false;

				if (m_SpwanPassedTime > 9 && !Jump)
				{
					Jump = true;

					m_pModel->Change_AnimIndex_UntilNReturn_Must(16, 17, 17, 0.15, true);
				}




			}
			else if (m_SpwanPassedTime < 11)
			{
				static _bool Sound = false;
				if (!Sound)
					//if (!Sound && m_pModel->Get_NowAnimIndex() == 16 && m_pModel->Get_PlayRate() > 0.8f)
				{
					Sound = true;
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(5.f, 80.f);
						tSoundDesc.fTargetSound = 0.35f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_swing02.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

				}

				_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoOut, _float3(128, 24.28f, 244), _float3(128, 46.79f, 236.659f), (_float)m_SpwanPassedTime - 10, 1);
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);
			}
			else if (m_SpwanPassedTime < 11.5)
			{
				_float3 EasedPos = g_pGameInstance->Easing_Vector(TYPE_ExpoIn, _float3(128, 46.79f, 236.659f), _float3(128, 27.39f, 226.782f), (_float)m_SpwanPassedTime - 11, 0.5);
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, EasedPos);

				static _bool Land = false;

				if (m_SpwanPassedTime > 11.35 && !Land)
				{
					Land = true;


					m_pModel->Change_AnimIndex(18);
				}
			}
			else if (m_SpwanPassedTime < 14)
			{
				static _uint iChecker = 0;

				if (m_pModel->Get_PlayRate() > 0.08 && !iChecker && m_pModel->Get_NowAnimIndex() == 18)
					//if (m_pModel->Get_PlayRate() > 0.1568627 && !iChecker && m_pModel->Get_NowAnimIndex() == 18)
				{
					iChecker++;
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.5f, _float4(1.f));

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(1000.f, 10000.f);
						tSoundDesc.fTargetSound = 0.35f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_Stomp_01.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					CGameInstance* pInstance = g_pGameInstance;
					CBreakedGazebo::BREAKEDGAZBODESC tDesc;
					tDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					for (_uint i = 0; i < 20; i++)
					{
						tDesc.MeshKinds = rand() % 2 + Prototype_QBattleTowerParticleA;
						pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_BreakedGazbo), &tDesc);
					}


					GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 6);
					GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 7);
					GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector(), 6);
					GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector(), 7);


				}
				if (m_pModel->Get_PlayRate() > 0.95 && iChecker == 1 && m_pModel->Get_NowAnimIndex() == 18)
				{
					m_pModel->Change_AnimIndex(0, 0.15, true);
					iChecker++;
				}

				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(128, 27.39f, 226.782f));

				if (iChecker == 2 && m_SpwanPassedTime > 13.8)
				{
					iChecker++;
					m_pModel->Change_AnimIndex_UntilNReturn_Must(20, 22, 14, 0.08f, true);
				}
			}
			else
			{
				static _uint iSoundChecker = 0;
				if (iSoundChecker == 0 && m_pModel->Get_NowAnimIndex() == 20 && m_pModel->Get_PlayRate() > 0.45f)
				{
					iSoundChecker++;
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(1000.f, 10000.f);
						tSoundDesc.fTargetSound = 0.15f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_vox_laugh01_short.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
				}
				else 	if (iSoundChecker == 1 && m_pModel->Get_NowAnimIndex() == 21 && m_pModel->Get_PlayRate() > 0.f)
				{
					iSoundChecker++;
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(1000.f, 10000.f);
						tSoundDesc.fTargetSound = 0.15f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_vox_laugh02.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
				}
				else if (iSoundChecker == 2 && m_pModel->Get_NowAnimIndex() == 22 && m_pModel->Get_PlayRate() > 0.f)
				{
					iSoundChecker++;
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(1000.f, 10000.f);
						tSoundDesc.fTargetSound = 0.15f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_vox_laugh03.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
				}

				static _uint iChecker = 0;
				if (!iChecker&& m_pModel->Get_NowAnimIndex() == 14 && m_pModel->Get_PlayRate() > 0.1230769)
				{

					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 2.f, _float4(1.3f));
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(1000.f, 10000.f);
						tSoundDesc.fTargetSound = 0.35f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_vox_new_roar_onetwo.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
					}

					iChecker++;
				}
				if (iChecker == 1 && m_pModel->Get_NowAnimIndex() == 14 && m_pModel->Get_PlayRate() > 0.57692307692)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(1000.f, 10000.f);
						tSoundDesc.fTargetSound = 0.35f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_vox_new_roar_up.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
					iChecker++;
				}

				if (iChecker == 2 && m_pModel->Get_NowAnimIndex() == 14 && m_pModel->Get_PlayRate() > 0.95)
				{
					m_bSpwanAnimFinished = true;
					iChecker++;
				}
			}




		}


		m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS), 7.8f);
		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime * ((m_bIsBerseked) ? 1.5f : 1.f), m_bIsOnScreen));
		FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

		if (m_bIsOnScreen)
		{

			_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr.pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr.pDefaultPivotMat);

			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

			for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
				m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());



			g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

#ifdef _DEBUG
			FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG
			m_vecWeapon[0]->Update(fDeltaTime);
		}


	}

	return _int();
}

_int CExecutor::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)return -1;

	if (m_bSpwanAnimFinished)
	{
		FAILED_CHECK(Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));
	}

	if (m_bIsOnScreen)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
		m_vecWeapon[0]->LateUpdate(fDeltaTime);
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CExecutor::Render()
{
	if (__super::Render() < 0)		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();


	for (_uint i = 0; i < NumMaterial; i++)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0, i, "g_BoneMatrices"));
	}

	return _int();
}

_int CExecutor::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 10, i, "g_BoneMatrices"));

	}
	return _int();
}

void CExecutor::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	{
		switch (eConflictedObjCollisionType)
		{

		case Engine::CollisionType_PlayerWeapon:
		{

			if (!lstrcmp(pConflictedObj->Get_NameTag(), L"VopalBlade"))
			{

				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 35);
				tSoundDesc.fTargetSound = 1.f;
				wstring SoundTrack = L"";
				SoundTrack = L"Weapon_vorpal_imp_plant0" + to_wstring(rand() % 5 + 1) + L".ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);


			}
			else if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Horse"))
			{

				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex);
				tSoundDesc.vMinMax = _float2(0, 35);
				tSoundDesc.fTargetSound = 0.5f;
				wstring SoundTrack = L"";
				SoundTrack = L"Weapon_hobby_imp_sand0" + to_wstring(rand() % 5 + 1) + L".ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);

			}
		}
		break;
		case Engine::CollisionType_Terrain:
			break;

		default:
			break;
		}

	}
}

_int CExecutor::Update_DmgCalculate(_double fDeltaTime)
{
	if (m_DmgPassedTime <= 0)
	{
		if (m_fDmgAmount > 0)
		{
			ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
			m_fDmgAmount = 0;
			m_DmgPassedTime = 0;
		}
		return 0;
	}
	m_DmgPassedTime -= fDeltaTime;

	if (m_fHP <= 0)
	{
		if (!m_bDeathAnimStart)
		{
			m_DeathAnimPassedTime = 0; 
			m_pModel->Change_AnimIndex_ReturnTo_Must(19, 0, 0.15, true);
			m_bDeathAnimStart = true;
		}
		return 0;
	}

	if (!m_bIsBerseked && m_fHP < m_fMaxHP * 0.6f)
	{
		m_bIsBerseked = true;

		m_bIsPatternFinished = true;
		m_PatternDelayTime = 0;
		m_PatternPassedTime = 0;
		m_ePattern = 0;
		m_bIsDmgAnimUpdated[0] = true;

		m_pTransformCom->Set_MoveSpeed(5.5f);



		m_pModel->Change_AnimIndex_ReturnTo_Must(15, 0, 0.15, true);
	}
	else if (!m_bIsDmgAnimUpdated[0] && m_fMaxHP * 0.1 < m_fDmgAmount)
	{

		m_pModel->Change_AnimIndex_ReturnTo_Must(19, 0, 0.15, true);

		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_PatternDelayTime = 3;
		m_bIsDmgAnimUpdated[0] = true;
		//Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 10);
	}


	

	return 0;
}

_int CExecutor::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;




	if (m_bIsBerseked)
		//???? ????
	{
		if (m_bIsPatternFinished)
		{
			m_ePattern += 1;



			if (m_ePattern > 4) m_ePattern = 0;
			m_bIsPatternFinished = false;
			m_PatternPassedTime = 0;
			//m_pModel->Change_AnimIndex(2);
		}


		switch (m_ePattern)
		{
		case 0:

		{

			if (m_PatternDelayTime)
			{
				m_PatternDelayTime -= fDeltaTime;


				_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
				if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
					m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

				if (iAnimIndex == 1 || iAnimIndex == 2)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					if (iAnimIndex == 1)
						m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
					else
						m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));

					m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
				}


				if (m_PatternDelayTime < 0)
				{
					m_bIsPatternFinished = true;
					m_PatternDelayTime = 0;
					m_PatternPassedTime = 0;
				}

			}
			else
			{

				if (!m_bIsJumping)
				{
					m_pModel->Change_AnimIndex_UntilTo(4, 6, 0.15, true);
					m_bIsJumping = true;
				}
				else if (m_pModel->Get_NowAnimIndex() == 5 && !m_bIsAddForceActived)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
					Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.25f + XMVectorSet(0, 1, 0, 0) * 0.5f, 200);
					//Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.75f + XMVectorSet(0, 1, 0, 0) * 0.25f, 100);
					m_LevitationTime = 0.7f;
				}
			}

		}

		break;
		case 1:
		{
			if (!m_PatternPassedTime)
			{

				m_pModel->Change_AnimIndex_UntilNReturn_Must(8, 9, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;



					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}


					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 2.8f;
					}
				}
			}


		}
		break;
		case 2:

		{
			if (!m_PatternPassedTime)
			{

				m_pModel->Change_AnimIndex_UntilNReturn_Must(10, 11, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;

					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}

					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 2.8f;
					}
				}
			}


		}

		break;
		case 3:
		{
			if (!m_PatternPassedTime)
			{
				m_pModel->Change_AnimIndex_ReturnTo_Must(13, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;

					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}

					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 2.8f;
					}
				}
			}
		}
		break;
		case 4:
		{
			if (!m_PatternPassedTime)
			{
				m_pModel->Change_AnimIndex_ReturnTo_Must(14, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;

					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}

					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 2.8f;
					}
				}
			}
		}
		break;


		default:
			break;
		}



	}
	else
		//?Ϲ? ????
	{
		if (m_bIsPatternFinished)
		{
			m_ePattern += 1;
			//m_ePattern = 3;

			if (m_ePattern > 3) m_ePattern = 0;
			m_bIsPatternFinished = false;
			m_PatternPassedTime = 0;
			m_PatternDelayTime = 0;
		}


		switch (m_ePattern)
		{
		case 0:

		{

			if (m_PatternDelayTime)
			{
				m_PatternDelayTime -= fDeltaTime;


				_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
				if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
					m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

				if (iAnimIndex == 1 || iAnimIndex == 2)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					if (iAnimIndex == 1)
						m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
					else
						m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));

					m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
				}


				if (m_PatternDelayTime < 0)
				{
					m_bIsPatternFinished = true;
					m_PatternDelayTime = 0;
					m_PatternPassedTime = 0;
				}

			}
			else
			{

				if (!m_bIsJumping)
				{
					m_pModel->Change_AnimIndex_UntilTo(4, 6, 0.15, true);
					m_bIsJumping = true;
				}
				else if (m_pModel->Get_NowAnimIndex() == 5 && !m_bIsAddForceActived)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
					Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.25f + XMVectorSet(0, 1, 0, 0) * 0.5f, 200);
					m_LevitationTime = 0.7f;
					//Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.75f + XMVectorSet(0, 1, 0, 0) * 0.25f, 100);
				}
			}

		}

			break;
		case 1:
		{
			if (!m_PatternPassedTime)
			{

				m_pModel->Change_AnimIndex_UntilNReturn_Must(8, 8, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;



					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}


					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;					
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 4.2f;
					}
				}
			}


		}
			break;
		case 2:

		{
			if (!m_PatternPassedTime)
			{

				m_pModel->Change_AnimIndex_UntilNReturn_Must(10, 10, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;

					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}

					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 4.2f;
					}
				}
			}


		}

			break;
		case 3:
		{
			if (!m_PatternPassedTime)
			{
				m_pModel->Change_AnimIndex_ReturnTo_Must(12, 0, 0.15, true);
			}
			else
			{
				if (m_PatternDelayTime)
				{
					m_PatternDelayTime -= fDeltaTime;

					_uint iAnimIndex = m_pModel->Get_NowAnimIndex();
					if (!iAnimIndex && m_pModel->Get_PlayRate() >= 0.01)
						m_pModel->Change_AnimIndex((m_bIsBerseked) ? 1 : 2);

					if (iAnimIndex == 1 || iAnimIndex == 2)
					{
						m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
						if (iAnimIndex == 1)
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.1f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.9f));
						else
							m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.05f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.95f));
						m_pTransformCom->MovetoDir(m_vLookDir.XMVector(), fDeltaTime);
					}

					if (m_PatternDelayTime < 0)
					{
						m_bIsPatternFinished = true;
						m_PatternDelayTime = 0;
						m_PatternPassedTime = 0;
					}
				}
				else
				{
					if (m_pModel->Get_NowAnimIndex() == 0)
					{
						m_PatternDelayTime = 4.2f;
					}
				}
			}


		}
			break;


		default:
			break;
		}


	}


	


	return _int();
}

void CExecutor::Add_Dmg_to_Monster(_float iDmgAmount)
{
	m_DmgPassedTime = MonsterDmgTime;
	m_fDmgAmount += iDmgAmount;
	m_fHP -= iDmgAmount;

	if (m_fHP < m_fMaxHP * 0.5f) m_pTransformCom->Set_MoveSpeed(5.5f);
}

HRESULT CExecutor::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Executor), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));

	m_tCollisionAttachPtr = m_pModel->Find_AttachMatrix_InHirarchyNode("Bip01-Spine1");
	NULL_CHECK_RETURN(m_tCollisionAttachPtr.pDefaultPivotMat, E_FAIL);



	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));


	//Pivot  : -0.160000f , 0.000000f , -4.440007f , 1
	//size  : 7.600080f , 1.000000f , 1.000000f  
	ColliderDesc.vScale = _float3(7.600080f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-0.160000f, 0.000000f, -4.440007f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));



	//Pivot  : 0.000000f , 0.310000f , -2.019999f , 1
	//size  : 2.039999f , 0.450001f , 1.180000f  
	//(x,z,y)
	//size  : 4.910018f , 1.699999f , 3.499998f  

	ColliderDesc.vScale = _float3(3.499998f, 2.699999f, 4.910018f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-0.160000f, 0.000000f, -4.440007f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();



	CTransform::TRANSFORMDESC tDesc = {};
	tDesc.fMovePerSec = 2.8f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));


	return S_OK;
}

HRESULT CExecutor::SetUp_Weapon()
{

	CGameInstance* pInstance = GetSingle(CGameInstance);

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);

	CMonsterWeapon::MONWEAPONDESC tWeaponDesc;
	tWeaponDesc.pModel = m_pModel;
	tWeaponDesc.pParantTransform = m_pTransformCom;
	tWeaponDesc.szHirarchyNodeName = "Bip01-Prop1";

	CMonsterWeapon* pWeapon = nullptr;
	pInstance->Add_GameObject_Out_of_Manager((CGameObject**)&pWeapon, m_eNowSceneNum, TAG_OP(Prototype_Scythe), &tWeaponDesc);
	NULL_CHECK_RETURN(pWeapon, E_FAIL);

	m_vecWeapon.push_back(pWeapon);


	return S_OK;
}

HRESULT CExecutor::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	m_LevitationTime += fDeltaTime;
	_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);

	pTransform->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_uint eTileKinds = Tile_End;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, pTransform->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),nullptr,&eTileKinds);

	if (bIsOn)
	{
		m_LevitationTime = 0;
		pTransform->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		if (m_bIsJumping && m_pModel->Get_NowAnimIndex() != 4 && m_pModel->Get_NowAnimIndex() != 5)
		{
			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(0.3f));

			CDustWind::DUSTWINDDESC tDesc;

			tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
			tDesc.ToTalLifeTime = 1.f;
			tDesc.StartScale = _float3(2, 5.f, 2);
			tDesc.TargetScale = _float3(7, 0.5f, 7);
			tDesc.eEasingType = TYPE_ExpoOut;
			
			g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));

			m_bIsJumping = false;
			m_PatternDelayTime = 3;
			m_pModel->Change_AnimIndex_ReturnTo_Must(7, 0, 0, true);

			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(5, 50);
				tSoundDesc.fTargetSound = 0.75f;
				wstring SoundTrack = L"";
				SoundTrack = L"Executioner_Stomp_01.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);


				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tSoundDesc.vPosition, 6);
				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tSoundDesc.vPosition, 7);
				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tSoundDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 6);
				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tSoundDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 7);
			}



			GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.3, _float4(0.5));

			CGameInstance* pInstance = g_pGameInstance;
			CBreakedGazebo::BREAKEDGAZBODESC tGizeboDesc;
			tGizeboDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 5;
			for (_uint i = 0; i < 20; i++)
			{
				tGizeboDesc.MeshKinds = rand() % 2 + Prototype_QBattleTowerParticleA;
				pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_BreakedGazbo), &tGizeboDesc);
			}

		}
	}

	return S_OK;
}

HRESULT CExecutor::Adjust_AnimMovedTransform(_double fDeltatime)
{
	if (!m_bSpwanAnimFinished) return S_FALSE;

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;

	//static float Power = 7.2f;
	//if (g_pGameInstance->Get_DIKeyState(DIK_UP)& DIS_Down)
	//{
	//	Power += 0.2f;
	//	string ttszLog = "//Power  : " + to_string(Power) + "\n";

	//	wstring ttDebugLog;
	//	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//	OutputDebugStringW(ttDebugLog.c_str());

	//}
	//else if (g_pGameInstance->Get_DIKeyState(DIK_DOWN)& DIS_Down)
	//{
	//	Power -= 0.2f;
	//	string ttszLog = "//Power  : " + to_string(Power) + "\n";

	//	wstring ttDebugLog;
	//	ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//	OutputDebugStringW(ttDebugLog.c_str());
	//}


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 1:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.4)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate > 0.93)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}

			break;
		case 2:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.34)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
				
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.75f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate > 0.86)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
				
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.75f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}

			break;


		case 8:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.35714)
			{
				Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 30);
				if (m_bIsBerseked)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(5, 50);
						tSoundDesc.fTargetSound = 0.2f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_swinglight0" + to_wstring(rand() % 3 + 1) + L".ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
				}
				else
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(5, 35);
						tSoundDesc.fTargetSound = 0.25f;
						wstring SoundTrack = L"";
						SoundTrack = L"Executioner_swing0" + to_wstring(rand() % 3 + 1) + L".ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
				}

				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate > 0.785714)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 5,0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}



			break;

		case 9:
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.088461538461)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.1f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_swinglight0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate >= 0.15)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.1f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_swinglight0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate >= 0.296875)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 8.8f, 0.5f / ((m_bIsBerseked) ? 1.5f : 1.f));


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 3 && PlayRate >= 0.356589147286)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 4 && PlayRate >= 0.40625)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.75f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_Stomp_01.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.3, _float4(0.5));

				CGameInstance* pInstance =  g_pGameInstance;
				CBreakedGazebo::BREAKEDGAZBODESC tDesc;
				tDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 5;
				for (_uint i = 0 ; i< 20; i++)
				{
					tDesc.MeshKinds = rand() % 2 + Prototype_QBattleTowerParticleA;
					pInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_BreakedGazbo), &tDesc);
				}


				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 6);
				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector() + XMVectorSet(0, 2.5f, 0, 0), 7);
				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector(), 6);
				GetSingle(CUtilityMgr)->Start_InstanceParticle(m_eNowSceneNum, tDesc.vPosition.XMVector(), 7);

				m_iAdjMovedIndex++;
			}
			break;

		case 10:
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.001)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 7.2f, 1.f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate >= 0.3378378378378)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 35);
					tSoundDesc.fTargetSound = 0.25f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_swing0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate >= 0.58666)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 3, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 3 && PlayRate >= 0.813333333)
			{

				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 4 && PlayRate >= 0.9333333333333)
			{

				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}


			break;
		case 11:
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.01)
			{

				CEntireCard::ENTIRECARDDESC tDesc;
				tDesc.vMonsterPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
				tDesc.vStartPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 3;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_EntireCard), &tDesc);

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate >= 0.44444)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate >= 0.505494505494)
			{
				{
					GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(0.1f));
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 50);
					tSoundDesc.fTargetSound = 0.5f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_step0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}

			else if (m_iAdjMovedIndex == 3 && PlayRate >= 0.527472527472527)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 35);
					tSoundDesc.fTargetSound = 0.25f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_swing0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}


			break;

		case 12://shout
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.2777)
			{

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.7f, _float4(0.3f));

				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 1.5f;
				tDesc.StartScale = _float3(2, 10, 2);
				tDesc.TargetScale = _float3(15, 0.5f, 15);
				tDesc.eEasingType = TYPE_ExpoIn;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_onetwo.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}

				//???? ???? ??ȯ
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}


			break;

		case 13://shoutNEQ
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.1328571428)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_onetwo.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate >= 0.1578947)
			{

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.7f, _float4(0.3f));

				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 1.5f;
				tDesc.StartScale = _float3(2, 10, 2);
				tDesc.TargetScale = _float3(15, 0.5f, 15);
				tDesc.eEasingType = TYPE_ExpoIn;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				tDesc.vColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
				tDesc.vColor.w = 1; tDesc.vColor.y *= 0.25f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				tDesc.vColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
				tDesc.vColor.w = 1; tDesc.vColor.y *= 0.25f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));


				
				//???? ???? ??ȯ
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 2 && PlayRate >= 0.4962406)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.1f, _float4(1.f));

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 35);
					tSoundDesc.fTargetSound = 0.05f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_spin_destrcut0" + to_wstring(rand() % 12 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 3 && PlayRate >= 0.503759)
			{

				//Eq ??ȯ
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				CExecutorThron::THRONDESC tDesc;
				tDesc.MoveDir = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_LOOK);

				tDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + tDesc.MoveDir.XMVector() * 3;
				tDesc.MeshKinds = 0;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_Thron), &tDesc);

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5, 35);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_spotalice02.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}


			break;

		case 14://shoutNSummon
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.103076)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_onetwo.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}

				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate >= 0.123076)
			{



				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.7f, _float4(0.3f));

				CDustWind::DUSTWINDDESC tDesc;

				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 1.5f;
				tDesc.StartScale = _float3(2, 10, 2);
				tDesc.TargetScale = _float3(4, 0.5f, 4);
				tDesc.eEasingType = TYPE_ExpoIn;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				tDesc.vColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
				tDesc.vColor.y *= 0.25f;
				tDesc.vColor.w = 1;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				tDesc.vColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
				tDesc.vColor.y *= 0.25f;
				tDesc.vColor.w = 1;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));



				CDustWindCurvedMove::DUSTWINDDESC CurvedDesc;

				CurvedDesc.StartScale = _float3(1.5f, 0.5f, 1.5f);
				CurvedDesc.ToTalLifeTime = 2.8f;
				CurvedDesc.vColor = _float4(0, 0, 0, 1);
				CurvedDesc.vTargetPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

				CurvedDesc.vPosition = _float3(108.9478f, 27.39f, 222.205826f);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_DustCurved", &(CurvedDesc));

				CurvedDesc.vPosition = _float3(128.586273f, 27.39f, 231.820633f);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_DustCurved", &(CurvedDesc));

				CurvedDesc.vPosition = _float3(145.696915f, 27.39f, 221.406021f);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_DustCurved", &(CurvedDesc));

				CurvedDesc.vPosition = _float3(145.431885f, 27.39f, 200.339203f);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_DustCurved", &(CurvedDesc));

				CurvedDesc.vPosition = _float3(126.985931f, 27.39f, 188.308807f);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_DustCurved", &(CurvedDesc));

				CurvedDesc.vPosition = _float3(108.9478f, 27.39f, 202.73001f);
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"Prototype_DustCurved", &(CurvedDesc));


				//???? ???? ??ȯ
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 2 && PlayRate >= 0.538461538461538)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_up.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 3 && PlayRate >= 0.72180)
			{
			
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 1.3f, _float4(0.3f));

				//?????? ??ȯ
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}


			break;

		case 15://????ũ
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.f)
			{

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 3.5f, _float4(1.f));

				CDustWind::DUSTWINDDESC tDesc;
				tDesc.vPosition = (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				tDesc.ToTalLifeTime = 3.5f;
				tDesc.StartScale = _float3(2, 10, 2);
				tDesc.TargetScale = _float3(15, 0.5f, 15);
				tDesc.eEasingType = TYPE_ExpoIn;

				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				tDesc.vColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
				tDesc.vColor.w = 1; tDesc.vColor.y *= 0.25f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				tDesc.vColor = GetSingle(CUtilityMgr)->RandomFloat3(0, 1);
				tDesc.vColor.w = 1; tDesc.vColor.y *= 0.25f;
				g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), TAG_OP(Prototype_DustWind), &(tDesc));
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_up.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}

			
				m_iAdjMovedIndex++;
			}

			if (m_iAdjMovedIndex == 1 && PlayRate >= 0.3289473684210)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.35f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_new_roar_onetwo.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}

				
				m_iAdjMovedIndex++;
			}

			break;


			
			
		case 19://Knockback
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.0)
			{

				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * -1 , 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(5.f, 60);
					tSoundDesc.fTargetSound = 0.15f;
					wstring SoundTrack = L"";
					SoundTrack = L"Executioner_vox_att01_" + to_wstring(rand() % 3) + L".ogg";
					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc));
				}

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.0759493670886)
			{

				m_iAdjMovedIndex++;
			}

			break;
		default:
			break;
		}
	}




	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

CExecutor * CExecutor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CExecutor*	pInstance = new CExecutor(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CExecutor::Clone(void * pArg)
{
	CExecutor*	pInstance = new CExecutor(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CExecutor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CExecutor::Free()
{
	__super::Free();


	for (auto& pWeapon : m_vecWeapon)
		Safe_Release(pWeapon);
	

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);

}
