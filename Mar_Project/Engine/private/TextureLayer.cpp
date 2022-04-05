#include "..\public\TextureLayer.h"
#include "Shader.h"

CTextureLayer::CTextureLayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CTextureLayer::CTextureLayer(const CTextureLayer & rhs)
	: CComponent(rhs)
	, m_vecTextures(rhs.m_vecTextures)
{
	for (auto& pTexture : m_vecTextures)
		Safe_AddRef(pTexture);
}


HRESULT CTextureLayer::Bind_OnShader(CShader * pShader, const char * pValueName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_vecTextures.size())
		return E_FAIL;

	return pShader->Set_Texture(pValueName, m_vecTextures[iTextureIndex]);

}

HRESULT CTextureLayer::Add_Another_Texture(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
	_tchar		szExt[MAX_PATH] = TEXT("");


	for (_uint i = 0; i < iNumTextures; ++i)
	{
		wsprintf(szTextureFilePath, pTextureFilePath, i);

		ID3D11ShaderResourceView*		pSRV = nullptr;

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			FAILED_CHECK(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV));
		}

		else if (!lstrcmp(szExt, TEXT(".tga")))
		{
			__debugbreak();
			MSGBOX("Not Available TGA File");
			return E_FAIL;
		}

		else
		{
			FAILED_CHECK(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV));
		}

		m_vecTextures.push_back(pSRV);
	}



	return S_OK;
}

HRESULT CTextureLayer::Add_Model_Texture(_uint iIndex, const _tchar * pTextureFilePath)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (iIndex >= m_vecTextures.size())
	{
		OutputDebugString(TEXT("Already Exist Texture Map\n"));
		__debugbreak();
		return E_FAIL;
	}

	_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
	_tchar		szExt[MAX_PATH] = TEXT("");


	wsprintf(szTextureFilePath, pTextureFilePath);

	ID3D11ShaderResourceView*		pSRV = nullptr;

	_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

	if (!lstrcmp(szExt, TEXT(".dds")))
	{
		FAILED_CHECK(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV));
	}

	else if (!lstrcmp(szExt, TEXT(".tga")))
	{
		__debugbreak();
		MSGBOX("Not Available TGA File");
		return E_FAIL;
	}

	else
	{
		FAILED_CHECK(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV));
	}

	m_vecTextures[iIndex] = pSRV;



	return S_OK;
}

HRESULT CTextureLayer::Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
	_tchar		szExt[MAX_PATH] = TEXT("");

	m_vecTextures.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{
		wsprintf(szTextureFilePath, pTextureFilePath, i);

		ID3D11ShaderResourceView*		pSRV = nullptr;

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			FAILED_CHECK(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV));
		}

		else if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;

		else
		{
			FAILED_CHECK(CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV));
		}

		m_vecTextures.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTextureLayer::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype(nullptr));

	m_vecTextures.resize(AI_TEXTURE_TYPE_MAX);

	return S_OK;
}

HRESULT CTextureLayer::Initialize_Clone(void * pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg));
	return S_OK;
}

CTextureLayer * CTextureLayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTextureLayer*	pInstance = new CTextureLayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSGBOX("Failed to Creating CTextureLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTextureLayer * CTextureLayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTextureLayer*	pInstance = new CTextureLayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSGBOX("Failed to Creating 3D Model TextureLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTextureLayer::Clone(void * pArg)
{
	CTextureLayer*	pInstance = new CTextureLayer(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Creating CTextureLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextureLayer::Free()
{
	__super::Free();

	for (auto& pTexture : m_vecTextures)
		Safe_Release(pTexture);

	m_vecTextures.clear();


}
