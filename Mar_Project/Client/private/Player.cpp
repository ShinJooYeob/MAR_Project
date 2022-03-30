#include "stdafx.h"
#include "..\public\Player.h"
#include "Camera.h"
#include "Terrain.h"


CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext) 
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Prototype(pArg));


	return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

_int CPlayer::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(Input_Keyboard(fDeltaTime));


	return _int();
}

_int CPlayer::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;


	FAILED_CHECK(Set_Player_On_Terrain());
	FAILED_CHECK(Set_Camera_On_Player(fDeltaTime));

	if (GetSingle(CGameInstance)->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS)))
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));
	return _int();
}

_int CPlayer::Render()
{
	if (__super::Render() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	_float4x4		ViewFloat4x4 = pInstance->Get_Transform_Float4x4_TP(PLM_VIEW);
	_float4x4		ProjFloat4x4 = pInstance->Get_Transform_Float4x4_TP(PLM_PROJ);;


	m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewFloat4x4, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjFloat4x4, sizeof(_float4x4));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));



	FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CPlayer::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

HRESULT CPlayer::SetUp_Components()
{


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VCT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Cube), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_SkyBox), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));


	m_pMainCamera =(CCamera*)( GetSingle(CGameInstance)->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main)));

	NULL_CHECK_RETURN(m_pMainCamera, E_FAIL);

	return S_OK;
}

HRESULT CPlayer::Input_Keyboard(_double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	_int PressedChecker[4];
	ZeroMemory(PressedChecker, sizeof(_bool) * 4);


	PressedChecker[0] = _int(pInstance->Get_DIKeyState(DIK_W) & DIS_Press);
	PressedChecker[1] = _int(pInstance->Get_DIKeyState(DIK_S) & DIS_Press);
	PressedChecker[2] = _int(pInstance->Get_DIKeyState(DIK_A) & DIS_Press);
	PressedChecker[3] = _int(pInstance->Get_DIKeyState(DIK_D) & DIS_Press);

	if (PressedChecker[0] || PressedChecker[1] || PressedChecker[2] || PressedChecker[3])
	{
		m_pTransformCom->LookDir(XMVectorSetY(m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), 0)
			- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));


		_Vector Dir = XMVectorSet(0, 0, 0, 0);

		if (PressedChecker[0]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
		if (PressedChecker[1]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
		if (PressedChecker[2]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
		if (PressedChecker[3]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);

		m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);
	}

	if (pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
	{
		m_fJumpPower = PlayerMaxJumpPower;
		m_JumpTime += fDeltaTime;
	}

	_float fGravity = m_fJumpPower - m_JumpTime*m_JumpTime * m_fJumpPower;
	

	m_pTransformCom->MovetoDir(XMVectorSet(0, fGravity, 0, 0), fDeltaTime);
	m_JumpTime += fDeltaTime;
	m_fJumpPower -= m_JumpTime * m_JumpTime * PlayerMaxJumpPower;

	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Terrain()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	
	CTerrain* pTerrain =(CTerrain*)( pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));

	if (bIsOn)
	{
		m_JumpTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}

	return S_OK;
}

HRESULT CPlayer::Set_Camera_On_Player(_double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);

	
	_long		MouseMove = 0;	
	if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
	{
		m_CamDegreeAngle.y += _float(fDeltaTime) *20 * MouseMove * 0.1f;
	}
	
	if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
	{
		m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;
	}

	_Matrix NewCamMatrix;
	_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

	NewCamMatrix = XMMatrixTranslation(0, 0, m_CamDegreeAngle.z) 
		* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
		* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
		* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


	CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();

	pCamTransform->Set_Matrix(NewCamMatrix);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CPlayer*	pInstance = new CPlayer(pDevice,pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	
}
