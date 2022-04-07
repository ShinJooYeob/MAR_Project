#pragma once

#include "Base.h"


BEGIN(Engine)

class CHierarchyNode final :public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	HRESULT Initialize_HierarchyNode(CHierarchyNode* pParent, const char* pName, _float4x4* TransformationMatrix, _uint iDepth);

public:/*GetSet*/
	_uint			Get_Depth() const { return m_iDepth; };
	const char*		Get_Name() const { return m_szName.c_str(); };


	void Set_OffsetMatrix(_float4x4& matOffSet) { memcpy(&m_matOffset, &matOffSet, sizeof(_float4x4)); };

private:
	CHierarchyNode*				m_pParent = nullptr;
	string						m_szName = "";
	_float4x4					m_matOffset;
	_float4x4					m_matTransformation;
	_float4x4					m_matCombinedTransformation;

	_uint						m_iDepth;
public:
	static CHierarchyNode* Create(CHierarchyNode* pParent, const char* pName, _float4x4* TransformationMatrix, _uint iDepth);
	virtual void Free() override;
};

END	