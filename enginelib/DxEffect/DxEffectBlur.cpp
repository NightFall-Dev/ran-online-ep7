// DxEffectBlur.cpp: implementation of the DxEffectBlur class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DxFrameMesh.h"
#include "DxEffectMan.h"

#include "DxEffectBlur.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


const DWORD DxEffectBlur::TYPEID = DEF_EFFECT_BLUR;
const DWORD	DxEffectBlur::VERSION = 0x00000100;
const char DxEffectBlur::NAME[] = "[1001]_[ 接雌反引 (送識) ]";
const DWORD DxEffectBlur::FLAG = NULL;

void DxEffectBlur::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(BLUR_PROPERTY);
	dwVer = VERSION;
}

void DxEffectBlur::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(BLUR_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectBlur::DxEffectBlur() :
	m_fTime(0.0f),
	m_vCurrentColor(0.0f,1.0f,1.0f,1.0f),
	m_vPrevColor(1.0f,0.0f,0.0f,0.0f),
	m_fFudge(10.0f),
	m_fFraction(7.0f)
{
}

DxEffectBlur::~DxEffectBlur()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectBlur::CreateDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr;

	DWORD dwDecl[] =
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
		D3DVSD_REG(3, D3DVSDT_FLOAT3 ),	
		D3DVSD_REG(7, D3DVSDT_FLOAT2 ),	
		D3DVSD_END()
	};

	hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwBlurDVertexShader, &m_dwEffect, 
		DxEffectMan::Instance.GetUseSwShader() );
	if( FAILED(hr) )	return E_FAIL;

//	SAFE_DELETE_ARRAY ( dwBlurDVertexShader );


	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 識情
		//
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		//	Note : SetTextureStageState() 識情
		//
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_dwEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectBlur::ReleaseDevice ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	pd3dDevice->DeleteVertexShader ( m_dwEffect );

	if ( pd3dDevice )
	{
		if ( m_dwSavedStateBlock )
			pd3dDevice->DeleteStateBlock( m_dwSavedStateBlock );
		if ( m_dwEffectStateBlock )
			pd3dDevice->DeleteStateBlock( m_dwEffectStateBlock );
	}

	m_dwSavedStateBlock		= NULL;
	m_dwEffectStateBlock	= NULL;

	return S_OK;
}

HRESULT DxEffectBlur::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICE8 pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectPrev;
	pFrame->pEffectPrev = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		strcpy ( m_szAdaptFrame, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectBlur::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{

	return S_OK;
}

HRESULT DxEffectBlur::DeleteDeviceObjects ()
{

	return S_OK;
}

HRESULT DxEffectBlur::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectBlur::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICE8 pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : Render ( 接雌 )
	//
	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : 五習 益軒奄.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				DWORD dwFVFSize;
				DWORD dwVerts, dwFaces;
				LPDIRECT3DINDEXBUFFER8 pIB	= NULL;
				LPDIRECT3DVERTEXBUFFER8 pVB = NULL;

				//	Note : LocalMesh, OctreeMesh 却 掻 馬蟹研 壱献陥.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{				
					pmsMeshs->m_pLocalMesh->GetIndexBuffer ( &pIB );
					pmsMeshs->m_pLocalMesh->GetVertexBuffer ( &pVB );
					
					dwFVFSize = pmsMeshs->m_pLocalMesh->GetFVF ();
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pLocalMesh->GetNumVertices();
					dwFaces = pmsMeshs->m_pLocalMesh->GetNumFaces();
				}
				else
				{
					dwFVFSize = pmsMeshs->m_pDxOctreeMesh->m_dwFVF;
					dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

					dwVerts = pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
					dwFaces = pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
				}


				//	Note : Shader 雌呪 識情
				//
				float fRadius;
				fRadius = pframeCur->GetTreeXWidth();
				if ( fRadius < pframeCur->GetTreeYWidth() )	fRadius = pframeCur->GetTreeYWidth();
				if ( fRadius < pframeCur->GetTreeZWidth() )	fRadius = pframeCur->GetTreeZWidth();

				float const	fOneOverExtent = m_fFudge/fRadius;
				D3DXVECTOR4 vOneOverExtent ( fOneOverExtent, 1.0f, m_fFraction, 1.0f );
				D3DXVECTOR4	m_vConstants ( 0.0f,1.0f,0.0f,0.0f );

				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+0,		&m_vConstants,		1 );
				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+1,		&vOneOverExtent,	1 );
				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+2,		&m_vPrevColor,		1 );
				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+3,		&m_vCurrentColor,	1 );

				//	Note : 接雌戚 格巷 掩惟 潅嬢 劾 凶 益軒走 省澗陥.
				//
				if( m_fTime > 0.3f )
				{
					m_fTime = 0.0f;
					m_matOldWorld = pframeCur->matCombined;
				}



				D3DXMATRIX	matWorld, matOldWorld;
				D3DXMatrixTranspose( &matWorld,		&(pframeCur->matCombined)		);
				D3DXMatrixTranspose( &matOldWorld,	&(m_matOldWorld)	);

				pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,		&matWorld,		4 );
				pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+4,		&matOldWorld,	4 );


				//	Note : 戚穿 雌殿 拷穣.
				//
				pd3dDevice->CaptureStateBlock ( m_dwSavedStateBlock );

				//	Note : 沓希元 雌殿 繕舛.
				//
				pd3dDevice->ApplyStateBlock ( m_dwEffectStateBlock );



				//	Note : 社覗闘裾嬢 獄努什 覗稽室縮
				//
				if ( DxEffectMan::Instance.GetUseSwShader() )
					pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE );


				//	Note : LocalMesh, OctreeMesh 却 掻 馬蟹研 壱献陥.
				//
				if ( pmsMeshs->m_pLocalMesh )
				{
					pd3dDevice->SetIndices ( pIB, 0 );
					pd3dDevice->SetStreamSource ( 0, pVB, dwFVFSize );
				}
				else
				{
					pd3dDevice->SetIndices ( pmsMeshs->m_pDxOctreeMesh->m_pIB, 0 );
					pd3dDevice->SetStreamSource ( 0, pmsMeshs->m_pDxOctreeMesh->m_pVB, dwFVFSize );
				}

				pd3dDevice->SetVertexShader ( m_dwEffect );

				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, dwVerts, 0, dwFaces );


				//	Note : 社覗闘裾嬢 獄努什 覗稽室縮
				//
				if ( DxEffectMan::Instance.GetUseSwShader() )
					pd3dDevice->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, FALSE );

				pd3dDevice->SetIndices ( NULL, 0 );
				pd3dDevice->SetStreamSource ( 0, NULL, 0 );



				//	Note : 戚穿雌殿稽 差据.
				//
				pd3dDevice->ApplyStateBlock ( m_dwSavedStateBlock );

				if( m_fTime > 0.05f )
				{
					m_fTime = 0.0f;
					m_matOldWorld = pframeCur->matCombined;
				}

				if ( pmsMeshs->m_pLocalMesh )
				{
					if(pIB) pIB->Release ();
					if(pVB) pVB->Release ();
				}
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : 切縦 覗傾績 益軒奄.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = Render ( pframeChild, pd3dDevice );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

/*
//	Note : Effect 益顕切, 神稽虞, 接雌反引 去拭 紫遂馬澗 Mesh
//			旭精 是帖拭 赤澗 繊税 葛菰聖 旭惟 幻級嬢辞 採杷研 滴惟 拝凶 紫遂廃陥.
//
HRESULT DxEffectBlur::MakeEffNormalMeshs ( LPD3DXMESH *pEffectMesh, DxOctreeMesh *pOctreeMesh )
{
	struct VERTEX { D3DXVECTOR3 p, n; };
	VERTEX*					pVertices;
    WORD*					pIndices;
	D3DXVECTOR3				Temp[10000];

	D3DXVECTOR3		Temp1;
	D3DXVECTOR3		Temp2;
	D3DXVECTOR3		Temp3;

	DWORD	dwVerts	= (*pEffectMesh)->GetNumVertices();
	DWORD	dwFaces	= (*pEffectMesh)->GetNumFaces();

	(*pEffectMesh)->LockVertexBuffer( 0L, (BYTE**)&pVertices );
    (*pEffectMesh)->LockIndexBuffer( 0L, (BYTE**)&pIndices );

	for( DWORD i = 0; i< dwVerts; i++)
	{
		// 段奄鉢
		Temp[i] = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );


		for( DWORD j = 0; j< dwFaces; j++)
		{
			WORD wFace0 = pIndices[3*j+0];
			WORD wFace1 = pIndices[3*j+1];
			WORD wFace2 = pIndices[3*j+2];
			
			if( pVertices[i].p == pVertices[wFace0].p )	
			{
				Temp1 = pVertices[wFace1].p - pVertices[wFace0].p;
				Temp2 = pVertices[wFace2].p - pVertices[wFace0].p;
				float a = fabsf(Temp1.x) + fabsf(Temp1.y) + fabsf(Temp1.z);
				float b = fabsf(Temp2.x) + fabsf(Temp2.y) + fabsf(Temp2.z);
				D3DXVec3Cross( &Temp3, &Temp1, &Temp2 );
				Temp[i] += Temp3 * ( a + b );
			}
			else if( pVertices[i].p == pVertices[wFace1].p )	
			{
				Temp1 = pVertices[wFace0].p - pVertices[wFace1].p;
				Temp2 = pVertices[wFace2].p - pVertices[wFace1].p;
				float a = fabsf(Temp1.x) + fabsf(Temp1.y) + fabsf(Temp1.z);
				float b = fabsf(Temp2.x) + fabsf(Temp2.y) + fabsf(Temp2.z);
				D3DXVec3Cross( &Temp3, &Temp2, &Temp1 );
				Temp[i] += Temp3 * ( a + b );
			}
			else if( pVertices[i].p == pVertices[wFace2].p )
			{
				Temp1 = pVertices[wFace1].p - pVertices[wFace2].p;
				Temp2 = pVertices[wFace0].p - pVertices[wFace2].p;
				float a = fabsf(Temp1.x) + fabsf(Temp1.y) + fabsf(Temp1.z);
				float b = fabsf(Temp2.x) + fabsf(Temp2.y) + fabsf(Temp2.z);
				D3DXVec3Cross( &Temp3, &Temp2, &Temp1 );
				Temp[i] += Temp3 * ( a + b );
			}
		}
	}

	for( i = 0; i< dwVerts; i++)
	{
		pVertices[i].n = Temp[i];
	}

	(*pEffectMesh)->UnlockVertexBuffer();
	(*pEffectMesh)->UnlockIndexBuffer();


	return S_OK;
}
*/