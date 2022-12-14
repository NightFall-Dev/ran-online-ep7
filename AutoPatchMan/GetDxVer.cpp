//-----------------------------------------------------------------------------
// File: GetDXVer.cpp
//
// Desc: Demonstrates how applications can detect what version of DirectX
//       is installed.
//
// (C) Copyright Microsoft Corp.  All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "GetDxVer.h"

#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <windowsx.h>
#include <basetsd.h>
#include <ddraw.h>
#include <dinput.h>
#include <dmusici.h>
#include <tchar.h>


typedef HRESULT(WINAPI * DIRECTDRAWCREATE)( GUID*, LPDIRECTDRAW*, IUnknown* );
typedef HRESULT(WINAPI * DIRECTDRAWCREATEEX)( GUID*, VOID**, REFIID, IUnknown* );
typedef HRESULT(WINAPI * DIRECTINPUTCREATE)( HINSTANCE, DWORD, LPDIRECTINPUT*,
                                             IUnknown* );

BOOL	DxCheckVer()
{
    DWORD  dwDXVersion = GetDXVersion();

    switch( dwDXVersion )
    {
        case 0x000:
            //strResult = TEXT("No DirectX installed" );
            //break;
        case 0x100:
            //strResult = TEXT("DirectX 1 installed" );
            //break;
        case 0x200:
            //strResult = TEXT("DirectX 2 installed" );
            //break;
        case 0x300:
            //strResult = TEXT("DirectX 3 installed" );
            //break;
        case 0x500:
            //strResult = TEXT("DirectX 5 installed" );
            //break;
        case 0x600:
            //strResult = TEXT("DirectX 6 installed" );
            //break;
        case 0x601:
            //strResult = TEXT("DirectX 6.1 installed" );
            //break;
        case 0x700:
            //strResult = TEXT("DirectX 7 installed" );
            //break;
        case 0x800:
            //strResult = TEXT("DirectX 8.0 installed" );
            //break;
			return FALSE;


        case 0x801:
            //strResult = TEXT("DirectX 8.1 installed" );
            //break;
        case 0x900:
            //strResult = TEXT("DirectX 9.0 or better installed" );
            //break;
			return TRUE;

        default:
            //strResult = TEXT("Unknown version of DirectX installed." );
            //break;
			return FALSE;
    }
}

//-----------------------------------------------------------------------------
// Name: GetDXVersion()
// Desc: This function returns the DirectX version number as follows:
//          0x0000 = No DirectX installed
//          0x0100 = DirectX version 1 installed
//          0x0200 = DirectX 2 installed
//          0x0300 = DirectX 3 installed
//          0x0500 = At least DirectX 5 installed.
//          0x0600 = At least DirectX 6 installed.
//          0x0601 = At least DirectX 6.1 installed.
//          0x0700 = At least DirectX 7 installed.
//          0x0800 = At least DirectX 8 installed.
//          0x0801 = At least DirectX 8.1 installed.
//          0x0900 = At least DirectX 9 installed.
// 
//       Please note that this code is intended as a general guideline. Your
//       app will probably be able to simply query for functionality (via
//       QueryInterface) for one or two components.
//
//       Please also note:
//          "if( dwDXVersion != 0x500 ) return FALSE;" is VERY BAD. 
//          "if( dwDXVersion <  0x500 ) return FALSE;" is MUCH BETTER.
//       to ensure your app will run on future releases of DirectX.
//-----------------------------------------------------------------------------
DWORD GetDXVersion()
{
    DIRECTDRAWCREATE     DirectDrawCreate   = NULL;
    DIRECTDRAWCREATEEX   DirectDrawCreateEx = NULL;
    DIRECTINPUTCREATE    DirectInputCreate  = NULL;
    HINSTANCE            hDDrawDLL          = NULL;
    HINSTANCE            hDInputDLL         = NULL;
    HINSTANCE            hD3D8DLL           = NULL;
    HINSTANCE            hD3D9DLL           = NULL;
    LPDIRECTDRAW         pDDraw             = NULL;
    LPDIRECTDRAW2        pDDraw2            = NULL;
    LPDIRECTDRAWSURFACE  pSurf              = NULL;
    LPDIRECTDRAWSURFACE3 pSurf3             = NULL;
    LPDIRECTDRAWSURFACE4 pSurf4             = NULL;
    DWORD                dwDXVersion        = 0;
    HRESULT              hr;

    // First see if DDRAW.DLL even exists.
    hDDrawDLL = LoadLibrary(TEXT("DDRAW.DLL"));
    if( hDDrawDLL == NULL )
    {
        dwDXVersion = 0;
        OutputDebugString(TEXT("Couldn't LoadLibrary DDraw\r\n"));
        return dwDXVersion;
    }

    // See if we can create the DirectDraw object.
    DirectDrawCreate = (DIRECTDRAWCREATE)GetProcAddress( hDDrawDLL, "DirectDrawCreate");
    if( DirectDrawCreate == NULL )
    {
        dwDXVersion = 0;
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't GetProcAddress DirectDrawCreate\r\n"));
        return dwDXVersion;
    }

    hr = DirectDrawCreate( NULL, &pDDraw, NULL );
    if( FAILED(hr) )
    {
        dwDXVersion = 0;
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't create DDraw\r\n"));
        return dwDXVersion;
    }

    // So DirectDraw exists.  We are at least DX1.
    dwDXVersion = 0x100;

    // Let's see if IID_IDirectDraw2 exists.
    hr = pDDraw->QueryInterface( IID_IDirectDraw2, (VOID**)&pDDraw2 );
    if( FAILED(hr) )
    {
        // No IDirectDraw2 exists... must be DX1
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't QI DDraw2\r\n"));
        return dwDXVersion;
    }

    // IDirectDraw2 exists. We must be at least DX2
    pDDraw2->Release();
    dwDXVersion = 0x200;


    //-------------------------------------------------------------------------
    // DirectX 3.0 Checks
    //-------------------------------------------------------------------------

    // DirectInput was added for DX3
    hDInputDLL = LoadLibrary(TEXT("DINPUT.DLL"));
    if( hDInputDLL == NULL )
    {
        // No DInput... must not be DX3
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't LoadLibrary DInput\r\n"));
        return dwDXVersion;
    }

    DirectInputCreate = (DIRECTINPUTCREATE)GetProcAddress( hDInputDLL,
                                                        "DirectInputCreateA");
    if( DirectInputCreate == NULL )
    {
        // No DInput... must be DX2
        FreeLibrary( hDInputDLL );
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't GetProcAddress DInputCreate\r\n"));
        return dwDXVersion;
    }

    // DirectInputCreate exists. We are at least DX3
    dwDXVersion = 0x300;
    FreeLibrary( hDInputDLL );

    // Can do checks for 3a vs 3b here


    //-------------------------------------------------------------------------
    // DirectX 5.0 Checks
    //-------------------------------------------------------------------------

    // We can tell if DX5 is present by checking for the existence of
    // IDirectDrawSurface3. First, we need a surface to QI off of.
    DDSURFACEDESC ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    hr = pDDraw->SetCooperativeLevel( NULL, DDSCL_NORMAL );
    if( FAILED(hr) )
    {
        // Failure. This means DDraw isn't properly installed.
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        dwDXVersion = 0;
        OutputDebugString(TEXT("Couldn't Set coop level\r\n"));
        return dwDXVersion;
    }

    hr = pDDraw->CreateSurface( &ddsd, &pSurf, NULL );
    if( FAILED(hr) )
    {
        // Failure. This means DDraw isn't properly installed.
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        dwDXVersion = 0;
        OutputDebugString(TEXT("Couldn't CreateSurface\r\n"));
        return dwDXVersion;
    }

    // Query for the IDirectDrawSurface3 interface
    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface3,
                                       (VOID**)&pSurf3 ) ) )
    {
        pSurf->Release();
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't QI DDS3\r\n"));
        return dwDXVersion;
    }

    // QI for IDirectDrawSurface3 succeeded. We must be at least DX5
    dwDXVersion = 0x500;
    pSurf3->Release();


    //-------------------------------------------------------------------------
    // DirectX 6.0 Checks
    //-------------------------------------------------------------------------

    // The IDirectDrawSurface4 interface was introduced with DX 6.0
    if( FAILED( pSurf->QueryInterface( IID_IDirectDrawSurface4,
                                       (VOID**)&pSurf4 ) ) )
    {
        pSurf->Release();
        pDDraw->Release();
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't QI DDS4\r\n"));
        return dwDXVersion;
    }

    // IDirectDrawSurface4 was create successfully. We must be at least DX6
    dwDXVersion = 0x600;
    pSurf4->Release();
    pSurf->Release();
    pDDraw->Release();


    //-------------------------------------------------------------------------
    // DirectX 6.1 Checks
    //-------------------------------------------------------------------------

    // Check for DMusic, which was introduced with DX6.1
    LPDIRECTMUSIC pDMusic = NULL;
    CoInitialize( NULL );
    hr = CoCreateInstance( CLSID_DirectMusic, NULL, CLSCTX_INPROC_SERVER,
                           IID_IDirectMusic, (VOID**)&pDMusic );
    if( FAILED(hr) )
    {
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't create CLSID_DirectMusic\r\n"));
        return dwDXVersion;
    }

    // DirectMusic was created successfully. We must be at least DX6.1
    dwDXVersion = 0x601;
    pDMusic->Release();
    CoUninitialize();
    

    //-------------------------------------------------------------------------
    // DirectX 7.0 Checks
    //-------------------------------------------------------------------------

    // Check for DirectX 7 by creating a DDraw7 object
    LPDIRECTDRAW7 pDD7;
    DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress( hDDrawDLL,
                                                       "DirectDrawCreateEx");
    if( NULL == DirectDrawCreateEx )
    {
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't GetProcAddress DirectDrawCreateEx\r\n"));
        return dwDXVersion;
    }

    if( FAILED( DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7,
                                    NULL ) ) )
    {
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't DirectDrawCreateEx\r\n"));
        return dwDXVersion;
    }

    // DDraw7 was created successfully. We must be at least DX7.0
    dwDXVersion = 0x700;
    pDD7->Release();


    //-------------------------------------------------------------------------
    // DirectX 8.0 Checks
    //-------------------------------------------------------------------------

    // Simply see if D3D8.dll exists.
    hD3D8DLL = LoadLibrary(TEXT("D3D8.DLL"));
    if( hD3D8DLL == NULL )
    {
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't LoadLibrary D3D8.DLL\r\n"));
        return dwDXVersion;
    }
    FreeLibrary( hD3D8DLL );

    // D3D8.dll exists. We must be at least DX8.0
    dwDXVersion = 0x800;


    //-------------------------------------------------------------------------
    // DirectX 8.1 Checks
    //-------------------------------------------------------------------------
    TCHAR szVersion[64];
    TCHAR szPath[512];
    if( GetSystemDirectory( szPath, MAX_PATH ) != 0 )
    {
        _tcscat( szPath, TEXT("\\d3d8.dll"));
        if( SUCCEEDED( GetFileVersion( szPath, szVersion, 64 ) ) )
        {
            if( _tcsstr( szVersion, TEXT("4.08.0001")) != NULL || // Win9x version
                _tcsstr( szVersion, TEXT("5.01.2600")) != NULL ) // Win2k/WinXP version
            {                    
                // d3d8.dll is the DX8.1 version, so we must be at least DX8.1
                dwDXVersion = 0x801;
            }
        }
    }
    
    //-------------------------------------------------------------------------
    // DirectX 9.0 Checks
    //-------------------------------------------------------------------------

    // Simply see if D3D9.dll exists.
    hD3D9DLL = LoadLibrary(TEXT("D3D9.DLL"));
    if( hD3D9DLL == NULL )
    {
        FreeLibrary( hDDrawDLL );
        OutputDebugString(TEXT("Couldn't LoadLibrary D3D9.DLL\r\n"));  
        return dwDXVersion;
    }
    FreeLibrary( hD3D9DLL );

    // D3D9.dll exists. We must be at least DX9.0
    dwDXVersion = 0x900;

    
    //-------------------------------------------------------------------------
    // End of checking for versions of DirectX 
    //-------------------------------------------------------------------------

    // Close open libraries and return
    FreeLibrary( hDDrawDLL );
    
    return dwDXVersion;
}




//-----------------------------------------------------------------------------
// Name: GetFileVersion()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT GetFileVersion( TCHAR* szPath, TCHAR* szVersion, DWORD cchVersion )
{   
    if( szPath == NULL || szVersion == NULL || cchVersion < 15 )
        return E_INVALIDARG;

    szVersion[0] = 0;

    DWORD dwHandle;
    UINT  cb;
    cb = GetFileVersionInfoSize( szPath, &dwHandle );
    if (cb > 0)
    {
        BYTE* pFileVersionBuffer = new BYTE[cb];
        if( pFileVersionBuffer == NULL )
            return E_OUTOFMEMORY;

        if (GetFileVersionInfo( szPath, 0, cb, pFileVersionBuffer))
        {
            VS_FIXEDFILEINFO* pVersion = NULL;
            if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) && 
                pVersion != NULL) 
            {
                wsprintf( szVersion, TEXT("%d.%02d.%04d.%04d"), HIWORD(pVersion->dwFileVersionMS), LOWORD(pVersion->dwFileVersionMS),  HIWORD(pVersion->dwFileVersionLS), LOWORD(pVersion->dwFileVersionLS) );
                delete[] pFileVersionBuffer;
                return S_OK;
            }
        }

        delete[] pFileVersionBuffer;
    }

    return E_FAIL;
}
