#include "..\Public\GameObject.h"
#include "GameInstance.h"



CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext),
	m_bIsClone(true), 
	m_bIsDead(false),
	m_szNameTag(nullptr)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CGameObject::Initialize_Prototype(void * pArg)
{
	return S_OK;
}

HRESULT CGameObject::Initialize_Clone(void * pArg)
{

	return S_OK;
}

_int CGameObject::Update(_double fDeltaTime)
{
	return _int();
}

_int CGameObject::LateUpdate(_double fDeltaTime)
{
	return _int();
}

_int CGameObject::Render()
{
	return _int();
}

_int CGameObject::LateRender()
{
	return _int();
}

CComponent* CGameObject::Get_Component(const _tchar * tagComponent)
{
	return Find_Components(tagComponent);
}


HRESULT CGameObject::Add_Component(_uint iScenenNum, const _tchar* tagPrototype, const _tchar* tagComponent, CComponent** ppOut, void* pArg)
{
	if (Find_Components(tagComponent) != nullptr)
		return E_FAIL;

	 CComponent* pCloneComponent = GetSingle(CGameInstance)->Clone_Component(iScenenNum, tagPrototype, pArg);

	 if (pCloneComponent == nullptr)
		 return E_FAIL;

	 m_mapComponets.emplace(tagComponent, pCloneComponent);

	 (*ppOut) = pCloneComponent;
	 Safe_AddRef(pCloneComponent);

	return S_OK;
}

HRESULT CGameObject::Change_Component(_uint iScenenNum, const _tchar * tagPrototype, const _tchar * tagComponent, CComponent ** ppOut, void * pArg)
{
	// ���� �̸��� ������Ʈ�� �ٸ� �ڽ��� ������Ʈ�� ��ü
	// EX) Rect -> Cube
	if (Find_Components(tagComponent) != nullptr)
	{
		// ���� ������Ʈ ����
		Safe_Release(*ppOut);
		Safe_Release(m_mapComponets[tagComponent]);

		CComponent* pCloneComponent = GetSingle(CGameInstance)->Clone_Component(iScenenNum, tagPrototype, pArg);
		if (pCloneComponent == nullptr)
			return E_FAIL;

		// Ű�� ������ �ش� ������Ʈ ���� �Ŀ� �ٽ� �ִ´�.
		m_mapComponets[tagComponent] = pCloneComponent;
		(*ppOut) = pCloneComponent;
		Safe_AddRef(pCloneComponent);
	}
	return S_OK;
}

CComponent * CGameObject::Find_Components(const _tchar * tagComponent)
{
	auto iter = find_if(m_mapComponets.begin(), m_mapComponets.end(), CTagFinder(tagComponent));

	if (iter == m_mapComponets.end())
		return nullptr;

	return iter->second;
}

//void CGameObject::Set_LoadData(OUTPUT_OBJECTINFO * pInfo)
//{
//	// OUTPUT_OBJECTINFO�� ������ ���Թ���
//	m_pLoadInfoData = pInfo;
//}


void CGameObject::Free()
{
	for (auto& ComponentPair : m_mapComponets)
		Safe_Release(ComponentPair.second);
	m_mapComponets.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

}
