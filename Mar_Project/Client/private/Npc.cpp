#include "stdafx.h"
#include "..\public\Npc.h"



CNpc::CNpc(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CNpc::CNpc(const CNpc & rhs)
	:CGameObject(rhs)
{
}

HRESULT CNpc::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CNpc::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	memcpy(&m_tDesc, pArg, sizeof(NPCDESC));

	FAILED_CHECK(SetUp_Components());
	m_bNeedToRend = true;
	return S_OK;
}

_int CNpc::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;

	FAILED_CHECK(Move_Update(fDeltaTime,g_pGameInstance));

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bNeedToRend));
	m_bNeedToRend = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 10);
	return _int();
}

_int CNpc::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;






	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CNpc::Render()
{
	if (__super::Render() < 0)
		return -1;

	NULL_CHECK_RETURN(m_pModel, E_FAIL);
	FAILED_CHECK(SetUp_ConstTable());



	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 3, i, "g_BoneMatrices"));
	}

	return _int();
}

_int CNpc::LightRender()
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

HRESULT CNpc::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP((COMPONENTPROTOTYPEID)(Prototype_Mesh_StageSelectCop + m_tDesc.MeshKinds)), 
		TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(m_pModel->Change_AnimIndex(0));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 0.75f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, m_tDesc.StartPosition);
	m_bMoveToDest = true;
	return S_OK;
}

HRESULT CNpc::SetUp_ConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	return S_OK;
}


HRESULT CNpc::Move_Update(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_WalkingTime < m_WalkingTargetTime)
		{
			m_WalkingTime += fDeltaTime;

			_Vector Dir = XMVectorSet(1,0,0,0);

			if (m_bMoveToDest)
			{
				Dir = XMVector3Normalize(m_tDesc.DestPosition.XMVector() - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			}
			else
			{
				Dir = XMVector3Normalize(m_tDesc.StartPosition.XMVector() - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
			}
		

			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)*0.05f + vOldLook*0.95f);
			else
				m_pTransformCom->LookDir((Dir *0.05f + vOldLook * 0.95f));


			m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


			if (m_bMoveToDest)
			{
				if (m_tDesc.DestPosition.Get_Distance(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)) < 0.5f)
				{
					m_bMoveToDest = false;
				}
			}
			else
			{
				if (m_tDesc.StartPosition.Get_Distance(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS)) < 0.5f)
				{
					m_bMoveToDest = true;
				}
			}


			m_pModel->Change_AnimIndex(4);


		}
		else if (m_WalkingTime < m_WalkingTargetTime * 1.5f)
		{
			m_WalkingTime += fDeltaTime;
			m_pModel->Change_AnimIndex_UntilTo(0, 3);
		}
		else
		{
			m_WalkingTargetTime = GetSingle(CUtilityMgr)->RandomFloat(5, 8);
			m_WalkingTime = 0;
		}
	}


	if (m_tDesc.MeshKinds)
	{

		_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
		_double PlayRate = m_pModel->Get_PlayRate();


		if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
			m_iAdjMovedIndex = 0;

		if (PlayRate <= 0.95)
		{
			switch (iNowAnimIndex)
			{
			case 4:
				if (m_iAdjMovedIndex == 0 && PlayRate > 0)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 8);
						tSoundDesc.fTargetSound = 0.5f;
						wstring SoundTrack = L"";

						SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					m_iAdjMovedIndex++;
				}
				else if (m_iAdjMovedIndex == 1 && PlayRate > 0.5f)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 8);
						tSoundDesc.fTargetSound = 0.5f;
						wstring SoundTrack = L"";

						SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					m_iAdjMovedIndex++;
				}

				break;

			}
		}

		m_iOldAnimIndex = iNowAnimIndex;

	}
	else
	{
		_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
		_double PlayRate = m_pModel->Get_PlayRate();


		if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
			m_iAdjMovedIndex = 0;

		if (PlayRate <= 0.95)
		{
			switch (iNowAnimIndex)
			{
			case 4:
				if (m_iAdjMovedIndex == 0 && PlayRate > 0)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 8);
						tSoundDesc.fTargetSound = 0.5f;
						wstring SoundTrack = L"";

						SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					m_iAdjMovedIndex++;
				}
				else if (m_iAdjMovedIndex == 1 && PlayRate > 0.2441860f)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 8);
						tSoundDesc.fTargetSound = 0.5f;
						wstring SoundTrack = L"";

						SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					m_iAdjMovedIndex++;
				}
				else if (m_iAdjMovedIndex == 2 && PlayRate > 0.5f)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 8);
						tSoundDesc.fTargetSound = 0.5f;
						wstring SoundTrack = L"";

						SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					m_iAdjMovedIndex++;
				}
				else if (m_iAdjMovedIndex == 3 && PlayRate > 0.771084337)
				{
					{
						SOUNDDESC tSoundDesc;

						tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
						tSoundDesc.vMinMax = _float2(0, 8);
						tSoundDesc.fTargetSound = 0.5f;
						wstring SoundTrack = L"";

						SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

						g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
					}

					m_iAdjMovedIndex++;
				}


				break;

			}
		}

		m_iOldAnimIndex = iNowAnimIndex;

	}
	return S_OK;
}

CNpc * CNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CNpc*	pInstance = new CNpc(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNpc::Clone(void * pArg)
{
	CNpc*	pInstance = new CNpc(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNpc::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);



}
