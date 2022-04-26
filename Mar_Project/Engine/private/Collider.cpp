#include "..\public\Collider.h"
#include "ColliderBuffer.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs),
	m_vecPrototypeCollideBuffer(rhs.m_vecPrototypeCollideBuffer)
{
	for (auto& pColliderProto : m_vecPrototypeCollideBuffer)
		Safe_AddRef(pColliderProto);
	

}

HRESULT CCollider::Initialize_Prototype()
{
	m_vecPrototypeCollideBuffer.reserve(COLLIDERTYPE_END);


	for (_uint i = 0; i < COLLIDERTYPE_END; i++)
	{
		CColliderBuffer* pColliderBuffer = CColliderBuffer::Create(m_pDevice, m_pDeviceContext, COLLIDERTYPE(i));
		NULL_CHECK_RETURN(pColliderBuffer, E_FAIL);
		m_vecPrototypeCollideBuffer.push_back(pColliderBuffer);

	}


	return S_OK;
}

HRESULT CCollider::Initialize_Clone(void * pArg)
{



	return S_OK;
}

void CCollider::Update_Transform(_uint iIndex, _fMatrix Transform)
{
	if (iIndex >= m_vecColliderBuffer.size())
	{
		__debugbreak();
		return;
	}

	m_vecColliderBuffer[iIndex]->Update_Transform(Transform);

}

_int CCollider::Add_ColliderBuffer(COLLIDERTYPE eColliderType, COLLIDERDESC * pColliderDesc)
{

	CColliderBuffer* pColliderBuffer = m_vecPrototypeCollideBuffer[eColliderType]->Clone(pColliderDesc);
	
	NULL_CHECK_RETURN(pColliderBuffer, -1);

	m_vecColliderBuffer.push_back(pColliderBuffer);

	return _int(m_vecColliderBuffer.size() - 1);
}


_int CCollider::Render()
{
	for (auto& pColliderBuffer : m_vecColliderBuffer)
	{
		if (pColliderBuffer->Render() < 0)
		{
			__debugbreak();
			return -1;
		}
	}


	return 0;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCollider*	pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSGBOX("Failed to Creating CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSGBOX("Failed to Creating CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();


	for (auto& pColliderBuffer : m_vecColliderBuffer)
		Safe_Release(pColliderBuffer);
	m_vecColliderBuffer.clear();



	for (auto& pColliderProto : m_vecPrototypeCollideBuffer)
		Safe_Release(pColliderProto);

	m_vecPrototypeCollideBuffer.clear();
}
