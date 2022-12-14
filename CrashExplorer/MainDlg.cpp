/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2007 IntelleSoft.
 * All rights reserved.
 *
 * Description: Main dialog.
 * Author: Maksim Pyatkovskiy.
 *
 * This source code is only intended as a supplement to the
 * BugTrap package reference and related electronic documentation
 * provided with the product. See these sources for detailed
 * information regarding the BugTrap package.
 */

#include "stdafx.h"
#include "resource.h"

#include "AboutDlg.h"
#include "MainDlg.h"
#include "ManualModeDlg.h"
#include "ExpressModeDlg.h"
#include "ThemeXP.h"
#include "MessageTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDM_ABOUTBOX 0x0010

CMainDlg::CTabItemData CMainDlg::m_arrTabs[] =
{
	{ RUNTIME_CLASS(CExpressModeDlg) },
	{ RUNTIME_CLASS(CManualModeDlg) }
};

/**
 * @param pMsg - message structure.
 * @return true if message was handler and false otherwise.
 */
BOOL CMainDlg::PreTranslateMessage(PMSG pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		UINT uMsg = pMsg->message;
		WPARAM wParam = pMsg->wParam;
		LPARAM lParam = pMsg->lParam;
		LRESULT lResult;
		MSG_WM_KEYDOWN(OnKeyDown)
	}
	return IsDialogMessage(pMsg);
}

/**
 * @param hWnd - handle to the control to receive the default keyboard focus.
 * @param lParam - specifies additional initialization data.
 * @return message result code.
 */
LRESULT CMainDlg::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	// Center the dialog on the screen.
	CenterWindow();
	CThemeXP::LoadThemeDll();
	CThemeXP::EnableThemeDialogTexture(m_hWnd, ETDT_ENABLETAB);
	CThemeXP::UnloadThemeDll();

	// Set the icon for this dialog.
	HINSTANCE hInstance = _Module.GetResourceInstance();
	int nCXIcon = GetSystemMetrics(SM_CXICON);
	int nCYIcon = GetSystemMetrics(SM_CYICON);
	HICON hBigIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, nCXIcon, nCYIcon, LR_DEFAULTCOLOR | LR_SHARED);
	SetIcon(hBigIcon, TRUE);
	int nCXSmallIcon = GetSystemMetrics(SM_CXSMICON);
	int nCYSmallIcon = GetSystemMetrics(SM_CYSMICON);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, nCXSmallIcon, nCYSmallIcon, LR_DEFAULTCOLOR | LR_SHARED);
	SetIcon(hSmallIcon, FALSE);

	// Register object for message filtering and idle updates.
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	// IDM_ABOUTBOX must be in the system command range.
	ATLASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ATLASSERT(IDM_ABOUTBOX < 0xF000);

	// Add "About..." menu item to system menu.
	HMENU hSysMenu = GetSystemMenu(FALSE);
	CMenuHandle SystemMenu(hSysMenu);
	if (hSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
        SystemMenu.AppendMenu(MF_SEPARATOR);
        SystemMenu.AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	m_nSelTabItem = -1;
	m_tcModes.Attach(GetDlgItem(IDC_MODES));
	CString strTitle;
	TCITEM tci;
	ZeroMemory(&tci, sizeof(tci));
	tci.mask = TCIF_TEXT;
	for (int i = 0; i < countof(m_arrTabs); ++i)
	{
		CBaseTabItem* pTabItem = STATIC_DOWNCAST(CBaseTabItem, m_arrTabs[i].m_pRuntimeClass->CreateObject());
		if (pTabItem == NULL)
		{
			DestroyWindow();
			return FALSE;
		}
		pTabItem->GetItemTitle(strTitle);
		m_arrTabs[i].m_pItem = pTabItem;
		tci.pszText = (PTSTR)(PCTSTR)strTitle;
		m_tcModes.InsertItem(i, &tci);
	}

	// select first item
	OnSelChange(NULL);

	return TRUE;
}

/**
 * @param uType - specifies the type of resizing requested.
 * @param size - specifies the new size of client area.
 */
void CMainDlg::OnSize(UINT uType, CSize size)
{
	if (uType != SIZE_MINIMIZED)
	{
		HDWP hdwp = BeginDeferWindowPos(1 + countof(m_arrTabs));
		if (hdwp)
		{
			CRect rcTab(0, 0, size.cx, size.cy);
			m_tcModes.AdjustRect(FALSE, &rcTab);
			m_tcModes.DeferWindowPos(hdwp, NULL, 0, 0, size.cx, size.cy, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			for (int i = 0; i < countof(m_arrTabs); ++i)
			{
				CBaseTabItem* pTabItem = m_arrTabs[i].m_pItem;
				if (pTabItem != NULL && pTabItem->m_hWnd != NULL)
					pTabItem->DeferWindowPos(hdwp, NULL, rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			}
			EndDeferWindowPos(hdwp);
		}
	}
}

/**
 * @param chCode - specifies the virtual key code of the given key.
 * @param uRepCount - the number of times the keystroke is repeated as a result of the user holding down the key.
 * @param uFlags - specifies the scan code, key-transition code, previous key state, and context code.
 */
void CMainDlg::OnKeyDown(TCHAR chCode, UINT /*uRepCount*/, UINT /*uFlags*/)
{
	if (chCode == VK_TAB && GetKeyState(VK_CONTROL) < 0)
	{
		int nItemCount = m_tcModes.GetItemCount();
		int nSelItemPos = m_nSelTabItem;
		if (GetKeyState(VK_SHIFT) < 0)
		{
			if (nSelItemPos >= 0)
				nSelItemPos = (nSelItemPos + nItemCount - 1) % nItemCount;
			else if (nItemCount > 0)
				nSelItemPos = nItemCount - 1;
			else
				return;
		}
		else
		{
			if (nSelItemPos >= 0)
				nSelItemPos = (nSelItemPos + 1) % nItemCount;
			else if (nItemCount > 0)
				nSelItemPos = 0;
			else
				return;
		}
		m_tcModes.SetCurSel(nSelItemPos);
		OnSelChange(NULL);
	}
	else
		SetMsgHandled(FALSE);
}

void CMainDlg::OnDestroy()
{
	for (int i = 0; i < countof(m_arrTabs); ++i)
	{
		CBaseTabItem* pTabItem = m_arrTabs[i].m_pItem;
		if (pTabItem != NULL)
		{
			if (pTabItem->m_hWnd != NULL)
				pTabItem->DestroyWindow();
			delete pTabItem;
			m_arrTabs[i].m_pItem = NULL;
		}
	}
}

/**
 * @param pnmh - pointer to an NMHDR structure.
 * @return unused.
 */
LRESULT CMainDlg::OnSelChange(LPNMHDR /*pnmh*/)
{
	CBaseTabItem* pTabItem;
	bool bFirstTime;
	if (m_nSelTabItem >= 0)
	{
		MsgTip::HideMessage();
		pTabItem = m_arrTabs[m_nSelTabItem].m_pItem;
		if (pTabItem != NULL && pTabItem->m_hWnd != NULL)
			pTabItem->ShowWindow(SW_HIDE);
		bFirstTime = false;
	}
	else
		bFirstTime = true;
	int nSelItemPos = m_tcModes.GetCurSel();
	if (nSelItemPos < 0)
		return 0;
	m_nSelTabItem = nSelItemPos;
	pTabItem = m_arrTabs[nSelItemPos].m_pItem;
	if (pTabItem != NULL)
	{
		if (pTabItem->m_hWnd == NULL)
		{
			if (! pTabItem->CreateItem(m_hWnd))
			{
				DestroyWindow();
				return 0;
			}
			CRect rcTab;
			if (bFirstTime)
			{
				pTabItem->GetClientRect(&rcTab);
				m_tcModes.AdjustRect(TRUE, &rcTab);
				int nWidth = 2 * GetSystemMetrics(SM_CXFRAME) + rcTab.Width();
				int nHeight = GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYFRAME) + rcTab.Height();
				SetWindowPos(NULL, 0, 0, nWidth, nHeight, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE);
			}
			GetClientRect(&rcTab);
			m_tcModes.AdjustRect(FALSE, &rcTab);
			pTabItem->SetWindowPos(HWND_TOP, &rcTab, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
		}
		ATLASSERT(pTabItem->m_hWnd != NULL);
		pTabItem->ShowWindow(SW_SHOW);
		pTabItem->SetFocus();
	}
	return 0;
}

/**
 * @param nVal - specifies an application exit code.
 */
void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

/**
 * @param uNotifyCode - notification code if the message is from a control. If the message is from an accelerator, this value is 1. If the message is from a menu, this value is zero.
 * @param nID - specifies the identifier of the menu item, control, or accelerator.
 * @param hWndCtl - handle to the control sending the message if the message is from a control. Otherwise, this parameter is NULL.
 */
void CMainDlg::OnCloseCmd(UINT /*uNotifyCode*/, int nID, HWND /*hWndCtl*/)
{
	CloseDialog(nID);
}

/**
 * @param uID - specifies the type of system command requested.
 * @param point - specifies the horizontal position of the cursor, in screen coordinates.
 */
void CMainDlg::OnSysCommand(UINT uID, CPoint /*point*/)
{
	if ((uID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal(m_hWnd);
	}
	else
		SetMsgHandled(FALSE);
}
