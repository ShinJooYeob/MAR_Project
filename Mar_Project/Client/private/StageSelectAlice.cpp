#include "stdafx.h"
#include "..\public\StageSelectAlice.h"
#include "Camera_Main.h"
#include "Terrain.h"
#include "GamePlayUI.h"

#define FootStepSound 0.5f




CStageSelectAlice::CStageSelectAlice(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CStageSelectAlice::CStageSelectAlice(const CStageSelectAlice & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStageSelectAlice::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CStageSelectAlice::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	if (pArg != nullptr)
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, *((_float3*)pArg));


	m_szNameTag = L"SelectAlice";

	return S_OK;
}

_int CStageSelectAlice::Update(_double fDeltaTime)
{
	if (m_eNowSceneNum == SCENE_LOADING || m_eNowSceneNum == SCENE_LOBY)
		return 0;

	if (__super::Update(fDeltaTime) < 0)return -1;
	m_pColliderCom->Update_ConflictPassedTime(fDeltaTime);


	FAILED_CHECK(Input_Keyboard(fDeltaTime));
	



	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime ));

	m_pColliderCom->Update_Transform(0, m_pTransformCom->Get_WorldMatrix()); 
	g_pGameInstance->Add_CollisionGroup(CollisionType_Player, this, m_pColliderCom);
#ifdef _DEBUG
	FAILED_CHECK(m_pRendererCom->Add_DebugGroup(m_pColliderCom));
#endif // _DEBUG

	return _int();
}

_int CStageSelectAlice::LateUpdate(_double fDeltaTime)
{
	if (m_eNowSceneNum == SCENE_LOADING || m_eNowSceneNum == SCENE_LOBY)
		return 0;

	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(Set_Player_On_Terrain());

	
	FAILED_CHECK(Set_Camera_On_Player(fDeltaTime));



	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	g_pGameInstance->Set_TargetPostion(PLV_PLAYER, m_vOldPos);
	return _int();
}

_int CStageSelectAlice::Render()
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

_int CStageSelectAlice::LightRender()
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




HRESULT CStageSelectAlice::SetUp_Components()
{

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(m_eNowSceneNum, TAG_CP(Prototype_Mesh_PlayerStageSelect), TAG_COM(Com_Model), (CComponent**)&m_pModel));
	FAILED_CHECK(m_pModel->Change_AnimIndex(0));




	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = PlayerMoveSpeed;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(0, 5.f, 0));


	m_pMainCamera =(CCamera_Main*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main)));
	NULL_CHECK_RETURN(m_pMainCamera, E_FAIL);


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Collider), TAG_COM(Com_Collider), (CComponent**)&m_pColliderCom));

	COLLIDERDESC			ColliderDesc;
	/* For.Com_SPHERE */
	ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));
	ColliderDesc.vScale = _float3(2.0f, 2.f, 2.f);
	ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
	FAILED_CHECK(m_pColliderCom->Add_ColliderBuffer(COLLIDER_SPHERE, &ColliderDesc));

	return S_OK;
}

HRESULT CStageSelectAlice::SetUp_ConstTable()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));




	return S_OK;
}

HRESULT CStageSelectAlice::Input_Keyboard(_double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(Move_Update(fDeltaTime, pInstance));
	FAILED_CHECK(Jump_Update(fDeltaTime, pInstance));


	return S_OK;
}

HRESULT CStageSelectAlice::Move_Update(_double fDeltaTime, CGameInstance* pInstance)
{

	if (!m_pModel->Get_IsHavetoBlockAnimChange())
	{

		_int PressedChecker[4];
		ZeroMemory(PressedChecker, sizeof(_bool) * 4);

		PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
		PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
		PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
		PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

		if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
		{
			//m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			//	- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector forword = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			_Vector right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forword);

			_Vector Dir = XMVectorSet(0, 0, 0, 0);

			/*
			//if (PressedChecker[0]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
			//if (PressedChecker[1]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
			//if (PressedChecker[2]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
			//if (PressedChecker[3]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
			*/

			if (PressedChecker[0]) Dir += forword;
			if (PressedChecker[1]) Dir -= forword;
			if (PressedChecker[2]) Dir -= right;
			if (PressedChecker[3]) Dir += right;

			Dir = XMVector3Normalize(Dir);
			_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

			if (XMVectorGetX(XMVector3Length(Dir + vOldLook)) < 0.1f)
				m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT) + vOldLook);
			else
				m_pTransformCom->LookDir((Dir *0.25 + vOldLook * 0.75));


			m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);


			if (pInstance->Get_DIKeyState(DIK_LSHIFT)  & DIS_Press)
			{
				m_pModel->Change_AnimIndex(7);
				m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed * 0.75f);
			}
			else
			{
				m_pModel->Change_AnimIndex(8);
				m_pTransformCom->Set_MoveSpeed(PlayerMoveSpeed * 0.25f);
			}



		}
		else {
				m_pModel->Change_AnimIndex_UntilTo(0, 5);


		}
	}


	static _uint				m_iOldAnimIndex = INT_MAX;
	static _uint				m_iAdjMovedIndex = 0;

	_uint iNowAnimIndex = m_pModel->Get_NowAnimIndex();
	_double PlayRate = m_pModel->Get_PlayRate();


	if (iNowAnimIndex != m_iOldAnimIndex || PlayRate > 0.95)
		m_iAdjMovedIndex = 0;

	if (PlayRate <= 0.95)
	{
		switch (iNowAnimIndex)
		{
		case 7:
		case 8:
			if (m_iAdjMovedIndex == 0 && PlayRate > 0)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(0, 5);
					tSoundDesc.fTargetSound = FootStepSound * 0.5f;
					wstring SoundTrack = L"";

					SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_PLAYER, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}
			else if (m_iAdjMovedIndex == 1 && PlayRate > 0.5f)
			{
				{
					SOUNDDESC tSoundDesc;

					tSoundDesc.vPosition = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
					tSoundDesc.vMinMax = _float2(0, 5);
					tSoundDesc.fTargetSound = FootStepSound * 0.5f;
					wstring SoundTrack = L"";

					SoundTrack = L"FootStep_rock0" + to_wstring(rand() % 9 + 1) + L".ogg";

					g_pGameInstance->PlaySoundW(SoundTrack.c_str(), CHANNEL_PLAYER, &tSoundDesc);
				}

				m_iAdjMovedIndex++;
			}

			break;

		}
	}



	m_iOldAnimIndex = iNowAnimIndex;
	return S_OK;
}

HRESULT CStageSelectAlice::Jump_Update(_double fDeltaTime, CGameInstance* pInstance)
{


	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), -1, fDeltaTime);

	return S_OK;
}

HRESULT CStageSelectAlice::Set_Player_On_Terrain()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	
	CTerrain* pTerrain =(CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	if (pTerrain == nullptr)
	{
		OutputDebugStringW(L"Not Exist Terrain Object!!\n");

		return S_FALSE;
	}

	_bool bIsOn = false;
	_uint eNowTile = Tile_End;


	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS),m_vOldPos.XMVector(),nullptr,&eNowTile);


	if (eNowTile == Tile_None)
	{

		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}
	else if (bIsOn)
	{
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}

	return S_OK;
}

HRESULT CStageSelectAlice::Set_Camera_On_Player(_double fDeltaTime)
{


	if (m_pMainCamera->Get_IsCamAction())return S_FALSE;


	CGameInstance* pInstance = GetSingle(CGameInstance);


#define CamLerpLate 0.2f
#define LowBreakpoint -50.f
#define HighBreakpoint 60.f



	{

		_long		MouseMove = 0;
		if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
		{
			m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

		}

		if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
		{
			m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			if (m_CamDegreeAngle.x < LowBreakpoint)m_CamDegreeAngle.x = LowBreakpoint;
			else if (m_CamDegreeAngle.x > HighBreakpoint)m_CamDegreeAngle.x = HighBreakpoint;
		}


		CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();
		_Matrix OldCamMat = pCamTransform->Get_WorldMatrix();
		_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
		_Matrix NewCamMatrix = XMMatrixTranslation(0, 1.5f , m_CamDegreeAngle.z	)
			* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
			* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
			* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


		NewCamMatrix.r[0] = XMVectorLerp(OldCamMat.r[0], NewCamMatrix.r[0], CamLerpLate);
		NewCamMatrix.r[1] = XMVectorLerp(OldCamMat.r[1], NewCamMatrix.r[1], CamLerpLate);
		NewCamMatrix.r[2] = XMVectorLerp(OldCamMat.r[2], NewCamMatrix.r[2], CamLerpLate);
		NewCamMatrix.r[3] = XMVectorLerp(OldCamMat.r[3], NewCamMatrix.r[3], CamLerpLate);

		pCamTransform->Set_Matrix(NewCamMatrix);

	}
	FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
	FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());


	return S_OK;
}

CStageSelectAlice * CStageSelectAlice::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CStageSelectAlice*	pInstance = new CStageSelectAlice(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CStageSelectAlice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CStageSelectAlice::Clone(void * pArg)
{
	CStageSelectAlice*	pInstance = new CStageSelectAlice(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CStageSelectAlice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStageSelectAlice::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	Safe_Release(m_pColliderCom);
	
}
