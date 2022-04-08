#pragma once


#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix DefaultPivotMatrix);
	virtual HRESULT Initialize_Clone(void* pArg)override;


public:
	_uint	Get_NumMaterial() { return m_iNumMaterials; };
	HRESULT Bind_OnShader(class CShader* pShader, _uint iMaterialIndex , _uint eTextureType, const char* pHlslConstValueName );
	HRESULT Render(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex);


private:
	const aiScene*				m_pScene = nullptr;
	Importer					m_Importer;

private: /*매시 보관(영항 받는 머테리얼 기준으로 분류하여 저장)*/
	_uint									m_iNumMeshContainers = 0;
	vector<class CMeshContainer*>*			m_vecMeshContainerArr;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;

private:/*머테리얼에 필요한 텍스처들을 모아서 보관*/
	_uint									m_iNumMaterials = 0;
	MESHMATERIALDESC						m_MeshMaterialDesc;

private:/*뼈들의 상속관계 순서대로 정렬해 보관*/
	vector<class CHierarchyNode*>			m_vecHierarchyNode;
	typedef vector<class CHierarchyNode*>	HIERARCHYNODES;

private:
	_uint									m_iCurrentAnimIndex = 0;
	_uint									m_iNumAnimationClip = 0;
	vector<class CAnimationClip*>			m_vecAnimator;
	typedef vector<class CAnimationClip*>	ANIMATOR;


private:
	MODELTYPE								m_eModelType = TYPE_END;
	_float4x4								m_DefaultPivotMatrix;

private:
	HRESULT Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT Ready_OffsetMatrices();			//뼈대의 로컬 좌표와 매쉬의 로컬좌표가 다른데 이걸 보정해주는 행렬을 계층뼈에 저장

	HRESULT Ready_MeshContainers(_fMatrix TransformMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Animation();

private:
	CHierarchyNode* Find_HierarchyNode(const char* pName);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fMatrix TransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END