#pragma once

namespace Engine
{
	typedef struct tag3DSoundDesc
	{
		class CTransform*		pTransform = nullptr;
		_float3					vPosition;
		_float2					vMinMax;
		_float					fTargetSound = 1.f;
		_bool					bStopSoundNow = false;
		_bool					bFollowTransform = true;
		_uint					iIdentificationNumber = 0;
		
	}SOUNDDESC;


	typedef struct tagVertex_Point
	{
		XMFLOAT3			vPosition;
		float				fPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Point_Instance_Matrix_Declaration
	{
		static const unsigned int iNumElements = 8;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT_INSTMAT_DECLARATION;

	typedef struct tagVertex_Instance_Matrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;

		XMFLOAT4			vUV_WHSize;
		XMFLOAT4			vColor;
	}VTXINSTMAT;



	typedef struct tagAttachBoneMatrixPtr
	{
		_float4x4*			pUpdatedNodeMat;
		_float4x4*			pDefaultPivotMat;
	}ATTACHBONEMATRIX_PTR;


	typedef struct tagColliderDesc
	{
		_float3 vScale; /* 배수 */
		_float4 vRotation;
		_float4 vPosition;
	} COLLIDERDESC;

	typedef struct tagKeyFrame
	{
		_float3		vScale;
		_float4		vRotation;
		_float3		vPosition;
		_double		Time;
	}KEYFRAME;


	typedef struct tagMeshMaterialDesc
	{
		class CTexture*	pTexture;
	}MESHMATERIALDESC;



	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE		eLightType = TYPE_END;
		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;
		//Directional : 방향 / Point : 위치
		_float4		vVector;
		_float		fRange;
	}LIGHTDESC;

	typedef struct tagVertex_Model
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertex_Model_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMODEL_DECLARATION;


	typedef struct tagVertex_Animation_Model
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
		XMUINT4				vBlendIndex;
		XMFLOAT4			vBlendWeight;
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertex_AnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;


	typedef struct tagVertex_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertex_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXTEX_DECLARATION;



	typedef struct tagVertex_CubeTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexUV;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertex_Cube_Texture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;




	typedef struct tagVertex_Normal_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Normal_Texture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORMTEX_DECLARATION;


	typedef struct tagVertex_Normal_Cube_Texture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT3			vTexUV;
	}VTXNORCUBTEX;

	typedef struct ENGINE_DLL tagVertex_Normal_Cube_Texture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORMCUBETEX_DECLARATION;



	typedef struct tagLineIndices32
	{
		unsigned long	_0, _1;
	}LINEINDICES32;


	typedef struct tagLineIndices16
	{
		unsigned short	_0, _1;
	}LINEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long	_0, _1, _2;
	}FACEINDICES32;


	typedef struct tagFaceIndices16
	{
		unsigned short	_0, _1, _2;
	}FACEINDICES16;



	typedef struct tagWorldViewProjMatirx
	{
		_float4x4		WorldMatrix;
		_float4x4		ViewMatrix;
		_float4x4		ProjMatrix;
	}MATRIXWVP;



	/*
		// 저장 불러오기 관련 데이터 타입
		// 텍스처 오브젝트 / 스테이트 / 경로를 저장하는 구조체
		typedef struct tagTexturePath
		{
			wstring			wstrObjKey = L"";
			wstring			wstrStateKey = L"";
			wstring			wstrPath = L"";
			int				iCount = 0;

		}IMGPATH;

		// 텍스처의 위치정보를 저장하는 구조체
		typedef struct tagMyTexturePath
		{
			// 파일구분 1
			wstring			wFileName = L"";
			// 전체 상대 경로
			wstring			wstrFullPath = L"";
			// 텍스처 개수
			int				iCount = 0;

		}MYFILEPATH;

		// 오브젝트에 대한 정보
		// 오브젝트 월드 행렬 / 이름 / 텍스처경로 / 키
		enum E_OUTPUTID { OUTPUT_NONE, OUTPUT_OBJECT, OUTPUT_MAP, OUTPUT_END };
	#define  STR_MAX 64

		typedef struct tagOutputFileidDESC
		{
			explicit tagOutputFileidDESC(E_OUTPUTID id, const _tchar* str)
			{
				FILEID = id;
				lstrcpy(strObjectName, str);

			}
			explicit tagOutputFileidDESC()
			{
				FILEID = OUTPUT_OBJECT;
				lstrcpy(strObjectName, L"");

			}

			explicit tagOutputFileidDESC(const tagOutputFileidDESC& rhs)
			{
				FILEID = rhs.FILEID;
				lstrcpy(strObjectName, rhs.strObjectName);
			}


			tagOutputFileidDESC&  operator=(const tagOutputFileidDESC& rhs)
			{
				FILEID = rhs.FILEID;
				lstrcpy(strObjectName, rhs.strObjectName);
				return *this;
			}

			E_OUTPUTID		FILEID = OUTPUT_NONE;
			TCHAR			strObjectName[STR_MAX];	// 파일이름 / 객체 이름

		}OUTPUT_ID;

		typedef struct tagOutputTetureDESC
		{
			explicit tagOutputTetureDESC()
			{
				eTextureType = 1;
				lstrcpy(szTextFilePath, L"");
				lstrcpy(szStateKey, L"");
				StateIndex = 0;
			}
			explicit tagOutputTetureDESC(const tagOutputTetureDESC& rhs)
			{
				eTextureType = rhs.eTextureType;
				StateIndex = rhs.StateIndex;
				lstrcpy(szTextFilePath, rhs.szTextFilePath);
				lstrcpy(szStateKey, rhs.szStateKey);
			}
			_uint				eTextureType;
			TCHAR				szTextFilePath[STR_MAX];	// 텍스처 경로 / txt파일
			TCHAR				szStateKey[STR_MAX];		// 텍스처 스테이트키
			unsigned int		StateIndex;					// 텍스처 번호


		}OUTPUT_TETURE;

		typedef struct tagOutputObject
		{
			explicit tagOutputObject()
			{
				CubeID = 0;
				D3DXMatrixIdentity(&WorldMatData);
				TexDesc = tagOutputTetureDESC();
			}
			explicit tagOutputObject(const tagOutputObject& rhs)
			{
				CubeID = rhs.CubeID;
				WorldMatData = rhs.WorldMatData;
				TexDesc = tagOutputTetureDESC(rhs.TexDesc);
			}


			tagOutputObject&  operator=(const tagOutputObject& rhs)
			{
				CubeID = rhs.CubeID;
				WorldMatData = rhs.WorldMatData;
				TexDesc = tagOutputTetureDESC(rhs.TexDesc);
				return *this;
			}

			unsigned int		CubeID;					// 어떤 종류의 큐브인지 판단
			OUTPUT_TETURE		TexDesc;				// 텍스처 정보
			D3DXMATRIX			WorldMatData;			// 월드 행렬 정보 //

		}OUTPUT_OBJECTINFO;


		typedef struct tag_Sphere
		{
			tag_Sphere()
			{
				mRadius = 0; mCenterPosition = _float2(0, 0);
			}

			_float2 mCenterPosition; // 중점
			_float mRadius; // 범위
		}MYSPHERE;*/
}