#include "stdafx.h"
#include "..\public\Grinder.h"




CGrinder::CGrinder(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CWeapon(pDevice,pDeviceContext)
{
}

CGrinder::CGrinder(const CGrinder & rhs)
	: CWeapon(rhs)
{
}

HRESULT CGrinder::Initialize_Prototype(void * pArg)
{
	__super::Initialize_Prototype(pArg);

	return S_OK;
}

HRESULT CGrinder::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));


	FAILED_CHECK(SetUp_Components());

	
	return S_OK;
}

_int CGrinder::Update(_double fDeltaTime)
{
	if (__super::Update(fDeltaTime) < 0)
		return -1;
	if (m_bIsDead) return 0;
	
	//XMMatrixRotationX(XMConvertToRadians(-27.03f))*
	//XMMatrixRotationY(XMConvertToRadians(-10.069997f))*
	//XMMatrixRotationZ(XMConvertToRadians(-201.929626f))*
	//XMMatrixTranslation(-0.61f, 0.0200f, -1.290f);
	/*{
		//Rot: -20.865261, -10.069997, -201.929626,
		//Pivot: -0.610000, 0.005000, -1.365000,

		static _float3 testFloat3 = _float3(-0.610000, 0.020000, -1.290000);
		static _float3 RotFloat3 = _float3(-27.030010, -10.069997, -201.929626);
		static _float value = 0.015f;
		static _int kind = 0;

		_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
			XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
			XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

		tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
		m_pTransformCom->Set_Matrix(tt);

		CGameInstance* m_pInstance = g_pGameInstance;
		if (m_pInstance->Get_DIKeyState(DIK_UP) & DIS_Down)
		{
			testFloat3.x -= value;


			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());
		}

		else if (m_pInstance->Get_DIKeyState(DIK_DOWN) & DIS_Down)
		{
			testFloat3.x += value;

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_LEFT) & DIS_Down)
		{
			testFloat3.z += value;

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_RIGHT) & DIS_Down)
		{
			testFloat3.z -= value;

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_DELETE) & DIS_Down)
		{
			testFloat3.y += value;
			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}

		else if (m_pInstance->Get_DIKeyState(DIK_END) & DIS_Down)
		{
			testFloat3.y -= value;
			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);
			string ttszLog = "Pivot  : " + to_string(testFloat3.x) + " , " + to_string(testFloat3.y) + " , " + to_string(testFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_PGUP) & DIS_Press)
		{
			switch (kind)
			{
			case 0:
				RotFloat3.x += value;
				break;
			case 1:
				RotFloat3.y += value;
				break;
			case 2:
				RotFloat3.z += value;
				break;

			default:
				break;
			}

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);

			string ttszLog = "rot  : " + to_string(RotFloat3.x) + " , " + to_string(RotFloat3.y) + " , " + to_string(RotFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_PGDN) & DIS_Press)
		{
			switch (kind)
			{
			case 0:
				RotFloat3.x -= value;
				break;
			case 1:
				RotFloat3.y -= value;
				break;
			case 2:
				RotFloat3.z -= value;
				break;

			default:
				break;
			}

			_Matrix tt = XMMatrixRotationX(XMConvertToRadians(RotFloat3.x))*
				XMMatrixRotationY(XMConvertToRadians(RotFloat3.y))*
				XMMatrixRotationZ(XMConvertToRadians(RotFloat3.z));

			tt.r[3] = XMVectorSetW(testFloat3.XMVector(), 1);
			m_pTransformCom->Set_Matrix(tt);

			string ttszLog = "Rot  : " + to_string(RotFloat3.x) + " , " + to_string(RotFloat3.y) + " , " + to_string(RotFloat3.z) + " , " + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());



		}
		else if (m_pInstance->Get_DIKeyState(DIK_TAB) & DIS_Down)
		{
			kind++;
			if (kind > 2)kind = 0;

			string ttszLog = "kind  : " + to_string(kind) + "\n";

			wstring ttDebugLog;
			ttDebugLog.assign(ttszLog.begin(), ttszLog.end());

			OutputDebugStringW(ttDebugLog.c_str());
		}

	}
*/



	_Matrix			TransformMatrix = XMLoadFloat4x4(m_tATBMat.pUpdatedNodeMat) * XMLoadFloat4x4(m_tATBMat.pDefaultPivotMat);

	TransformMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	TransformMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	TransformMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	m_BoneMatrix = XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()* TransformMatrix * m_tWeaponDesc.pParantTransform->Get_WorldMatrix());



	FAILED_CHECK(m_pModel->Update_AnimationClip(fDeltaTime));

	return _int();
}

_int CGrinder::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	if (m_bIsDead) return 0;


	//if (m_bIsOnScreen)	
		FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this));
	m_vOldPos = m_pTransformCom->Get_MatrixState_Float3(CTransform::STATE_POS);
	return _int();
}

_int CGrinder::Render()
{

	NULL_CHECK_RETURN(m_pModel, E_FAIL);



	_float4x4 ShaderMat = m_BoneMatrix;
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

_int CGrinder::LightRender()
{

	return _int();
}

HRESULT CGrinder::SetUp_Components()
{
	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_pRendererCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VAM), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Mesh_Grinder), TAG_COM(Com_Model), (CComponent**)&m_pModel));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Transform), TAG_COM(Com_Transform), (CComponent**)&m_pTransformCom));


	return S_OK;
}

CGrinder * CGrinder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGrinder*	pInstance = new CGrinder(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(nullptr)))
	{
		MSGBOX("Failed to Created CGrinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGrinder::Clone(void * pArg)
{
	CGrinder*	pInstance = new CGrinder(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Created CGrinder");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGrinder::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModel);
	
}
