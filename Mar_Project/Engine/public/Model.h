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
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix TransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg)override;


public:
	HRESULT Render(class CShader* pShader, _uint iPassIndex);


private:
	const aiScene*				m_pScene = nullptr;
	Importer					m_Importer;

private:
	_uint									m_iNumMeshContainers = 0;
	vector<class CMeshContainer*>			m_vecMeshContainer;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;

private:
	_uint									m_iNumMaterials = 0;
	//vector<MESHMATERIALDESC*>				m_vecMaterials;
	//typedef vector<MESHMATERIALDESC*>		MATERIALS;

	MESHMATERIALDESC						m_MeshMaterialDesc;


private:
	MODELTYPE				m_eModelType = TYPE_END;
	_float4x4								m_TransformMatrix;

private:
	HRESULT Ready_MeshContainers(_fMatrix TransformMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName, _fMatrix TransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END