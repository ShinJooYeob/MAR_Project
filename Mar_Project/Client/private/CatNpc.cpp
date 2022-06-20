#include "stdafx.h"
#include "..\public\CatNpc.h"



CCatNpc::CCatNpc(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice,pDeviceContext)
{
}

CCatNpc::CCatNpc(const CCatNpc & rhs)
	:CGameObject(rhs)
{
}

HRESULT CCatNpc::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CCatNpc::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());
	m_bNeedToRend = true;
	return S_OK;
}

_int CCatNpc::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)return -1;

	FAILED_CHECK(Move_Update(fDeltaTime,g_pGameInstance));

	if (g_pGameInstance->Get_DIKeyState(DIK_Q) & DIS_Down)
	{
		MoveToNext();
	}

	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime, m_bNeedToRend));
	m_bNeedToRend = g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 10);
	return _int();
}

_int CCatNpc::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;






	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CCatNpc::Render()
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

_int CCatNpc::LightRender()
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

void CCatNpc::MoveToNext()
{
	m_bMoveIndex++;
	m_bMoveToDest = true;
	m_pModel->Change_AnimIndex_UntilNReturn_Must(2, 3,3);
}

HRESULT CCatNpc::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_StageSelectCat),TAG_COM(Com_Model), (CComponent**)&m_pModel));

	FAILED_CHECK(m_pModel->Change_AnimIndex_UntilNReturn(0,1,0));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 0.75f;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(76.032f, 20.f, 56.025f));
	m_pTransformCom->Rotation_CW(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(180));

	m_bMoveToDest = false;
	return S_OK;
}

HRESULT CCatNpc::SetUp_ConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	return S_OK;
}


HRESULT CCatNpc::Move_Update(_double fDeltaTime, CGameInstance * pInstance)
{
	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{
		if (m_bMoveToDest && m_pModel->Get_NowAnimIndex() == 3)
		{
			_Vector DestPos;

			if (m_bMoveIndex == 1)
				DestPos = XMVectorSet(84.715f, 20.f, 60.765f, 0);
			else if (m_bMoveIndex == 2)
				DestPos = XMVectorSet(96.602f, 20.f, 86.361f, 0);


			_Vector Dir = XMVectorSet(1, 0, 0, 0);


			Dir = XMVector3Normalize(DestPos - m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));



			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT)*0.05f + vOldLook*0.95f);
			else
				m_pTransformCom->LookDir((Dir *0.05f + vOldLook * 0.95f));


			m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);



			if (m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS).Get_Distance(DestPos) < 5.f)
			{
				m_bMoveToDest = false;
				if (m_bMoveIndex == 1)
				{
					m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(84.734f, 20.f, 78.619f));

					m_pModel->Change_AnimIndex_UntilNReturn_Must(0, 1, 0);
					m_pTransformCom->Rotation_CW(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(180));
				}
				else if (m_bMoveIndex == 2)
				{
					Set_IsDead();

				}

			}


			m_pModel->Change_AnimIndex(3);


		}
		else
		{

			if (m_pModel->Get_NowAnimIndex() > 2 || (m_pModel->Get_NowAnimIndex() == 1 && m_pModel->Get_PlayRate() > 0.95))
			{
				FAILED_CHECK(m_pModel->Change_AnimIndex_UntilNReturn(0, 1, 0));
			}
		}
	}




	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();


	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;

	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 2:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0.217391304)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(1000, 1001);
					tSoundDesc.fTargetSound = 1.f;
					wstring SoundTrack = L"";

					SoundTrack = L"Cat_Meow_0" + to_wstring(m_bMoveIndex) + L".ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_OBJECT, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}

			break;

		}
	}

	m_iOldAnimIndex = iNowAnimIndex;



	return S_OK;
}

CCatNpc * CCatNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CCatNpc*	pInstance = new CCatNpc(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CCatNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCatNpc::Clone(void * pArg)
{
	CCatNpc*	pInstance = new CCatNpc(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CCatNpc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCatNpc::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);



}
