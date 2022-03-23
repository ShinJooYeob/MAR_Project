#include "stdafx.h"
#include "..\Public\Camera_Main.h"



CCamera_Main::CCamera_Main(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CCamera(pDevice,pDeviceContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main & rhs)
	:CCamera(rhs)
{
}
//1
HRESULT CCamera_Main::Initialize_Prototype(void * pArg)
{
	if (FAILED(__super::Initialize_Prototype(pArg)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCamera_Main::Initialize_Clone(void * pArg)
{


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Main::Update(_double fDeltaTime)
{
	__super::Update(fDeltaTime);


	return _int();
}

_int CCamera_Main::LateUpdate(_double fDeltaTime)
{	

	__super::LateUpdate(fDeltaTime);


	return _int();
}

_int CCamera_Main::Render()
{



	return _int();
}

_int CCamera_Main::LateRender()
{


	return _int();
}


HRESULT CCamera_Main::SetUp_Components()
{

	//if (FAILED(__super::Add_Component(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Renderer), TAG_COM(Com_Renderer), (CComponent**)&m_ComRenderer)))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(SCENEID::SCENE_STATIC, TAG_CP(Prototype_Texture_Blank), TAG_COM(Com_Texture), (CComponent**)&m_ComTexture)))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(SCENEID::SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_ComVIBuffer)))
	//	return E_FAIL;



	return S_OK;
}



CCamera_Main * CCamera_Main::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CCamera_Main* pInstance = new CCamera_Main(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg))) {
		MSGBOX("Failed to Create CCamera_Main");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CCamera_Main::Clone(void * pArg)
{
	CCamera_Main* pInstance = new CCamera_Main(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg))) {
		MSGBOX("Failed to Create CCamera_Main");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();


}
