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
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const char * pModelFilePath, const char * pModelFileName, _fMatrix DefaultPivotMatrix, _uint iAnimCount);
	virtual HRESULT Initialize_Clone(void* pArg)override;


public:
	HRESULT Change_AnimIndex(_uint iAnimIndex, _double ExitTime = 0.15, _bool bBlockAnimUntilReturnChange = false);
	HRESULT Change_AnimIndex_ReturnTo(_uint iAnimIndex, _uint iReturnIndex, _double ExitTime = 0.15, _bool bBlockAnimUntilReturnChange = false);
	HRESULT Change_AnimIndex_ReturnTo_Must(_uint iAnimIndex, _uint iReturnIndex, _double ExitTime = 0.15, _bool bBlockAnimUntilReturnChange = false);

	HRESULT Change_AnimIndex_UntilNReturn(_uint iAnimIndex, _uint iUntilIndex, _uint iReturnIndex,_double ExitTime = 0.15, _bool bBlockAnimUntilReturnChange = false);
	HRESULT Change_AnimIndex_UntilNReturn_Must(_uint iAnimIndex, _uint iUntilIndex, _uint iReturnIndex, _double ExitTime = 0.15, _bool bBlockAnimUntilReturnChange = false);
	HRESULT Change_AnimIndex_UntilTo(_uint iAnimIndex, _uint iReturnIndex, _double ExitTime = 0.15, _bool bBlockAnimUntilReturnChange = false);
	_uint	Get_NowAnimIndex() { return m_iNowAnimIndex; };
	_uint	Get_IsHavetoBlockAnimChange() { return m_bIsBlockAnim; };
	_double	Get_PlayRate();
	_bool	Get_IsUntillPlay() { return m_bIsUntill; };
	_bool	Get_IsAnimChanging() { return m_bIsChagingAnim; }
	void	Set_BlockAnim(_bool IsBlock) { m_bIsBlockAnim = IsBlock; };
	void    Set_NextAnim_Must(_uint iAnim) { m_iNextAnimIndex = iAnim; };
	HRESULT	Remove_CertainKeyFrameIndex(_uint iAnimIndex);
	void	Add_Time_To_NowAnimPlayAcc(_double Time) { m_NowPlayTimeAcc += Time; }

	HRESULT Bind_OnShader(class CShader* pShader, _uint iMaterialIndex , _uint eTextureType, const char* pHlslConstValueName);
	HRESULT Update_AnimationClip(_double fDeltaTime, _bool IsUpdateAll = true);	
	HRESULT Render(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* szBoneValueName = nullptr);

	ATTACHBONEMATRIX_PTR Find_AttachMatrix_InHirarchyNode(const char* pName);
public:
	_uint	Get_NumMaterial() { return m_iNumMaterials; };
private:
	const aiScene*				m_pScene = nullptr;
	Importer					m_Importer;

private: /*???? ????(???? ???? ???????? ???????? ???????? ????)*/
	_uint									m_iNumMeshContainers = 0;
	vector<class CMeshContainer*>*			m_vecMeshContainerArr;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;

private:/*?????????? ?????? ?????????? ?????? ????*/
	_uint									m_iNumMaterials = 0;
	MESHMATERIALDESC						m_MeshMaterialDesc;

private:/*?????? ???????? ???????? ?????? ????*/
	vector<class CHierarchyNode*>			m_vecHierarchyNode;
	typedef vector<class CHierarchyNode*>	HIERARCHYNODES;

private:
	_uint									m_iNumAnimationClip = 0;
	vector<class CAnimationClip*>			m_vecAnimator;
	typedef vector<class CAnimationClip*>	ANIMATOR;

	//i???? ???????????? ???????? j???? ???????? ???? ?????????? ????
	vector<vector<_uint>>					m_vecCurrentKeyFrameIndices;
	//???? ???????????? ?????????? ???? ????
	_uint		m_iReturnIndex = 0;
	_uint		m_iNextAnimIndex = 0;
	_uint		m_iNowAnimIndex = 0;
	_uint		m_iOldAnimIndex = 0;
	_double		m_NowPlayTimeAcc = 0.0;
	_double		m_OldPlayTimeAcc = 0.0;
	//?????? ?????? ???????????? ????
	_double		m_TotalAnimExitTime = 0.2;
	_double		m_AnimExitAcc = 0.0;

	_bool		m_bIsUntill = false;
	_bool		m_bIsBlockAnim = false;
	_bool		m_bIsChagingAnim = false;
	_uint		m_KindsOfAnimChange = 0;
	_bool		m_bIsSwapFunctionCalled= false;

private:
	MODELTYPE								m_eModelType = TYPE_END;
	_float4x4								m_DefaultPivotMatrix;

private:
	HRESULT Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT Ready_OffsetMatrices();			//?????? ???? ?????? ?????? ?????????? ?????? ???? ?????????? ?????? ???????? ????

	HRESULT Ready_MeshContainers(_fMatrix TransformMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Animation();
	HRESULT Ready_MoreAnimation(const char* szFileFullPath, _uint iAnimCount, _uint iFlag = 0);

private:
	CHierarchyNode* Find_HierarchyNode(const char* pName, _uint* pNodeIndex = nullptr);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		MODELTYPE eModelType, const char* pModelFilePath, const char* pModelFileName,_fMatrix TransformMatrix = XMMatrixIdentity(), _uint iAnimCount = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END