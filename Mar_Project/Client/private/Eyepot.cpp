#include "stdafx.h"
#include "..\public\Eyepot.h"
#include "Terrain.h"



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
	m_fHP = m_fMaxHP = 32;

	m_iPatternCount = 0;
	m_bIsJumping = false;
	m_bIsFarPattern = false;

	return S_OK;
}

_int CEyepot::Update(_double fDeltaTime)
{

	
	//static float testFloat = 1.;
	//if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
	//m_pModel->Change_AnimIndex(1); //=>0.5
	//if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
	//m_pModel->Change_AnimIndex(3); //=> 5.5

	////FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.05f));

	//if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
	//{
	//testFloat += 0.1f;
	//m_pTransformCom->Set_MoveSpeed(testFloat);


	//string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	//wstring ttDebugLog;
	//ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//OutputDebugStringW(ttDebugLog.c_str());
	//}
	//else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
	//{
	//testFloat -= 0.1f;
	//m_pTransformCom->Set_MoveSpeed(testFloat);

	//string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
	//wstring ttDebugLog;
	//ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

	//OutputDebugStringW(ttDebugLog.c_str());

	//}
	//

	if (g_pGameInstance->Get_DIKeyState(DIK_C)&DIS_Down)
		Add_Dmg_to_Monster(1);

	if (g_pGameInstance->Get_DIKeyState(DIK_V)&DIS_Down)
	{
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_UP), 30);
	}


	_uint AnimIndex = m_pModel->Get_NowAnimIndex();


	if (!m_bIsDmgAnimUpdated[2])
	{
		if (m_bStartPos)
		{
			if (!m_bIsPatternFinished || Distance_BetweenPlayer(m_pTransformCom) < 3)
			{
				Update_Pattern(fDeltaTime);
			}
			else
			{
				if (!m_pModel->Get_IsUntillPlay() && !m_pModel->Get_IsHavetoBlockAnimChange())
				{
					m_pModel->Change_AnimIndex(1);
					FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime, 0.05f));
				}
			}
		}
		else//스타트 패턴
		{
			if (!m_bIsPatternFinished)
			{
				if (Distance_BetweenPlayer(m_pTransformCom) < 3)
				{
					m_pModel->Change_AnimIndex_ReturnTo_Must(24, 0, 0.15, true);
					m_bIsPatternFinished = true;
					m_PatternPassedTime = 0;
				}
			}
			else 
			{
				m_PatternPassedTime += fDeltaTime;

				if (m_PatternPassedTime > 2.5)
				{
					m_PatternPassedTime = 0;
					m_bStartPos = true;
				}

			}

		}

	}


	Update_DmgCalculate(fDeltaTime);

	m_bIsOnScreen = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS));

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bIsOnScreen));
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

_int CEyepot::LateRender()
{
	return _int();
}

_int CEyepot::Update_DmgCalculate(_double fDeltaTime)
{
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




		}
		return 0;
	}
	m_DmgPassedTime -= fDeltaTime;



	if (!m_bIsDmgAnimUpdated[0] && m_fMaxHP * 0.1 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(15, 2, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(0.5);
		m_bIsDmgAnimUpdated[0] = true;
	}
	else if (!m_bIsDmgAnimUpdated[1] && m_fMaxHP * 0.2 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_ReturnTo_Must(16, 2, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(0.5);
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK)* -1, 10);
		m_bIsDmgAnimUpdated[1] = true;
	}
	else if (!m_bIsDmgAnimUpdated[2] && m_fMaxHP * 0.4 < m_fDmgAmount)
	{
		m_pModel->Change_AnimIndex_UntilTo(17, 18, 0.15, true);
		m_bIsPatternFinished = true;
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(0.5);
		m_bIsDmgAnimUpdated[2] = true;
		m_DmgPassedTime = 5;

	}

	return _int();
}

HRESULT CEyepot::Set_Monster_On_Terrain(CTransform * pTransform, _double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	m_LevitationTime += fDeltaTime;
	_float fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);

	pTransform->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);


	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, pTransform->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector());

	if (bIsOn)
	{
		m_LevitationTime = 0;
		pTransform->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		if (m_bIsJumping)
		{
			m_bIsJumping = false;
			m_pModel->Change_AnimIndex_ReturnTo(7, 2, 0.15, true);

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
				m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS),0));
				m_pModel->Change_AnimIndex_UntilTo(5, 6, 0.15, true);
				m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
				Add_Force(m_pTransformCom, m_vLookDir.XMVector() + XMVectorSet(0,1,0,0), 50);
				m_bIsJumping = true;
				m_iPatternCount++;
			}
			else
			{
				m_bIsPatternFinished = true;
				m_PatternPassedTime = 0;
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

			if (m_bIsFarPattern)
			{
				if (Distance_BetweenPlayer(m_pTransformCom) < 8)
				{
					if (!m_pModel->Get_IsHavetoBlockAnimChange())
					{

						m_pTransformCom->Set_MoveSpeed(5.5);
						m_pModel->Change_AnimIndex(4, 0.15, false);
						FAILED_CHECK(FarPatternWander(fDeltaTime));
					}
				}
				else
				{
					m_bIsFarPattern = false;
					m_pTransformCom->Set_MoveSpeed(0.5);
					m_pModel->Change_AnimIndex_UntilTo(8, 12, 0.15, true);
				}

			}
			else
			{
				if (m_pModel->Get_NowAnimIndex() == 9)
				{
					m_vLookDir = XMVector3Normalize(XMVectorSetY(m_pPlayerTransfrom->Get_MatrixState(CTransform::STATE_POS) - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0));
					m_pTransformCom->LookDir(m_vLookDir.XMVector()*(0.15f) + m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK) * (0.85f));
				}

				if (m_pModel->Get_NowAnimIndex() == 12)
				{
					if (m_PatternPassedTime > 3)
					{
						m_PatternPassedTime = 0;
						m_iPatternCount++;
						if (m_iPatternCount < 4)
							m_pModel->Change_AnimIndex_ReturnTo_Must(13, 12, 0.15, false);
					}
					else if (Distance_BetweenPlayer(m_pTransformCom) < 5)
					{
						m_bIsFarPattern = true;
						m_pModel->Change_AnimIndex_ReturnTo_Must(14, 4, 0.15, true);

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
			m_pTransformCom->Set_MoveSpeed(5.5);
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
				m_PatternPassedTime = 0;
				m_iPatternCount = 0;
				m_bIsJumping = false;
				m_bIsFarPattern = false;
				m_pTransformCom->Set_MoveSpeed(0.5);
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
		m_PatternPassedTime = 0;
		m_pTransformCom->Set_MoveSpeed(0.5);
		m_bIsDmgAnimUpdated[2] = true;
	

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




	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 0.6f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	return S_OK;
}

HRESULT CEyepot::FarPatternWander(_double fDeltaTime)
{
	_uint iChecker = 0;

	
	if (!(Check_Movable_Terrain(m_pTransformCom, m_vLookDir.XMVector() * -1, 0.1f)))
	{
		m_bIsFarPattern = false;
		m_pTransformCom->Set_MoveSpeed(0.5);
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

}
