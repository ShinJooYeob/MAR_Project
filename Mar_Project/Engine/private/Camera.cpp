#include "..\Public\Camera.h"
#include "GameInstance.h"






CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CCamera::CCamera(const CCamera & rhs)
	:CGameObject(rhs)
{
	m_CameraDesc = rhs.m_CameraDesc;
}

HRESULT CCamera::Initialize_Prototype(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	return S_OK;
}

HRESULT CCamera::Initialize_Clone(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	if (m_pDevice == nullptr || m_pDeviceContext == nullptr)
		return E_FAIL;

	//m_pTransform = CTransform::Create(m_pDevice, m_pDeviceContext);

	//if (nullptr == m_pTransform)
	//	return E_FAIL;

	//m_bIsOrtho = true;

	//m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	//_float3 vRight, vUp, vLook;

	//vLook = (m_CameraDesc.vAt - m_CameraDesc.vEye).Get_Nomalize();

	//vRight = m_CameraDesc.vAxisY.Get_Cross(vLook).Get_Nomalize();

	//vUp = vLook.Get_Cross(vRight).Get_Nomalize();

	//m_pTransform->Set_MatrixState(CTransform::STATE_RIGHT, vRight);
	//m_pTransform->Set_MatrixState(CTransform::STATE_UP, vUp);
	//m_pTransform->Set_MatrixState(CTransform::STATE_LOOK, vLook);
	//m_pTransform->Set_MatrixState(CTransform::STATE_POS, m_CameraDesc.vEye);


	return S_OK;
}

_int CCamera::Update(_double fDeltaTime)
{
	if (m_pDevice == nullptr || m_pDeviceContext == nullptr)
		return -1;

	//if (FAILED(m_pGraphicDevice->SetTransform(D3DTS_VIEW, &(m_pTransform->Get_InverseWorldMatrix()))))
	//	return -1;

	//_Matrix matProj;
	//

	//if (FAILED(Set_ProjectMatrix(m_bIsOrtho)))
	//	return E_FAIL;


	return _int();
}

_int CCamera::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CCamera::Render()
{
	return _int();
}

_int CCamera::LateRender()
{


	return _int();
}

HRESULT CCamera::Set_ProjectMatrix(_bool bIsOrtho)
{
	//_Matrix matProj;

	//if (bIsOrtho)
	//	D3DXMatrixOrthoLH(&matProj, m_fOrthoZoomInOut * m_CameraDesc.fAspect, m_fOrthoZoomInOut, -m_CameraDesc.fFar, m_CameraDesc.fFar);

	//else
	//	D3DXMatrixPerspectiveFovLH(&matProj, m_CameraDesc.fFovy*m_fOrthoZoomInOut / 16.f, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);


	//if (FAILED(m_pGraphicDevice->SetTransform(D3DTS_PROJECTION, &matProj)))
	//	return E_FAIL;

	return S_OK;
}


void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
}
