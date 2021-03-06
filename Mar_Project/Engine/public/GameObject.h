#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject&  rhs);
	virtual ~CGameObject() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual _int Update(_double fDeltaTime);
	virtual _int LateUpdate(_double fDeltaTime);
	
	virtual _int Render();
	virtual _int LightRender();

	virtual void CollisionTriger(_uint iMyColliderIndex, CGameObject* pConflictedObj,class CCollider* pConflictedCollider ,_uint iConflictedObjColliderIndex,CollisionTypeID eConflictedObjCollisionType) {};

	virtual void Set_NowSceneNum(_uint eNowSceneNum) {	m_eNowSceneNum = eNowSceneNum; };

	void Set_NameTag(const _tchar* szNameTag) {	m_szNameTag = szNameTag;};
	const _tchar* Get_NameTag() { return m_szNameTag; };
	
	virtual _float	Get_RenderSortValue() { return m_fRenderSortValue; };
	virtual _float	Compute_RenderSortValue();

	virtual void Set_IsDead() { m_bIsDead = true; }
	_bool Get_IsDead() { return m_bIsDead; }


	virtual HRESULT ReInitialize(void* pArg = nullptr) { return S_OK; };
	virtual HRESULT Object_Function(void* pArg = nullptr) { return S_OK; };
	//virtual _int Obsever_On_Trigger(CGameObject* pDestObjects, _float3 fCollision_Distance, _float fDeltaTime) { return S_OK; };

	class CComponent* Get_Component(const _tchar* tagComponent);
	class CComponent* Find_Components(const _tchar* tagComponent);
	HRESULT Change_Component_by_NewAssign(_uint iScenenNum, const _tchar* tagPrototype, const _tchar* tagComponent, void* pArg = nullptr);
	HRESULT Change_Component_by_Parameter(CComponent* NewComponent, const _tchar* tagComponent, void* pArg = nullptr);


protected:
	map<const _tchar*, class CComponent**>	m_mapComponets;
	typedef map<const _tchar*, class CComponent**>	COMPONENTS;

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;
	_float3						m_vOldPos = _float3(0, 0, 0);
	_uint						m_eNowSceneNum = 0;
	const _tchar*				m_szNameTag = nullptr;
	_bool						m_bIsClone = false;
	_bool						m_bIsDead = false;

	_float						m_fRenderSortValue = 0;
	_bool						m_bIsOnScreen = false;

protected:
	HRESULT Add_Component(_uint iScenenNum, const _tchar* tagPrototype,const _tchar* tagComponent, CComponent** ppOut , void* pArg =nullptr);
	
public:
	virtual CGameObject* Clone(void* pArg = nullptr)PURE;
	virtual void Free() override;
};

END