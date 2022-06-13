#include "stdafx.h"
#include "..\public\Teapot.h"
#include "Player.h"
#include "Camera_Main.h"




CTeapot::CTeapot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CTeapot::CTeapot(const CTeapot & rhs)
	: CWeapon(rhs)
{
}

HRESULT CTeapot::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CTeapot::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	
	return S_OK;
}

_int CTeapot::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	

	if (m_pModel->Get_NowAnimIndex() == 2)
	{
		if (!m_bSoundOn)
		{
			m_bSoundOn = true;


			{
				SOUNDDESC tSoundDesc;

				tSoundDesc.pTransform = ((CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerLastIndex(m_eNowSceneNum,TAG_LAY(Layer_Camera_Main))))->Get_Camera_Transform();
				tSoundDesc.vMinMax = _float2(0, 10);
				tSoundDesc.fTargetSound = 1.f;
				tSoundDesc.iIdentificationNumber = 4;
				tSoundDesc.bFollowTransform = true;
				wstring SoundTrack = L"";

				SoundTrack = L"Weapon_teacannon_charged_loop.ogg";

				FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_PLAYER, &tSoundDesc, &m_pSoundDesc));


			}


		}
		else
		{
			if (m_pSoundDesc != nullptr && m_pSoundDesc->iIdentificationNumber != 4)
			{

				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.pTransform = ((CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerLastIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main))))->Get_Camera_Transform();
					tSoundDesc.vMinMax = _float2(0, 10);
					tSoundDesc.fTargetSound = 1.f;
					tSoundDesc.iIdentificationNumber = 4;
					tSoundDesc.bFollowTransform = true;
					wstring SoundTrack = L"";

					SoundTrack = L"Weapon_teacannon_charged_loop.ogg";

					FAILED_CHECK(g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_PLAYER, &tSoundDesc, &m_pSoundDesc));


				}

			}


		}

	}
	else
	{
		if (m_pSoundDesc != nullptr && m_pSoundDesc->iIdentificationNumber == 4)
		{
			m_pSoundDesc->bStopSoundNow = true;
		}
	}



	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));


	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	//m_BoneMatrix = TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();
	//TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();
	m_BoneMatrix = TransformMatrix = m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix();



	return _int();
}

_int CTeapot::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CTeapot::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);




	_float4x4 ShaderMat = m_BoneMatrix.TransposeXMatrix();
	m_pShaderCom->Set_RawValue("g_AttechMatrix", &ShaderMat, sizeof(_float4x4));



	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));


	_uint NumMaterial = m_pModel->Get_NumMaterial();

	for (_uint i = 0; i < NumMaterial; i++)
	{

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
			FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, i, j, MODLETEXTYPE(j)));

		FAILED_CHECK(m_pModel->Render(m_pShaderCom, 6, i, "g_BoneMatrices"));
	}


	return _int();
}

_int CTeapot::LightRender()
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

HRESULT CTeapot::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Teapot), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	



	return S_OK;
}

CTeapot * CTeapot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTeapot*	pInstance = new CTeapot(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CTeapot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTeapot::Clone(void * pArg)
{
	CTeapot*	pInstance = new CTeapot(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CTeapot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTeapot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);


	
}
