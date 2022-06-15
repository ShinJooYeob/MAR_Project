#include "stdafx.h"
#include "..\public\Eyepot.h"
#include "Terrain.h"
#include "EyepotChainGranade.h"

#define FootSetpSoud 0.35f
#define FootSetpSoudMin 0.f
#define FootSetpSoudMax 25.f

CEyepot::CEyepot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CEyepot::CEyepot(const CEyepot & rhs)
	: CMonster(rhs)
{
}

HRESULT CEyepot::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CEyepot::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));

	m_pModel->Change_AnimIndex(25);
	m_bStartPos = false;
	ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
	m_fHP = m_fMaxHP = 56;

	m_iPatternCount = 0;
	m_bIsJumping = false;
	m_bIsFarPattern = false;


	FAILED_CHECK(Ready_ParticleDesc());

	return S_OK;
}

_int CEyepot::Update(_double fDeltaTime)
{

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	if (m_bStartPos)
		Update_DmgCalculate(fDeltaTime);


	if (g_pGameInstance->Get_DIKeyState(DIK_5)&DIS_Down)
		Add_Dmg_to_Monster(2);

	if (g_pGameInstance->Get_DIKeyState(DIK_O)&DIS_Down)
		Add_Dmg_to_Monster(1000);


	_uint AnimIndex = m_pModel->Get_NowAnimIndex();

	static _double iSteamSoudTimer = 0;
	iSteamSoudTimer -= fDeltaTime;
	if (iSteamSoudTimer < 0)
	{

		iSteamSoudTimer = 0.1f;
		if(m_pModel->Get_NowAnimIndex() == 9 || m_pModel->Get_NowAnimIndex() == 10)
		{

			SOUNDDESC tSoundDesc;

			tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			tSoundDesc.vMinMax = _float2(0, 25);
			tSoundDesc.fTargetSound = 0.35f;

			wstring SoundTrack = L"";

			SoundTrack = L"Eyepot_tellb0" + to_wstring(rand() % 2 + 1) + L".ogg";

			//SoundTrack = L"MapObject_shrinkflower_open.ogg";

			g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);

			if (!m_pSteamSoundDesc || m_pSteamSoundDesc->iIdentificationNumber != 30)
			{
				{

					SOUNDDESC tSoundDesc;
					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.vMinMax = _float2(0, 25);
					tSoundDesc.fTargetSound = 0.1f;
					tSoundDesc.iIdentificationNumber = 30;


					wstring SoundTrack = L"";
					SoundTrack = L"c1_steamloop0" + to_wstring(rand() % 4 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc, &m_pSteamSoundDesc);
				}
			}

		}
	}


	if (m_bDeathAnimStart)
	{
		if (m_bDeathDissolveStart)
		{
			m_fDissolveTime += (_float)fDeltaTime;
			if (m_fDissolveTime > 5.f)
				Set_IsDead();
		}
		else
		{

			if (m_pModel->Get_PlayRate() < 0.95)
			{

				m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
				FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));

				FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));
			}
			else
			{
				m_bDeathDissolveStart = true;
				m_fDissolveTime = 0;
				return 0;
			}
		}
	}
	else 
	{
		if (!m_bIsDmgAnimUpdated[2])
		{

			if (m_bStartPos)
			{
				if (m_PatternDelayTime > 4 && (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 15))
				{
					Update_Pattern(fDeltaTime);
				}
				else
				{
					m_PatternDelayTime += fDeltaTime;

					if (!m_pModel->Get_IsUntillPlay() && !m_pModel->Get_IsHavetoBlockAnimChange())
					{
						m_pModel->Change_AnimIndex(1);
						FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.05f));
					}
				}
			}
			else//스타트 패턴
			{
				if (m_bStartSprout)
				{
					if (!m_bIsPatternFinished)
					{
						m_pModel->Change_AnimIndex_ReturnTo_Must(24, 0, 0.15, true);
						m_bIsPatternFinished = true;
						m_PatternPassedTime = 0;
						m_PatternDelayTime = 0;

					}
					else
					{
						m_PatternPassedTime += fDeltaTime;

						if (m_PatternPassedTime > 5.)
						{
							m_PatternPassedTime = 0;
							m_bStartPos = true;
						}

					}

				}
			}


		}


		m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));
		_uint NowAnimIndex = m_pModel->Get_NowAnimIndex();

		if ((NowAnimIndex >= 8 && NowAnimIndex <= 12)|| NowAnimIndex == 7)
		{
			FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime * 1.8f, m_bIsOnScreen));

		}
		else
		{
			FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
		}

		FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

		if (m_bIsOnScreen)
		{

			for (_uint i = 0; i < 3; i++)
				m_pColliderCom->Update_Transform(i, m_pTransformCom->Get_WorldMatrix());

			for (_uint i = 0; i < 2; i++)
			{
				_Matrix			TransformMatrix = XMLoadFloat4x4(m_ArrCollisionAttach[i].pUpdatedNodeMat) * XMLoadFloat4x4(m_ArrCollisionAttach[i].pDefaultPivotMat);

				TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
				TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
				TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);


				TransformMatrix = TransformMatrix* m_pTransformCom->Get_WorldMatrix();
				m_pColliderCom->Update_Transform(i + 3, TransformMatrix);

			}

		}
		if (!m_bIsPatternFinished && (m_ePattern == 0 || m_ePattern == 2))
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_MonsterWeapon, this, m_pColliderCom);

		}
		else
		{
			g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

		}
#ifdef _DEBUG
		FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

		m_pSubTransformCom->Set_MatrixState(CTransform::STATE_POS, m_pColliderCom->Get_ColliderPosition(4));
	}
	return _int();
}

_int CEyepot::LateUpdate(_double fDeltaTime)
{


	FAILED_CHECK(Set_Monster_On_Terrain(m_pTransformCom, fDeltaTime));


	if (m_bIsOnScreen)
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));


	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CEyepot::Render()
{
	if (__super::Render() < 0)
		return -1;

	if (m_bDeathDissolveStart)
	{
		NULL_CHECK_RETURN(m_pModel, E_FAIL);


		FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

		CGameInstance* pInstance = GetSingle(CGameInstance);

		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


		_uint NumMaterial = m_pModel->Get_NumMaterial();



		FAILED_CHECK(GetSingle(CUtilityMgr)->BindOnShader_DissolveTexture(m_pShaderCom, "g_BurnRampTexture", "g_NoiseTexture"));

		_float VisualValue = 1 - (_float(5.f - m_fDissolveTime) / 5.f);


		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fVisualValue", &(VisualValue), sizeof(_float)));


		for (_uint i = 0; i < NumMaterial; i++)
		{
			for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

			FAILED_CHECK(m_pModel->Render(m_pShaderCom, 12, i, "g_BoneMatrices"));
		}


	}
	else
	{

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


	}


	return _int();
}

_int CEyepot::LightRender()
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

_int CEyepot::Update_DmgCalculate(_double fDeltaTime)
{

	if (m_bDeathAnimStart)return 0;
	if (m_fHP <= 0)
	{
		if (!m_bDeathAnimStart)
		{

			ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
			m_fDmgAmount = 0;
			m_DmgPassedTime = 0;

			m_bDeathAnimStart = true;
			m_pModel->Change_AnimIndex(23, 0.15, true);
			if (m_pSteamSoundDesc && m_pSteamSoundDesc->iIdentificationNumber == 30)m_pSteamSoundDesc->bStopSoundNow = true;
		}

		return 0;
	}

	if (m_DmgPassedTime <= 0)
	{
		if (m_fDmgAmount > 0)
		{
			if (m_bIsDmgAnimUpdated[2])
			{
				m_pModel->Change_AnimIndex_ReturnTo(21 + rand()%2, 2, 0.15, true);
			}

			ZeroMemory(m_bIsDmgAnimUpdated, sizeof(_bool) * 3);
			m_fDmgAmount = 0;
			m_DmgPassedTime = 0;
			m_pSubTransformCom->Set_IsOwnerDead(true);

		}
		return 0;
	}
	m_DmgPassedTime -= fDeltaTime;





	if (!m_bIsDmgAnimUpdated[0] && m_fMaxHP * 0.1 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(15, 2, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternDelayTime = 0;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(1.5f);
		m_bIsDmgAnimUpdated[0] = true;



		if (m_pSteamSoundDesc && m_pSteamSoundDesc->iIdentificationNumber == 30)m_pSteamSoundDesc->bStopSoundNow = true;
	}
	else if (!m_bIsDmgAnimUpdated[1] && m_fMaxHP * 0.15 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(16, 2, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_PatternDelayTime = 0;
		m_pTransformCom->Set_MoveSpeed(1.5f);
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 10);
		m_bIsDmgAnimUpdated[1] = true;


		if (m_pSteamSoundDesc && m_pSteamSoundDesc->iIdentificationNumber == 30)m_pSteamSoundDesc->bStopSoundNow = true;
	}
	else if (!m_bIsDmgAnimUpdated[2] && m_fMaxHP * 0.2 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_UntilTo(17, 18, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_PatternDelayTime = 0;
		m_pTransformCom->Set_MoveSpeed(1.5f);
		m_bIsDmgAnimUpdated[2] = true;
		m_DmgPassedTime = 5;

		if (m_pSteamSoundDesc && m_pSteamSoundDesc->iIdentificationNumber == 30)m_pSteamSoundDesc->bStopSoundNow = true;
	}

	return _int();
}

void CEyepot::CollisionTriger(_uint iMyColliderIndex, CGameObject * pConflictedObj, CCollider * pConflictedCollider, _uint iConflictedObjColliderIndex, CollisionTypeID eConflictedObjCollisionType)
{
	switch (eConflictedObjCollisionType)
	{

	case Engine::CollisionType_Player:
	{
		if (!m_bIsPatternFinished && (m_ePattern == 0 || m_ePattern == 2))
		{
			if (!lstrcmp(pConflictedObj->Get_NameTag(), L"Alice"))
			{
				pConflictedCollider->Set_Conflicted();
				((CPlayer*)(pConflictedObj))->Add_Dmg_to_Player(rand() % 2 + 3);
			}
		}
	}
	break;

	case Engine::CollisionType_PlayerWeapon:
	{

		if (!lstrcmp(pConflictedObj->Get_NameTag(), L"VopalBlade"))
		{

			SOUNDDESC tSoundDesc;

			tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			tSoundDesc.vMinMax = _float2(0, 35);
			tSoundDesc.fTargetSound = 0.5f;
			wstring SoundTrack = L"";
			SoundTrack = L"Weapon_vorpal_imp_metalshield0" + to_wstring(rand() % 6 + 1) + L".ogg";

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
			SoundTrack = L"Weapon_hobby_imp_metalshield0" + to_wstring(rand() % 6 + 1) + L".ogg";

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

HRESULT CEyepot::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
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

		if (m_bIsJumping && m_pModel->Get_NowAnimIndex() == 6)
		{
			m_bIsJumping = false; 
			m_tParticleDesc2.FixedTarget = CaculatedPos;
			GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc2);
			m_pModel->Change_AnimIndex_ReturnTo_Must(7, 2, 0.08f, true);

			{

				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 35);
				tSoundDesc.fTargetSound = 0.5f;

				wstring SoundTrack = L"";
				SoundTrack = L"Eyepot_deathfall02.ogg";

				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}

			{

				SOUNDDESC tSoundDesc;

				tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
				tSoundDesc.vMinMax = _float2(0, 35);
				tSoundDesc.fTargetSound = 0.5f;

				wstring SoundTrack = L"";

				SoundTrack = L"MapObject_hatterbuild_jumpland0" + to_wstring(rand() % 3 + 1) + L".ogg";
				//SoundTrack = L"MapObject_shrinkflower_open.ogg";

				g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
			}



		}
	}

	return S_OK;
}

_int CEyepot::Update_Pattern(_double fDeltaTime)
{
	m_PatternPassedTime += fDeltaTime;

	if (m_bIsPatternFinished)
	{
		m_ePattern += 1;
		if (m_ePattern > 2) m_ePattern = 0;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
		m_iPatternCount = 0;
		m_bIsJumping = false;
		m_bIsFarPattern = false;
		
	}

	switch (m_ePattern)
	{
	case 0:
		if (!m_bIsJumping && !m_bIsAddForceActived)
		{
			if (m_iPatternCount < 3)
			{
				if (!m_iPatternCount)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					m_pModel->Change_AnimIndex_UntilNReturn_Must(5, 6, 6, 0.15, true);
					m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
					Add_Force(m_pTransformCom, m_vLookDir.XMVector() + (XMVectorSet(0, 1, 0, 0)), 120);
					m_bIsJumping = true;
					m_iPatternCount++;
				}
				else if (m_pModel->Get_NowAnimIndex() == 2 && m_pModel->Get_PlayRate() > 0.1)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					m_pModel->Change_AnimIndex_UntilNReturn_Must(5, 6, 6, 0.15, true);
					m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
					Add_Force(m_pTransformCom, m_vLookDir.XMVector() + (XMVectorSet(0, 1, 0, 0) * 3.f), 80);
					m_LevitationTime = 0.5f;

					m_bIsJumping = true;
					m_iPatternCount++;
				}
			}
			else
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0;
				m_PatternDelayTime = 0;
				m_iPatternCount = 0;
				m_bIsJumping = false;
			}

		}
		else
		{
			m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
		}


		break;
	case 1:
		if (!m_iPatternCount)
		{
			m_bIsFarPattern = true;
			m_iPatternCount++;

			_Vector Dir = XMVector3Normalize(XMVectorSetY( m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS)- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));

			_float RandFloat = GetSingle(CUtilityMgr)->RandomFloat(-0.5, 0.5);
			if (RandFloat < 0)
			{
				_Vector Left = XMVector3Cross(XMVectorSet(0, 1, 0, 0), Dir)* -1;

				m_vLookDir = XMVector3Normalize(Dir* (1 + RandFloat) + (Left * -RandFloat));
			}
			else
			{
				_Vector vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), Dir);

				m_vLookDir = XMVector3Normalize(Dir* (1 - RandFloat) + (vRight  * RandFloat));
			}


		}

		if (m_iPatternCount < 4)
		{
			static _uint iChecker = 0;


			if (m_bIsFarPattern)
			{
				if (Distance_BetweenPlayer(m_pTransformCom) < 10)
				{
					if (!m_pModel->Get_IsHavetoBlockAnimChange())
					{

						m_pTransformCom->Set_MoveSpeed(16.5f);
						m_pModel->Change_AnimIndex(4, 0.15, false);
						FAILED_CHECK(FarPatternWander(fDeltaTime));
					}
				}
				else
				{
					m_bIsFarPattern = false;
					m_pTransformCom->Set_MoveSpeed(1.5f);
					m_pModel->Change_AnimIndex_UntilTo(8, 12, 0.3, true);
					iChecker = 0;
				}

			}
			else
			{
				_uint Temp = m_pModel->Get_NowAnimIndex();

				
				if (m_pModel->Get_NowAnimIndex() == 8 && iChecker == 0)
				{
					m_pSubTransformCom->Set_IsOwnerDead(false);
					GetSingle(CUtilityMgr)->Create_ParticleObject(m_eNowSceneNum, m_tParticleDesc);
					{

						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 25);
						tSoundDesc.fTargetSound = 0.35f;

						wstring SoundTrack = L"";
						SoundTrack = L"Eyepot_tella_steam.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}
					{

						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 25);
						tSoundDesc.fTargetSound = 0.35f;

						wstring SoundTrack = L"";
						SoundTrack = L"Eyepot_tella02.ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					{

						SOUNDDESC tSoundDesc;
						tSoundDesc.pTransform = m_pTransformCom;
						tSoundDesc.vMinMax = _float2(0, 25);
						tSoundDesc.fTargetSound = 0.1f;
						tSoundDesc.iIdentificationNumber = 30;


						wstring SoundTrack = L"";
						SoundTrack = L"c1_steamloop0" + to_wstring(rand() % 4 + 1) + L".ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc,&m_pSteamSoundDesc);
					}
					

					iChecker++;
				}

				if (m_pModel->Get_NowAnimIndex() == 9)
				{

					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
				}

				if (iChecker == 1&& m_pModel->Get_NowAnimIndex() == 11 && m_pModel->Get_PlayRate() > 0.7)
				{
					m_pSubTransformCom->Set_IsOwnerDead(true);
					if (m_pSteamSoundDesc && m_pSteamSoundDesc->iIdentificationNumber == 30)m_pSteamSoundDesc->bStopSoundNow = true;

					CEyepotChainGranade::ECGDESC tDesc;

					tDesc.vPosition = m_pColliderCom->Get_ColliderPosition(3);
					tDesc.MeshKinds = 0;

					tDesc.MoveDir = XMVector3Normalize(XMVectorSetY(m_pColliderCom->Get_ColliderPosition(3).XMVector() - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					g_pGameInstance->Add_GameObject_To_Layer(m_eNowSceneNum, TAG_LAY(Layer_MonsterBullet), L"ProtoType_EyepotChainGranade", &tDesc);
				
					{

						SOUNDDESC tSoundDesc;

						tSoundDesc.pTransform = nullptr;
						tSoundDesc.vPosition = tDesc.vPosition;
						tSoundDesc.vMinMax = _float2(0, 35);
						tSoundDesc.fTargetSound = 1.f;

						wstring SoundTrack = L"";
						SoundTrack = L"Eyepot_projectile_explode0" + to_wstring(rand() % 2 + 1) + L".ogg";

						//SoundTrack = L"MapObject_shrinkflower_open.ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
					}


					iChecker++;
				}



				if (m_pModel->Get_NowAnimIndex() == 12)
				{
					if (m_pModel->Get_PlayRate() > 0.5)
					{
						m_PatternPassedTime = 0;
						m_iPatternCount++;
						if (m_iPatternCount < 4)
							m_pModel->Change_AnimIndex_ReturnTo_Must(13, 12, 0.3, false);
					}
					else if (Distance_BetweenPlayer(m_pTransformCom) < 30)
					{
						m_iPatternCount++;
						m_bIsFarPattern = true;
						m_pModel->Change_AnimIndex_ReturnTo_Must(14, 4, 0.3, true);

						_Vector Dir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));

						_float RandFloat = GetSingle(CUtilityMgr)->RandomFloat(-0.5, 0.5);
						if (RandFloat < 0)
						{
							_Vector Left = XMVector3Cross(XMVectorSet(0, 1, 0, 0), Dir)* -1;

							m_vLookDir = XMVector3Normalize(Dir* (1 + RandFloat) + (Left * -RandFloat));
						}
						else
						{
							_Vector vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), Dir);

							m_vLookDir = XMVector3Normalize(Dir* (1 - RandFloat) + (vRight  * RandFloat));
						}


					}
				}

			}

		}
		else
		{
			m_pModel->Set_BlockAnim(false);
			m_pModel->Change_AnimIndex_ReturnTo(14,1, 0.15,true);
			m_bIsPatternFinished = true;
			m_PatternDelayTime = 0;
			m_PatternPassedTime = 0;
			m_iPatternCount = 0;
			m_bIsJumping = false;
			m_bIsFarPattern = false;
		}

		break;
	case 2:
		if (!m_iPatternCount)
		{
			m_pModel->Change_AnimIndex(3, 0.15, true);
			m_pTransformCom->Set_MoveSpeed(16.5f);
			m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
			m_iPatternCount++;
		}


		else
		{

			if (m_PatternPassedTime < 5)
			{
				FAILED_CHECK(DashPatternWander( fDeltaTime));
			}
			else
			{
				m_pModel->Set_BlockAnim(false);
				m_bIsPatternFinished = true;
				m_PatternDelayTime = 0;
				m_PatternPassedTime = 0;
				m_iPatternCount = 0;
				m_bIsJumping = false;
				m_bIsFarPattern = false;
				m_pTransformCom->Set_MoveSpeed(1.5f);
			}
		}


		break;
	case 3:

		break;


	default:
		break;
	}


	return _int();
}

void CEyepot::Add_Dmg_to_Monster(_float iDmgAmount)
{
	if (m_bIsDmgAnimUpdated[2])
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(19 + rand() % 2, 18, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternDelayTime = 0;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(1.5f);
		m_bIsDmgAnimUpdated[2] = true;
	

		{

			SOUNDDESC tSoundDesc;

			tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			tSoundDesc.vMinMax = _float2(0, 25);
			tSoundDesc.fTargetSound = 0.35f;

			wstring SoundTrack = L"";
			SoundTrack = L"Eyepot_paincreak0" + to_wstring(rand() % 5 + 1) + L".ogg";

			//SoundTrack = L"MapObject_shrinkflower_open.ogg";

			g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
		}
		{

			SOUNDDESC tSoundDesc;
			tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
			tSoundDesc.vMinMax = _float2(0, 25);
			tSoundDesc.fTargetSound = 0.5f;


			wstring SoundTrack = L"";
			SoundTrack = L"Eyepot_melee_impact0" + to_wstring(rand() % 3 + 1) + L".ogg";

			//SoundTrack = L"MapObject_shrinkflower_open.ogg";
			g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
		}

		m_fHP -= iDmgAmount;
		m_fDmgAmount += iDmgAmount;
	}
	else
	{
		m_DmgPassedTime = MonsterDmgTime;
		m_fDmgAmount += iDmgAmount;

		m_fHP -= iDmgAmount;
	}
}

HRESULT CEyepot::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Eyepot), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

	//Pivot  : 0.000000f , 1.189999f , 0.000000f , 1
	//size  : 2.969998f , 1.000000f , 1.000000f , 
	ColliderDesc.vScale = _float3(9.f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 1.189999f, 0.000000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	//size  : 1.529999f , 1.000000f , 1.000000f , 
	ColliderDesc.vScale = _float3(4.5f, 1.000000f, 1.000000f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);	
	ColliderDesc.vPosition = _float4(0.000000f, 3.54f, 0.000000f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();

	//size  : 1.210000f , 0.710000f , 1.180000f , 
	//Pivot  : 0.000000f , 0.500000f , -0.100000f , 1
	ColliderDesc.vScale = _float3(5.00000f, 2.130000f, 5.0f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 1.065000f, 0.f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_OBB, &ColliderDesc));
	m_pColliderCom->Set_ParantBuffer();



	/*

Pivot  : -3.559997f , 0.000000f , -4.330004f , 1
size  : 1.000000f , 1.000000f , 1.000000f
	*/
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(-3.559997f, 0.000000f, -4.330004f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[0] = m_pModel->Find_AttachMatrix_InHirarchyNode("Bone_Gob03");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[0].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();

	//Pivot  : 0.000000f , 0.000000f , -5.580033f , 1
	//size  : 1.000000f , 1.000000f , 1.000000f  

	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.000000f, 0.000000f, -5.580033f, 1);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));
	m_ArrCollisionAttach[1] = m_pModel->Find_AttachMatrix_InHirarchyNode("Pot");
	NULL_CHECK_RETURN(m_ArrCollisionAttach[1].pDefaultPivotMat, E_FAIL);
	m_pColliderCom->Set_ParantBuffer();


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 1.5f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_SubTransform), (CComponent**)&m_pSubTransformCom, &tDesc));
	

	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	return S_OK;
}

HRESULT CEyepot::FarPatternWander(_double fDeltaTime)
{
	_uint iChecker = 0;

	
	if (!(Check_Movable_Terrain(m_pTransformCom, m_vLookDir.XMVector() * -1, 0.1f)))
	{
		m_bIsFarPattern = false;
		m_pTransformCom->Set_MoveSpeed(1.5f);
		m_pModel->Change_AnimIndex_UntilTo(8, 12, 0.15, true);
	}
	else
	{
		m_pTransformCom->LookDir(m_vLookDir.XMVector() * 0.15f + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 0.85f);
		m_pTransformCom->MovetoDir((m_vLookDir.XMVector() * -1), fDeltaTime);

	}





	return S_OK;
}

HRESULT CEyepot::DashPatternWander(_double fDeltaTime)
{
	if (!(Check_Movable_Terrain(m_pTransformCom, m_vLookDir.XMVector(), 0.1f)))
	{
		m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
	}
	else
	{
		m_pTransformCom->LookDir(m_vLookDir.XMVector() * 0.15f + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * 0.85f);
		m_pTransformCom->MovetoDir((m_vLookDir.XMVector()), fDeltaTime);

	}
	return S_OK;
}

HRESULT CEyepot::Ready_ParticleDesc()
{
	m_tParticleDesc.eParticleTypeID = Particle_Cone;

	m_tParticleDesc.FollowingTarget = m_pSubTransformCom;

	m_tParticleDesc.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc.szTextureLayerTag = L"Dust5";
	m_tParticleDesc.iSimilarLayerNum = 1;

	m_tParticleDesc.TextureChageFrequency = 1;
	m_tParticleDesc.vTextureXYNum = _float2(5, 4);

	m_tParticleDesc.TotalParticleTime = 9999999999.f;
	m_tParticleDesc.EachParticleLifeTime = 0.34f;
	m_tParticleDesc.MaxParticleCount = 25;

	m_tParticleDesc.SizeChageFrequency = 1;
	m_tParticleDesc.ParticleSize = _float3(0.1f);
	m_tParticleDesc.ParticleSize2 = _float3(3.5f);

	m_tParticleDesc.ColorChageFrequency = 1;
	m_tParticleDesc.TargetColor = _float4(0.5f,0.5f,0.5f, 1.f);
	m_tParticleDesc.TargetColor2 = _float4(0.f);


	m_tParticleDesc.Particle_Power = 15;
	m_tParticleDesc.PowerRandomRange = _float2(0.8f, 1.f);
	m_tParticleDesc.SubPowerRandomRange = _float2(5.f, 6.f);

	m_tParticleDesc.vUp = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_UP);

	m_tParticleDesc.MaxBoundaryRadius = 999999.f;

	m_tParticleDesc.m_bIsUI = false;
	m_tParticleDesc.m_bUIDepth = 0;

	m_tParticleDesc.ParticleStartRandomPosMin = _float3(-0.1f, -1.f, -0.1f);
	m_tParticleDesc.ParticleStartRandomPosMax = _float3(0.1f, -1.f, 0.1f);

	m_tParticleDesc.DepthTestON = true;
	m_tParticleDesc.AlphaBlendON = true;

	m_tParticleDesc.m_fAlphaTestValue = 0.18f;
	m_tParticleDesc.m_iPassIndex = 5;


	

	m_tParticleDesc2.eParticleTypeID = Particle_Spread;

	m_tParticleDesc2.FollowingTarget = nullptr;

	m_tParticleDesc2.szTextureProtoTypeTag = TAG_CP(Prototype_Texture_PlayerEffect);
	m_tParticleDesc2.szTextureLayerTag = L"Dust4";
	m_tParticleDesc2.iSimilarLayerNum = 1;

	m_tParticleDesc2.TextureChageFrequency = 1;
	m_tParticleDesc2.vTextureXYNum = _float2(5, 4);

	m_tParticleDesc2.TotalParticleTime = 0.1f;
	m_tParticleDesc2.EachParticleLifeTime = 0.6f;
	m_tParticleDesc2.MaxParticleCount = 25;

	m_tParticleDesc2.SizeChageFrequency = 1;
	m_tParticleDesc2.ParticleSize = _float3(4.0f);
	m_tParticleDesc2.ParticleSize2 = _float3(3.5f);

	m_tParticleDesc2.ColorChageFrequency = 1;
	m_tParticleDesc2.TargetColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
	m_tParticleDesc2.TargetColor2 = _float4(0.f, 0.f,0.f, 0.1f);


	m_tParticleDesc2.Particle_Power = 15;
	m_tParticleDesc2.PowerRandomRange = _float2(0.8f, 1.f);
	m_tParticleDesc2.SubPowerRandomRange = _float2(5.f, 6.f);

	m_tParticleDesc2.vUp = _float3(0,1,0);

	m_tParticleDesc2.MaxBoundaryRadius = 999999.f;

	m_tParticleDesc2.m_bIsUI = false;
	m_tParticleDesc2.m_bUIDepth = 0;

	m_tParticleDesc2.ParticleStartRandomPosMin = _float3(0);
	m_tParticleDesc2.ParticleStartRandomPosMax = _float3(0);

	m_tParticleDesc2.DepthTestON = true;
	m_tParticleDesc2.AlphaBlendON = true;

	m_tParticleDesc2.m_fAlphaTestValue = 0.1f;
	m_tParticleDesc2.m_iPassIndex = 3;


	return S_OK;
}

HRESULT CEyepot::Adjust_AnimMovedTransform(_double fDeltatime)
{

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();


	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;

	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 1:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.409090909090)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate > 0.68181818181818)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}

				m_iAdjMovedIndex++;

			}
			else if (m_iAdjMovedIndex == 3 && PlayRate > 0.93333333333333)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			break;

		case 3:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			if (m_iAdjMovedIndex == 1 && PlayRate > 0.166666666666)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 2 && PlayRate > 0.444444444444)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			if (m_iAdjMovedIndex == 3 && PlayRate > 0.611111111111)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			if (m_iAdjMovedIndex == 4 && PlayRate > 0.777777777777)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			if (m_iAdjMovedIndex == 5 && PlayRate > 0.888888888888)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			break;

		case 4:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.85714285)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud * 2.f;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
	
			break;

		case 7:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.615384615384)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
	
			break;

		case 9:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.5625)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud *2.f;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.875)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud*2.f;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			

			break;


		case 14:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.4)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			break;
		case 15:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.1153846)
			{
				{

					SOUNDDESC tSoundDesc;
					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(0, 25);
					tSoundDesc.fTargetSound = 0.5f;


					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_melee_impact0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";
					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}

			break;
		case 16:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.1)
			{
				{

					SOUNDDESC tSoundDesc;
					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(0, 25);
					tSoundDesc.fTargetSound = 0.5f;


					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_melee_impact0" + to_wstring(rand() % 3 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";
					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}
				m_iAdjMovedIndex++;
			}


			if (m_iAdjMovedIndex == 1 && PlayRate > 0.425)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}

			break;


		case 21:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.53333333333)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.6666666666)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate > 0.8)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}

				m_iAdjMovedIndex++;

			}
			break;


		case 22:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.2105263157894)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.29824561403508)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate > 0.45614035087719)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}

				m_iAdjMovedIndex++;

			}
			break;


		case 23:

			if (m_iAdjMovedIndex == 0 && PlayRate > 0.f)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_deathcreak01.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.588235294117647)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = 0.5f;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_deathtwo01.ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			break;


		case 24:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.0)
			{

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.6153846153846)
			{
				{

					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 2 && PlayRate > 0.666666666666666)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}


				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 3 && PlayRate > 0.717948717948)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = m_pTransformCom;
					tSoundDesc.bFollowTransform = true;
					tSoundDesc.vMinMax = _float2(FootSetpSoudMin, FootSetpSoudMax);
					tSoundDesc.fTargetSound = FootSetpSoud;

					wstring SoundTrack = L"";
					SoundTrack = L"Eyepot_step0" + to_wstring(rand() % 10 + 1) + L".ogg";

					//SoundTrack = L"MapObject_shrinkflower_open.ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_UI, &tSoundDesc);
				}

				m_iAdjMovedIndex++;

			}
			break;

		}
	}




	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;

	return S_OK;
}

CEyepot * CEyepot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEyepot*	pInstance = new CEyepot(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CEyepot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEyepot::Clone(void * pArg)
{
	CEyepot*	pInstance = new CEyepot(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CEyepot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEyepot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pSubTransformCom);

	

}
