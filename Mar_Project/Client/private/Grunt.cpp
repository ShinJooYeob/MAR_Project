#include "stdafx.h"
#include "..\public\Grunt.h"



CGrunt::CGrunt(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CMonster(pDevice,pDeviceContext)
{
}

CGrunt::CGrunt(const CGrunt & rhs)
	: CMonster(rhs)
{
}

HRESULT CGrunt::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CGrunt::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));


	return S_OK;
}

_int CGrunt::Update(_double fDeltaTime)
{


	/*
	static float testFloat = 1.;
	if (g_pGameInstance->Get_DIKeyState(DIK_1)&DIS_Down)
		m_pModel->Change_AnimIndex(2);
	if (g_pGameInstance->Get_DIKeyState(DIK_2)&DIS_Down)
		m_pModel->Change_AnimIndex(6);
	if (g_pGameInstance->Get_DIKeyState(DIK_3)&DIS_Down)
		m_pModel->Change_AnimIndex(17);


	if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
	{
		testFloat += 0.1f;
		m_pTransformCom->Set_MoveSpeed(testFloat);


		string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
		wstring ttDebugLog;
		ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

		OutputDebugStringW(ttDebugLog.c_str());
	}
	else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
	{
		testFloat -= 0.1f;
		m_pTransformCom->Set_MoveSpeed(testFloat);

		string ttszLog = "Monster Speed: " + to_string(testFloat) + "\n";
		wstring ttDebugLog;
		ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

		OutputDebugStringW(ttDebugLog.c_str());

	}
	*/

	if (g_pGameInstance->Get_DIKeyState(DIK_V)&DIS_Down)
	{
		Add_Force(m_pTransformCom, m_pTransformCom->Get_MatrixState(CTransform::STATE_UP), 30);
	}


	if (Distance_BetweenPlayer(m_pTransformCom) < 3)
	{

	}
	else
	{
		FAILED_CHECK(__super::Update_WanderAround(m_pTransformCom, fDeltaTime,0.05f));
	}



	return _int();
}

_int CGrunt::LateUpdate(_double fDeltaTime)
{

	FAILED_CHECK(__super::Set_Monster_On_Terrain(m_pTransformCom,fDeltaTime));

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));

	if (g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS)))
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CGrunt::Render()
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

_int CGrunt::LateRender()
{
	return _int();
}

_int CGrunt::Update_Pattern(_double fDeltaTime)
{

	if (m_bIsPatternFinished)
	{
		m_ePattern = rand() % 4;
		m_bIsPatternFinished = false;
		m_PatternPassedTime = 0;
	}
	else
	{
		m_PatternPassedTime += fDeltaTime;

		switch (m_ePattern)
		{
		case 0:
			if (m_PatternPassedTime == fDeltaTime)
			{
				m_pModel->Change_AnimIndex_UntilTo(3, 4, 0.15, true);
			}
			else
			{
				if (!m_pModel->Get_IsHavetoBlockAnimChange())
				{
					m_bIsPatternFinished = true;
				}
			}


			break;
		case 1:
			if (m_PatternPassedTime == fDeltaTime)
			{
				m_pModel->Change_AnimIndex_UntilTo(3, 4, 0.15, true);
			}
			else
			{
				if (!m_pModel->Get_IsHavetoBlockAnimChange())
				{
					m_bIsPatternFinished = true;
				}
			}


			break;
		case 2:
			if (m_PatternPassedTime == fDeltaTime)
			{
				m_pModel->Change_AnimIndex_UntilTo(3, 4, 0.15, true);
			}
			else
			{
				if (!m_pModel->Get_IsHavetoBlockAnimChange())
				{
					m_bIsPatternFinished = true;
				}
			}

			break;
		case 3:
			if (m_PatternPassedTime == fDeltaTime)
			{
				m_pModel->Change_AnimIndex_UntilTo(3, 4, 0.15, true);
			}
			else
			{
				if (!m_pModel->Get_IsHavetoBlockAnimChange())
				{
					m_bIsPatternFinished = true;
				}
			}

			break;
		case 4:
			if (m_PatternPassedTime == fDeltaTime)
			{
				m_pModel->Change_AnimIndex_UntilTo(3, 4, 0.15, true);
			}
			else
			{
				if (!m_pModel->Get_IsHavetoBlockAnimChange())
				{
					m_bIsPatternFinished = true;
				}
			}

			break;

		default:
			break;
		}



	}


	/*
	2	=> 0.5
	6	=>6.5
	17	=>0.3
	*/


	return _int();
}

HRESULT CGrunt::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_Grunt), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(2));




	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 0.5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	__super::SetUp_WanderLook(m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK));

	return S_OK;
}

CGrunt * CGrunt::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CGrunt*	pInstance = new CGrunt(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrunt::Clone(void * pArg)
{
	CGrunt*	pInstance = new CGrunt(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGrunt");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrunt::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);

}
