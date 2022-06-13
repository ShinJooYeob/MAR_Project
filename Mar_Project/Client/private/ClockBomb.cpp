#include "stdafx.h"
#include "..\public\ClockBomb.h"
#include "Terrain.h"
#include "GamePlayUI.h"
#include "Monster.h"
#include "BreakableObj.h"
#include "CircleTornado.h"


#define CBSoundMin 5
#define CBSoundMax 30
#define CBTargetSound 0.35f

CClockBomb::CClockBomb(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CClockBomb::CClockBomb(const CClockBomb & rhs)
	: CWeapon(rhs)
{
}

CClockBomb::CClockBomb(const CClockBomb & rhs, _uint Clone2CloneChecker)
	: CWeapon(rhs,1),
	m_bHavetoMaking(true),
	m_tParticleDesc(rhs.m_tParticleDesc)
{
}

HRESULT CClockBomb::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CClockBomb::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	m_bHavetoMaking = true;

	FAILED_CHECK(Ready_ParticleDesc());
	return S_OK;
}

HRESULT CClockBomb::Initialize_Clone2Clone(void * pArg)
{

	FAILED_CHECK(SetUp_Components());
	m_SpwanPassedTime = 0;
	m_bHavetoMaking = true;

	m_szNameTag = TAG_LAY(Layer_ClockBomb);
	return S_OK;
}

_int CClockBomb::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);

	if (m_bDeadBomb)
	{
		m_DeadPassedTime += fDeltaTime;
		_float Scale = g_pGameInstance->Easing(TYPE_Linear, 1, 5, (_float)m_DeadPassedTime, 0.2f);

		m_pTransformCom->Scaled_All(_float3(Scale));

		if (m_DeadPassedTime > 0.2f)
		{
			Set_IsDead();
			return 0;
		}


		m_BoneMatrix = XMMatrixInverse(nullptr,XMMatrixTranslation(-1.065f, 0.306f, -1.229f))  *  m_pTransformCom->Get_WorldMatrix() * XMMatrixTranslation(0, 1.f, 0);
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_BoneMatrix.XMatrix());
	}
	else
	{

		m_SpwanPassedTime += fDeltaTime;
		if (!m_bIsOn && m_SpwanPassedTime < 1.3)
		{
			_Matrix Scale = m_pTransformCom->Get_MatrixScale_All();

			_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);
			TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
			TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
			TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


			m_pTransformCom->Set_Matrix(TransformMatrix* m_tWeaponDesc.pParantTransform->Get_WorldMatrix());

			m_bIsOn = false;
			m_LevitationTime = 0.3;
			CGameInstance* pInstance = GetSingle(CGameInstance);

			//m_LevitationTime += fDeltaTime;
			//_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);

			//m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


			CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

			_uint eNowTile = Tile_End;
			_float3 CaculatedPos = pTerrain->PutOnTerrain(&m_bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 0.1f, 0, 0), m_vOldPos.XMVector(), nullptr, &eNowTile);

			if (eNowTile == Tile_None)
			{
				m_bIsOn = true;
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
					tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
					tSoundDesc.fTargetSound = CBTargetSound * 2.f;
					wstring SoundTrack = L"";

					//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
					SoundTrack = L"Weapon_cb_land.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
				}


			}
			else if (m_bIsOn)
			{
				m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
					tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
					tSoundDesc.fTargetSound = CBTargetSound * 2.f;
					wstring SoundTrack = L"";

					//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
					SoundTrack = L"Weapon_cb_land.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
				}

			}


		}
		else
		{
			if (!m_bIsOn)
			{

				CGameInstance* pInstance = GetSingle(CGameInstance);

				m_LevitationTime += fDeltaTime;
				_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
				m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


				CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

				_uint eNowTile = Tile_End;
				_float3 CaculatedPos = pTerrain->PutOnTerrain(&m_bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS) + XMVectorSet(0, 0.1f, 0, 0), m_vOldPos.XMVector(), nullptr, &eNowTile);

				if (eNowTile == Tile_None)
				{
					m_bIsOn = true;
					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
						tSoundDesc.fTargetSound = CBTargetSound * 2.f;
						wstring SoundTrack = L"";

						//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						SoundTrack = L"Weapon_cb_land.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
					}

				}
				else if (m_bIsOn)
				{
					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos.XMVector() - XMVectorSet(0, 0.1f, 0, 0));

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
						tSoundDesc.fTargetSound = CBTargetSound * 2.f;
						wstring SoundTrack = L"";

						//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						SoundTrack = L"Weapon_cb_land.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
					}

				}

			}
			else
			{
				if (m_pModel->Get_NowAnimIndex() == 4)
					m_pModel->Change_AnimIndex_ReturnTo_Must(5, 0, 0.15, true);


				if (m_ClockPassedTime > 0)
				{
					m_TargetTimer -= (_float)fDeltaTime;



					m_ClockPassedTime -= fDeltaTime;


					if (m_ClockPassedTime < 2.5)
						m_pModel->Change_AnimIndex(1, 0.15, true);

					if (m_TargetTimer < 0)
					{
						{
							SOUNDDESC tSoundDesc;

							tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
							tSoundDesc.vMinMax = _float2(1000000000.f, 10000000001.f);
							tSoundDesc.fTargetSound = 1.f;
							wstring SoundTrack = L"";

							//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
							SoundTrack = L"Weapon_cb_watch.ogg";

							g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
							m_TargetTimer = ((_float)m_ClockPassedTime) / 5.f + 0.1f;
						}
					}

					switch (m_iSoundChecker)
					{
					case 0:
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
						tSoundDesc.fTargetSound = CBTargetSound;
						wstring SoundTrack = L"";

						//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						SoundTrack = L"Weapon_cb_tick_slow.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
						m_iSoundChecker++;
					}

						break;

					case 1:
						if (m_ClockPassedTime < 4.0)
						{
							{
								SOUNDDESC tSoundDesc;

								tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
								tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
								tSoundDesc.fTargetSound = CBTargetSound;
								wstring SoundTrack = L"";

								//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
								SoundTrack = L"Weapon_cb_tick_slow.ogg";

								g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
								m_iSoundChecker++;
							}

						}
						break;

					case 2:
						if (m_ClockPassedTime < 3.0)
						{
							
							{
								SOUNDDESC tSoundDesc;

								tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
								tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.75f);
								tSoundDesc.fTargetSound = CBTargetSound;
								wstring SoundTrack = L"";

								//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
								SoundTrack = L"Weapon_cb_tick_warn.ogg";

								g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
							}

							{
								SOUNDDESC tSoundDesc;

								tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
								tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
								tSoundDesc.fTargetSound = CBTargetSound;
								wstring SoundTrack = L"";

								//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
								SoundTrack = L"Weapon_cb_tick_slow.ogg";

								g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
								m_iSoundChecker++;
							}

						}
						break;

					case 3:
						if (m_ClockPassedTime < 2.0)
						{
							{
								SOUNDDESC tSoundDesc;

								tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
								tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
								tSoundDesc.fTargetSound = CBTargetSound;
								wstring SoundTrack = L"";

								//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
								SoundTrack = L"Weapon_cb_tick_fast.ogg";

								g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
								m_iSoundChecker++;
							}

						}
						break;

					case 4:
						if (m_ClockPassedTime < 1.0)
						{
							{
								SOUNDDESC tSoundDesc;

								tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
								tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
								tSoundDesc.fTargetSound = CBTargetSound;
								wstring SoundTrack = L"";

								//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
								SoundTrack = L"Weapon_cb_tick_fast.ogg";

								g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
								m_iSoundChecker++;
							}

						}
						break;


					default:
						break;
					}

















					if (m_ClockPassedTime < 0)
					{
						CUtilityMgr* pUtil = GetSingle(CUtilityMgr);


						m_tParticleDesc.FixedTarget = m_pColliderCom->Get_ColliderPosition();
						pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


						pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
						pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



						CCircleTornado::CIRCLETORNADODESC tDesc;
						tDesc.vLook = _float3(0.00000001f, 1.f, 0);
						tDesc.vPosition = m_tParticleDesc.FixedTarget;
						tDesc.fSize = 1.5f;
						g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
						tDesc.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
						tDesc.fSize = 1;
						g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);

						m_bDeadBomb = true;

						{
							SOUNDDESC tSoundDesc;

							tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
							tSoundDesc.vMinMax = _float2(CBSoundMin, CBSoundMax);
							tSoundDesc.fTargetSound = CBTargetSound;
							wstring SoundTrack = L"";

							SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
							//SoundTrack = L"london_l5_wooddoor_wind.ogg";

							g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
						}

						m_DeadPassedTime = 0;
					}


				}

			}


		}






		m_BoneMatrix = m_pTransformCom->Get_WorldMatrix();
		for (_uint i = 0; i < m_pColliderCom->Get_NumColliderBuffer(); i++)
			m_pColliderCom->Update_Transform(i, m_BoneMatrix.XMatrix());

		FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));
		FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));



	}


	g_pGameInstance->Add_CollisionGroup(CollisionType_PlayerWeapon, this, m_pColliderCom);
#ifdef _DEBUG
	FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG


	return _int();
}

_int CClockBomb::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	if (!m_bDeadBomb)
	{
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	}

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CClockBomb::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);





	_float4x4 ShaderMat = m_BoneMatrix.TransposeXMatrix();
	m_pShaderCom->Set_RawValue("g_AttechMatrix", &ShaderMat, sizeof(_float4x4));



	CGameInstance* pInstance = GetSingle(CGameInstance);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamPosition", &pInstance->Get_TargetPostion_float4(PLV_CAMERA), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_CamLookDir", &pInstance->Get_TargetPostion_float4(PLV_CAMLOOK), sizeof(_float4)));


	//const LIGHTDESC* pLightDesc = pInstance->Get_LightDesc(LIGHTDESC::TYPE_DIRECTIONAL, 0);
	//NULL_CHECK_RETURN(pLightDesc, -1);

	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightVector", &(pLightDesc->vVector), sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4)));
	//FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4)));
	//FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));



	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader(m_pShaderCom, "g_MskingTextrue", 0));
	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		if (m_bHavetoMaking && i > 0) continue;

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		
		if (m_bHavetoMaking)
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 7, i, "g_BoneMatrices"));
		}
		else 
		{
			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 6, i, "g_BoneMatrices"));
		}
	}


	return _int();
}

_int CClockBomb::LightRender()
{
	if (__super::LightRender() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);

	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 11, i, "g_BoneMatrices"));

	}
	return _int();
}

void CClockBomb::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider* pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	if (m_SpwanPassedTime > 1.3)
	{
		if (!m_bDeadBomb)
		{

			switch (eConflictedObjCollisionType)
			{

			case Engine::CollisionType_Monster:
			{
				if (lstrcmp(pConflictedObj->Get_NameTag(), TAG_LAY(Layer_Breakable)))
				{
					CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

					pConflictedCollider->Set_Conflicted();
					pUtil->SlowMotionStart();
					((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(20);


					m_tParticleDesc.FixedTarget = m_pColliderCom->Get_ColliderPosition();
					pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



					CCircleTornado::CIRCLETORNADODESC tDesc;
					tDesc.vLook = _float3(0.00000001f, 1.f, 0);

					tDesc.vPosition = m_tParticleDesc.FixedTarget;
					tDesc.fSize = 1.5f;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
					tDesc.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
					tDesc.fSize = 1;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
					m_bDeadBomb = true;
					m_DeadPassedTime = 0;

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin, CBSoundMax);
						tSoundDesc.fTargetSound = CBTargetSound;
						wstring SoundTrack = L"";

						SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						//SoundTrack = L"london_l5_wooddoor_wind.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
					}


				}
				else
				{

					CGamePlayUI* pGameObj = (CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay)));
					NULL_CHECK_BREAK(pGameObj);
					pGameObj->Set_DrawClockBombUI(false);

					CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

					pConflictedCollider->Set_Conflicted();
					pUtil->SlowMotionStart();
					((CBreakableObj*)(pConflictedObj))->Add_Dmg_To_BreakableObj(20);


					m_tParticleDesc.FixedTarget = m_pColliderCom->Get_ColliderPosition();
					pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



					CCircleTornado::CIRCLETORNADODESC tDesc;
					tDesc.vLook = _float3(0.00000001f, 1.f, 0);
					tDesc.vPosition = m_tParticleDesc.FixedTarget;
					tDesc.fSize = 1.5f;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
					tDesc.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
					tDesc.fSize = 1;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
					m_bDeadBomb = true;
					m_DeadPassedTime = 0;

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin, CBSoundMax);
						tSoundDesc.fTargetSound = CBTargetSound;
						wstring SoundTrack = L"";

						SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						//SoundTrack = L"london_l5_wooddoor_wind.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
					}


				}

			}
			break;
			case Engine::CollisionType_Terrain:
				break;

			default:
				break;
			}

		}
		else if(m_DeadPassedTime > 0.15f)
		{
			switch (eConflictedObjCollisionType)
			{

			case Engine::CollisionType_Monster:
			{
				if (lstrcmp(pConflictedObj->Get_NameTag(), TAG_LAY(Layer_Breakable)))
				{

					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex));

					CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

					pConflictedCollider->Set_Conflicted();
					pUtil->SlowMotionStart();
					((CMonster*)(pConflictedObj))->Add_Dmg_to_Monster(20);


					m_tParticleDesc.FixedTarget = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex);
					pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



					CCircleTornado::CIRCLETORNADODESC tDesc;
					tDesc.vLook = _float3(0.00000001f, 1.f, 0);

					tDesc.vPosition = m_tParticleDesc.FixedTarget;
					tDesc.fSize = 1.5f;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
					tDesc.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
					tDesc.fSize = 1;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);

					m_bDeadBomb = true;
					m_DeadPassedTime = 0;

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin, CBSoundMax);
						tSoundDesc.fTargetSound = CBTargetSound;
						wstring SoundTrack = L"";

						SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						//SoundTrack = L"london_l5_wooddoor_wind.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
					}


				}
				else
				{

					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex));

					CGamePlayUI* pGameObj = (CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay)));
					NULL_CHECK_BREAK(pGameObj);
					pGameObj->Set_DrawClockBombUI(false);

					CUtilityMgr* pUtil = GetSingle(CUtilityMgr);

					pConflictedCollider->Set_Conflicted();
					pUtil->SlowMotionStart();
					((CBreakableObj*)(pConflictedObj))->Add_Dmg_To_BreakableObj(20);


					m_tParticleDesc.FixedTarget = pConflictedCollider->Get_ColliderPosition(iConflictedObjColliderIndex);
					pUtil->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);


					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);
					pUtil->Start_InstanceParticle(m_eNowSceneNum, m_tParticleDesc.FixedTarget, 0);



					CCircleTornado::CIRCLETORNADODESC tDesc;
					tDesc.vLook = _float3(0.00000001f, 1.f, 0);
					tDesc.vPosition = m_tParticleDesc.FixedTarget;
					tDesc.fSize = 1.5f;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);
					tDesc.vPosition = m_tParticleDesc.FixedTarget.XMVector() + XMVectorSet(0, 0.15f, 0, 0);
					tDesc.fSize = 1;
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_Particle), TAG_OP(Prototype_PlayerCircleTornado), &tDesc);

					m_bDeadBomb = true;
					m_DeadPassedTime = 0;

					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
						tSoundDesc.vMinMax = _float2(CBSoundMin, CBSoundMax);
						tSoundDesc.fTargetSound = CBTargetSound;
						wstring SoundTrack = L"";

						SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
						//SoundTrack = L"london_l5_wooddoor_wind.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
					}

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
}



HRESULT CClockBomb::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_ClockBomb), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	m_pModel->Change_AnimIndex(4, 0, true);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_MskTex), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));
	m_pTextureCom->Change_TextureLayer(L"ClockBomb");

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(-1.065f, 0.306f, -1.229f,1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

HRESULT CClockBomb::Ready_ParticleDesc()
{

	/////////5///////////////////////////////////////////////////////////////
	m_tParticleDesc = PARTICLEDESC();

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
	m_tParticleDesc.ParticleSize = _float3(0.15f, 0.15f, 0.15f);
	m_tParticleDesc.ParticleSize2 = _float3(5.f, 5.f, 5.f);

	m_tParticleDesc.ColorChageFrequency = 1;
	m_tParticleDesc.TargetColor = _float4(1.f, 0.643f, 0.141f, 1.f);
	m_tParticleDesc.TargetColor2 = _float4(1.f, 1.f, 1.f, 0.f);


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

HRESULT CClockBomb::Adjust_AnimMovedTransform(_double fDeltatime)
{
	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();

	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;



	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 5:
			if (m_bHavetoMaking && PlayRate > 0.9)
			{
				m_ClockPassedTime = 5;

				CGamePlayUI* pGameObj = (CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay)));
				NULL_CHECK_RETURN(pGameObj, E_FAIL);
				pGameObj->Set_DrawClockBombUI(true);
				
				m_bHavetoMaking = false;

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
					tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
					tSoundDesc.fTargetSound = CBTargetSound * 1.5f;
					wstring SoundTrack = L"";

					//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
					SoundTrack = L"Weapon_cb_grow.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
				}

			}
			
			break;
		}
	}
	else
	{
		switch (iNowAnimIndex)
		{
		case 5:
			if (m_bHavetoMaking)
			{
				m_ClockPassedTime = 5;

				CGamePlayUI* pGameObj = (CGamePlayUI*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_UI_GamePlay)));
				NULL_CHECK_RETURN(pGameObj, E_FAIL);
				pGameObj->Set_DrawClockBombUI(true);

				m_bHavetoMaking = false;


				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pColliderCom->Get_ColliderPosition();
					tSoundDesc.vMinMax = _float2(CBSoundMin* 0.5f, CBSoundMax * 0.5f);
					tSoundDesc.fTargetSound = CBTargetSound * 1.5f;
					wstring SoundTrack = L"";

					//SoundTrack = L"Weapon_cb_explode0" + to_wstring(rand() % 4 + 1) + L".ogg";
					SoundTrack = L"Weapon_cb_grow.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_EFFECT, &tSoundDesc);
				}
			}
			
			break;
		}

	}

	return S_OK;
}

CClockBomb * CClockBomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CClockBomb*	pInstance = new CClockBomb(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CClockBomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CClockBomb::Clone(void * pArg)
{
	CClockBomb*	pInstance = new CClockBomb(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CClockBomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CClockBomb::Clone2Clone(void * pArg)
{
	CClockBomb*	pInstance = new CClockBomb(*this,0);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created C2CClockBomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CClockBomb::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTextureCom);
	
}
