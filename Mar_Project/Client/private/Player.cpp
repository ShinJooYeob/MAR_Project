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

	static bool test = false;
	if (g_pGameInstance->Get_DIKeyState(DIK_L)&DIS_Down)
		test = !test;

	if (!test)
	{
		FAILED_CHECK(Set_Player_On_Terrain());
	}
	else {
		FAILED_CHECK(Set_Player_On_Slieder(fDeltaTime));
	}
	
	FAILED_CHECK(Set_Camera_On_Player(fDeltaTime));

	if (g_pGameInstance->IsNeedToRender(m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS)))
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));

	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CPlayer::Render()
{
	if (__super::Render() < 0)
		return -1;


	NULL_CHECK_RETURN(m_pModel, E_FAIL);


	FAILED_CHECK(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CGameInstance* pInstance = GetSingle(CGameInstance);

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_VIEW), sizeof(_float4x4)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pInstance->Get_Transform_Float4x4_TP(PLM_PROJ), sizeof(_float4x4)));

	
	//FAILED_CHECK(m_pModel->Bind_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	//FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));


	FAILED_CHECK(m_pModel->Render(m_pShaderCom, 0));
	//FAILED_CHECK(m_pVIBufferCom->Render(m_pShaderCom, 0));

	return _int();
}

_int CPlayer::LateRender()
{
	if (__super::LateRender() < 0)
		return -1;




	return _int();
}

void CPlayer::Add_JumpForce(_float JumpPower)
{
	m_fJumpPower = m_fMaxJumpPower = JumpPower;
	m_LevitationTime = 0.0000000001f;
}

HRESULT CPlayer::SetUp_Components()
{


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VNAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Player), TAG_COM(Com_Model), (CComponent**)&m_pModel));

	//FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_SkyBox), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));


	CTransform::TRANSFORMDESC tDesc = {};

	tDesc.fMovePerSec = 5;
	tDesc.fRotationPerSec = XMConvertToRadians(60);
	tDesc.fScalingPerSec = 1;
	tDesc.vPivot = _float3(0, 0, 0);
	m_fSmallScale = 1.f;

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom, &tDesc));

	m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, _float3(0, 5.f, 0));
	m_fJumpPower = m_fMaxJumpPower= PlayerMaxJumpPower;
	m_LevitationTime = g_fDeltaTime;

	m_pMainCamera =(CCamera*)(g_pGameInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Camera_Main)));
	NULL_CHECK_RETURN(m_pMainCamera, E_FAIL);

	return S_OK;
}

HRESULT CPlayer::Input_Keyboard(_double fDeltaTime)
{

	CGameInstance* pInstance = GetSingle(CGameInstance);


	if (pInstance->Get_DIKeyState(DIK_LALT)&DIS_Down)
		GetSingle(CUtilityMgr)->SlowMotionStart();


	FAILED_CHECK(Smalling_Update(fDeltaTime, pInstance));
	FAILED_CHECK(Dash_Update(fDeltaTime, pInstance));
	if (!m_fDashPassedTime)
		FAILED_CHECK(Move_Update(fDeltaTime, pInstance));
	FAILED_CHECK(Jump_Update(fDeltaTime, pInstance));
	FAILED_CHECK(RockOn_Update(fDeltaTime, pInstance));

	
	return S_OK;
}

HRESULT CPlayer::Smalling_Update(_double fDeltaTime, CGameInstance* pInstance)
{

	BYTE LShiftState = pInstance->Get_DIKeyState(DIK_LCONTROL);
	if (LShiftState & DIS_Press)
	{
		if (LShiftState & DIS_Down)
		{
			m_fSmallPassedTime = 0;
		}
		else if (LShiftState & DIS_Up)
		{

			m_fSmallPassedTime = 0;
		}
		else 
		{
			if (m_fSmallPassedTime < 0.3f)
			{
				
				m_fSmallPassedTime += _float(fDeltaTime);
				m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, 1.f, PlayerSmallingSize, m_fSmallPassedTime, 0.3f);

				if (m_fSmallPassedTime > 0.3f)
				{
					m_fSmallScale = PlayerSmallingSize;
				}

				m_fSmallVisualTime = (1 - m_fSmallScale) / (1 - PlayerSmallingSize);

				m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
			}

		}
	}
	else {

		if (m_fSmallPassedTime < 0.3f)
		{
			m_fSmallPassedTime += _float(fDeltaTime);
			m_fSmallScale = pInstance->Easing(TYPE_QuarticOut, PlayerSmallingSize, 1.f, m_fSmallPassedTime, 0.3f);

			if (m_fSmallPassedTime > 0.3f)
			{
				m_fSmallScale = 1.f;
			}

			m_pTransformCom->Scaled_All(_float3(m_fSmallScale));
		}

		if (m_fSmallVisualTime > 0.f)
		{
			m_fSmallVisualTime -= _float(fDeltaTime);
			if (m_fSmallVisualTime < 0) m_fSmallVisualTime = 0;
		}
	}

	
	return S_OK;
}

HRESULT CPlayer::Move_Update(_double fDeltaTime, CGameInstance* pInstance)
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

		//if (PressedChecker[0]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
		//if (PressedChecker[1]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_LOOK);
		//if (PressedChecker[2]) Dir -= m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);
		//if (PressedChecker[3]) Dir += m_pTransformCom->Get_MatrixState_Normalized(CTransform::STATE_RIGHT);

		if (PressedChecker[0]) Dir += forword;
		if (PressedChecker[1]) Dir -= forword;
		if (PressedChecker[2]) Dir -= right;
		if (PressedChecker[3]) Dir += right;

		Dir = XMVector3Normalize(Dir);
		_Vector vOldLook = m_pTransformCom->Get_MatrixState(CTransform::STATE_LOOK);

		if (XMVector3Equal(Dir * -1, vOldLook))
			m_pTransformCom->LookDir(m_pTransformCom->Get_MatrixState(CTransform::STATE_RIGHT));
		else 
			m_pTransformCom->LookDir((Dir + vOldLook));
		
		m_pTransformCom->MovetoDir(XMVector3Normalize(Dir), fDeltaTime);
	}

	return S_OK;
}

HRESULT CPlayer::Jump_Update(_double fDeltaTime, CGameInstance* pInstance)
{
	if (pInstance->Get_DIKeyState(DIK_SPACE) & DIS_Down)
	{
		Add_JumpForce(PlayerMaxJumpPower * m_fSmallScale);
	}

	m_LevitationTime += fDeltaTime;
	_float fGravity = 0;
	if (m_fJumpPower > 0)
	{
		m_fJumpPower = _float(m_fMaxJumpPower * (m_LevitationTime - 1.f)* (m_LevitationTime - 1.f));
		fGravity = m_fJumpPower;

		if (m_fJumpPower <= 1.0f)
		{
			m_fJumpPower = 0;
			m_LevitationTime = fDeltaTime;
		}
	}
	else
	{
		fGravity = _float((m_LevitationTime) * (m_LevitationTime)* -29.4f);
	}


	m_pTransformCom->MovetoDir_bySpeed(XMVectorSet(0, 1.f, 0, 0), fGravity, fDeltaTime);

	return S_OK;
}

HRESULT CPlayer::Dash_Update(_double fDeltaTime, CGameInstance* pInstance, _float TotalDashTime)
{
	if (!m_fDashPassedTime && pInstance->Get_DIKeyState(DIK_LSHIFT) & DIS_Down)
	{
		//m_vDashDir = m_pMainCamera->Get_Camera_Transform()->Get_MatrixState_Float3(CTransform::STATE_LOOK);
		if (m_bIsRockOn)
		{
			m_vDashDir = XMVector3Normalize(((CTransform*)(m_pRockOnMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS) 
				- m_pTransformCom->Get_MatrixState(CTransform::STATE_POS));
		}
		else {
			m_vDashDir = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_LOOK);
		}
		m_vDashDir.y += m_pMainCamera->Get_Camera_Transform()->Get_MatrixState_Float3(CTransform::STATE_LOOK).y * 1.5f;
		m_fDashPassedTime =_float(fDeltaTime);
		m_fDashPower = PlayerMaxDashPower;
	}

	if (m_fDashPassedTime)
	{
		m_fDashPassedTime += _float(fDeltaTime);
		m_fDashPower = pInstance->Easing_Return(TYPE_ExpoInOut, TYPE_ExpoInOut,0, PlayerMaxDashPower, m_fDashPassedTime, TotalDashTime);
		
		if (m_fDashPassedTime > TotalDashTime)
		{
			m_fDashPower = 0;
			m_fDashPassedTime = 0;
		}

		m_pTransformCom->MovetoDir_bySpeed(m_vDashDir.XMVector(), m_fDashPower, fDeltaTime);
	}



	return S_OK;
}

HRESULT CPlayer::RockOn_Update(_double fDeltaTime, CGameInstance * pInstance)
{
	BYTE LTabState = pInstance->Get_DIKeyState(DIK_TAB);

	if (LTabState & DIS_Down)
	{
		if (LTabState & DIS_DoubleDown)
		{
			Safe_Release(m_pRockOnMonster);

			m_bIsRockOn = false;
		}
		else 
		{
			_Vector vPlayerPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);
			_Vector vLookVector = XMVector3Normalize(XMVectorSetY(vPlayerPos, 0)
				- XMVectorSetY(m_pMainCamera->Get_Camera_Transform()->Get_MatrixState(CTransform::STATE_POS), 0));

			//나중에 몬스터 넣으면 몬스터로 바꾸자
			list<CGameObject*>* MosterList = pInstance->Get_ObjectList_from_Layer(m_eNowSceneNum, TAG_LAY(Layer_JumpPad));

			_float	fNear = 5.f;
			_float Length = 0;
			CGameObject* pNearMonster = nullptr;
			
			_Vector vMonsterPos;

			for (auto& pMonster : *MosterList)
			{
				vMonsterPos = ((CTransform*)(pMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS);


				_float fCosValue = XMVectorGetX(XMVector3Dot(vLookVector, XMVector3Normalize
				(XMVectorSetY(vMonsterPos, 0) - XMVectorSetY(vPlayerPos, 0))));

				if (fCosValue > (sqrtf(3) / 2.f) && fNear > (Length = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos))))
				{
					fNear = Length;
					pNearMonster = pMonster;
				}

			}

			if (pNearMonster == nullptr)
				return S_FALSE;

			Safe_Release(m_pRockOnMonster);
			m_pRockOnMonster = pNearMonster;
			Safe_AddRef(m_pRockOnMonster);
			m_bIsRockOn = true;
		}
	}








	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Terrain()
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	
	CTerrain* pTerrain =(CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS),m_vOldPos.XMVector());

	if (bIsOn)
	{
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);
	}

	return S_OK;
}

HRESULT CPlayer::Set_Player_On_Slieder(_double fDeltatime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);
	CTerrain* pTerrain = (CTerrain*)(pInstance->Get_GameObject_By_LayerIndex(m_eNowSceneNum, TAG_LAY(Layer_Terrain)));

	_bool bIsOn = false;
	_float3 vPlaneWorldNormal;

	_float3 CaculatedPos = pTerrain->PutOnTerrain(&bIsOn, m_pTransformCom->Get_MatrixState(CTransform::STATE_POS), m_vOldPos.XMVector(),&vPlaneWorldNormal);

	if (bIsOn)
	{
		m_LevitationTime = 0;
		m_fJumpPower = -1.f;
		m_pTransformCom->Set_MatrixState(CTransform::STATE_POS, CaculatedPos);

		_Vector vRight;
		_Matrix matScale = m_pTransformCom->Get_MatrixScale_All();


		_Vector vUp = vPlaneWorldNormal.XMVector();

		m_pTransformCom->Set_MatrixState(CTransform::STATE_UP, vUp * matScale.r[CTransform::STATE_UP]);

		if (XMVector3Equal( _float3(0, 1, 0).XMVector(), vUp))
		{
			//__debugbreak();
			//MSGBOX("Can't Cross With Same Vector");
#ifdef _DEBUG
			OutputDebugString(TEXT("Error: Cross With Same Vector (Player)\n"));
#endif // _DEBUG


			vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.000001f, 1, 0, 0), vUp));
			m_pTransformCom->Set_MatrixState(CTransform::STATE_RIGHT, vRight * matScale.r[CTransform::STATE_RIGHT]);

		}
		else {

			vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0,1,0,0), vUp));
			m_pTransformCom->Set_MatrixState(CTransform::STATE_RIGHT, vRight * matScale.r[CTransform::STATE_RIGHT]);

		}

		m_pTransformCom->Set_MatrixState(CTransform::STATE_LOOK, XMVector3Normalize(XMVector3Cross(vRight, vUp))  * matScale.r[CTransform::STATE_LOOK]);


		m_pTransformCom->Move_Forward(fDeltatime);
	}
	return S_OK;
}

HRESULT CPlayer::Set_Camera_On_Player(_double fDeltaTime)
{
	CGameInstance* pInstance = GetSingle(CGameInstance);


	if (m_bIsRockOn)
	{
		_Vector vMonsterPos = ((CTransform*)(m_pRockOnMonster->Get_Component(TAG_COM(Com_Transform))))->Get_MatrixState(CTransform::STATE_POS);
		_Vector vPlayerPos = m_pTransformCom->Get_MatrixState(CTransform::STATE_POS);

		_float fCamDist = _float(m_CamDegreeAngle.z * m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f) );
		_Vector vMon2PlayerDir = XMVector3Normalize(XMVectorSetY(vPlayerPos,0) - XMVectorSetY(vMonsterPos,0));


		CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();

		pCamTransform->Set_MatrixState(CTransform::STATE_POS, 
			vPlayerPos + (XMVectorSet(0, 1, 0, 0) * 1 )- (vMon2PlayerDir * fCamDist));

		pCamTransform->LookAt(vMonsterPos);
	}
	else {

		_long		MouseMove = 0;
		if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
		{
			m_CamDegreeAngle.y += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

		}

		if (MouseMove = pInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
		{
			m_CamDegreeAngle.x += _float(fDeltaTime) * 20 * MouseMove * 0.1f;

			if (m_CamDegreeAngle.x < -20.f)m_CamDegreeAngle.x = -20.f;
			else if (m_CamDegreeAngle.x > 60.f)m_CamDegreeAngle.x = 60.f;
		}

		_Matrix NewCamMatrix;
		_float3 PlayerPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);

		NewCamMatrix = XMMatrixTranslation(0, 0, m_CamDegreeAngle.z
			* m_fSmallScale * (1 - m_fDashPower / PlayerMaxDashPower * 0.1f))
			* XMMatrixRotationX(XMConvertToRadians(m_CamDegreeAngle.x))
			* XMMatrixRotationY(XMConvertToRadians(m_CamDegreeAngle.y))
			* XMMatrixTranslation(PlayerPos.x, PlayerPos.y, PlayerPos.z);


		CTransform* pCamTransform = m_pMainCamera->Get_Camera_Transform();

		pCamTransform->Set_Matrix(NewCamMatrix);
	}
	FAILED_CHECK(m_pMainCamera->Set_ViewMatrix());
	FAILED_CHECK(m_pMainCamera->Set_ProjectMatrix());

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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	
	//Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pTextureCom);
	
	Safe_Release(m_pRockOnMonster);
	

}
