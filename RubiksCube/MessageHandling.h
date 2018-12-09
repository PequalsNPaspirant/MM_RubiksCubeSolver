#pragma once

#include "stdafx.h"
#include "Globals.h"

/* void Cls_OnMouseLeave(HWND hWnd, int x, int y, UINT keyFlags) */
#define HANDLE_WM_MOUSELEAVE(hWnd, wParam, lParam, fn) \
    ((fn)((hWnd)), 0L)
#define HANDLE_RC_CHANGED(hWnd, wParam, lParam, fn) \
	((fn)((hWnd)), 0L)

void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
void OnDestroy(HWND hWnd);
void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);
void OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys);
void OnSize(HWND hWnd, UINT state, int cx, int cy);
void OnMouseLeave(HWND hWnd);
void OnRubiksCubeChanged(HWND hWnd);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

