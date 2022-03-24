#include "stdafx.h"
#include "..\public\Player.h"



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


	if (GetSingle(CGameInstance)->Get_DIKeyState(DIK_RETURN)&DIS_Down)
	{
		static int test = 0;
		test++;

		if (test > 2)
			test = 0;

		switch (test)
		{
		case 0:
			FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"lower"));
			break;
		case 1:
			FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"upper"));
			break;
		case 2:
			FAILED_CHECK(m_pTextureCom->Change_TextureLayer(L"number"));
			break;
		default:
			break;
		}
	}




	return _int();
}

_int CPlayer::LateUpdate(_double fDeltaTime)
{
	if (__super::LateUpdate(fDeltaTime) < 0)
		return -1;

	FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this));

	return _int();
}

_int CPlayer::Render()
{
	if (__super::Render() < 0)
		return -1;
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_Matrix));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_Matrix));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &XMMatrixIdentity(), sizeof(_Matrix));

	FAILED_CHECK(m_pTextureCom->Bind_OnShader_AutoFrame(m_pShaderCom, "g_DiffuseTexture", g_fDeltaTime));



	m_pVIBufferCom->Render(m_pShaderCom, 0);

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


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Shader_VT), TAG_COM(Com_Shader), (CComponent**)&m_pShaderCom));


	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_VIBuffer_Rect), TAG_COM(Com_VIBuffer), (CComponent**)&m_pVIBufferCom));

	FAILED_CHECK(Add_Component(SCENE_STATIC, TAG_CP(Prototype_Texture_Player), TAG_COM(Com_Texture), (CComponent**)&m_pTextureCom));


	

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

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
