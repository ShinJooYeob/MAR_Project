#include "stdafx.h"
#include "..\public\Executor.h"
#include "Scythe.h"
#include "Terrain.h"



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

	m_fHP = m_fMaxHP = 96;

	m_pModel->Change_AnimIndex(2);


	FAILED_CHECK(SetUp_Weapon());

	return S_OK;
}

_int CExecutor::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);
	Update_DmgCalculate(fDeltaTime);

	//if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	//	m_pModel->Change_AnimIndex(0);
	//if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	//	Add_Dmg_to_Monster(10);

	//m_pModel->Change_AnimIndex(16) // 슬램더
	//walk 2.8 / 0.05f

	//run 5.5 / 0.1
	if (m_bSpwanAnimFinished)
	{


		if (g_pGameInstance->Get_DIKeyState(DIK_F1)&DIS_Down)
		{
			m_bIsPatternFinished = true;
			m_PatternDelayTime = 0;
			m_PatternPassedTime = 0;
			m_ePattern = 0;

			m_pModel->Change_AnimIndex_ReturnTo(15, 0, 0.15, true);

			if (m_bIsBerseked)
			{

				m_bIsBerseked = false;
				m_pTransformCom->Set_MoveSpeed(2.8f);
			}
			else
			{
				m_bIsBerseked = true;
				m_pTransformCom->Set_MoveSpeed(5.5f);
			}
		}


		if (m_pModel->Get_NowAnimIndex() != 19 && m_pModel->Get_NowAnimIndex() != 15)
		{

			if (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 10)
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

				iStartCounter++;
			}
			else if (iStartCounter == 1 && PlayRate > 0.84&& m_pModel->Get_NowAnimIndex() == 2)
			{
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.2f, _float4(0.2f));
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

			if (m_pModel->Get_PlayRate() > 0.1568627 && !iChecker && m_pModel->Get_NowAnimIndex() == 18)
			{
				iChecker++;
				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 0.5f, _float4(1.f));
			}
			if (m_pModel->Get_PlayRate() > 0.95 && iChecker ==1 && m_pModel->Get_NowAnimIndex() == 18)
			{
				m_pModel->Change_AnimIndex(0,0.15,true);
				iChecker++;
			}

			m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(128, 27.39f, 226.782f));

			if (iChecker==2 && m_SpwanPassedTime > 13.8)
			{
				iChecker++;
				m_pModel->Change_AnimIndex_UntilNReturn_Must(20, 22, 14, 0.08f, true);
			}
		}
		else
		{
			static _uint iChecker = 0;
			if (!iChecker&& m_pModel->Get_NowAnimIndex() == 14 && m_pModel->Get_PlayRate() > 0.1230769)
			{

				GetSingle(CUtilityMgr)->Start_ScreenEffect(CUtilityMgr::ScreenEffect_CamShaking, 2.f, _float4(1.3f));
				iChecker++;
			}

			if (iChecker == 1&& m_pModel->Get_NowAnimIndex() == 14 && m_pModel->Get_PlayRate() > 0.95)
			{
				m_bSpwanAnimFinished = true;
				iChecker++;
			}
		}



	}


	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS),7.8f);
	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime * ((m_bIsBerseked) ? 1.5f : 1.f), m_bIsOnScreen));
	FAILED_CHECK(Adjust_AnimMovedTransform(fDeltaTime));

	if (m_bIsOnScreen)
	{

		_Matrix			TransformMatrix = XMLoadFloat4x4(m_tCollisionAttachPtr.pUpdatedNodeMat) * XMLoadFloat4x4(m_tCollisionAttachPtr.pDefaultPivotMat);

		TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
		TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
		TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

		for (_uint i = 0 ; i < m_pColliderCom->Get_NumColliderBuffer() ; i++)
			m_pColliderCom->Update_Transform(i, TransformMatrix * m_pTransformCom->Get_WorldMatrix());

		

		g_pGameInstance->Add_CollisionGroup(CollisionType_Monster, this, m_pColliderCom);

		m_vecWeapon[0]->Update(fDeltaTime);
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

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

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
	return _int();
}

_int CExecutor::LateRender()
{
	return _int();
}

_int CExecutor::Update_DmgCalculate(_double fDeltaTime)
{
	if (m_DmgPassedTime <= 0)
	{
		if (m_fDmgAmount > 0)
		{
			m_fDmgAmount = 0;
			m_DmgPassedTime = 0;
		}
		return 0;
	}

	m_DmgPassedTime -= fDeltaTime;

	if (m_fDmgAmount != 0 )
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(19, 0, 0.15, true);

		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_PatternDelayTime = 3;
		m_fDmgAmount = 0;
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 10);
	}
	

	return 0;
}

_int CExecutor::Update_Pattern(_double fDeltaTime)
{

	m_PatternPassedTime += fDeltaTime;




	if (m_bIsBerseked)
		//버닝 모드
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
					Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.25f + XMVectorSet(0, 1, 0, 0) * 0.25f, 100);
					//Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.75f + XMVectorSet(0, 1, 0, 0) * 0.25f, 100);
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
						m_PatternDelayTime = 2;
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
						m_PatternDelayTime = 2;
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
						m_PatternDelayTime = 2;
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
						m_PatternDelayTime = 2;
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
		//일반 모드
	{
		if (m_bIsPatternFinished)
		{
			m_ePattern += 1;
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
					Add_Force(m_pTransformCom, m_vLookDir.XMVector() * 0.25f + XMVectorSet(0, 1, 0, 0) * 0.25f, 100);
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
						m_PatternDelayTime = 2;
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
						m_PatternDelayTime = 2;
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
						m_PatternDelayTime = 2;
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
			m_bIsJumping = false;
			m_PatternDelayTime = 3;
			m_pModel->Change_AnimIndex_ReturnTo_Must(7, 0, 0, true);
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

	static float Power = 7.2f;
	if (g_pGameInstance->Get_DIKeyState(DIK_UP)& DIS_Down)
	{
		Power += 0.2f;
		string ttszLog = "//Power  : " + to_string(Power) + "\n";

		wstring ttDebugLog;
		ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

		OutputDebugStringW(ttDebugLog.c_str());

	}
	else if (g_pGameInstance->Get_DIKeyState(DIK_DOWN)& DIS_Down)
	{
		Power -= 0.2f;
		string ttszLog = "//Power  : " + to_string(Power) + "\n";

		wstring ttDebugLog;
		ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

		OutputDebugStringW(ttDebugLog.c_str());
	}


	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 8:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.35714)
			{
				Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 30);


				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate > 0.785714)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 5,0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}



			break;

		case 9:
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.296875)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 8.8f, 0.5f / ((m_bIsBerseked) ? 1.5f : 1.f));


				m_iAdjMovedIndex++;
			}
			break;

		case 10:
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.001)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 7.2f, 1.f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate >= 0.58666)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 3, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			
			break;
		case 11:
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.44444)
			{
				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}


			break;

		case 12://shout
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.2777)
			{

				//공격 파장 소환
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}


			break;

		case 13://shoutNEQ
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.1578947)
			{

				//공격 파장 소환
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate >= 0.5864661)
			{

				//Eq 소환
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}


			break;

		case 14://shoutNSummon
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.123076)
			{

				//공격 파장 소환
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}
			if (m_iAdjMovedIndex == 1 && PlayRate >= 0.72180)
			{

				//몬스터 소환
				//Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK), 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
				m_iAdjMovedIndex++;
			}


			break;


			
			
		case 19://Knockback
			if (m_iAdjMovedIndex == 0 && PlayRate >= 0.01)
			{

				Add_Force_Smooth(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * -1 , 4.0, 0.4f / ((m_bIsBerseked) ? 1.5f : 1.f));
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
