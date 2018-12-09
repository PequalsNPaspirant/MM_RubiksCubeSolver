#pragma once

#include "stdafx.h"
#include <windows.h>
#include <gl\gl.h>
#include "Camera.h"
#include "Vector3.h"
#include "RubiksCubeModel_v1.h"

#define MAX_LOADSTRING 100
#define RC_CHANGED WM_APP + 100

const GLsizei SELECT_BUFFER_SIZE = 128;

extern BOOL g_bMouseDown;
extern CCamera g_cCamera;
extern CRubiksCube g_cCube;
extern int g_nPrevX;
extern int g_nPrevY;
extern BOOL g_bRotating;
extern CVector3 g_vRotationAxis;
extern int g_nRotatingSection;
extern int g_nRotationAngle;
extern BOOL g_bFullScreen;
extern RECT g_rWnd;
extern CVector3 g_vMouseDown;
extern int g_nHitCount;
extern GLuint g_pSelectBuffer[SELECT_BUFFER_SIZE];
extern HCURSOR g_hArrow;
extern HCURSOR g_hHand;
extern BOOL g_bFlipRotation;
extern TCHAR g_szTitle[MAX_LOADSTRING];					// The title bar text
extern HWND g_hWnd;
extern HINSTANCE g_hInstance;								// current instance

extern CColor RED;
extern CColor ORANGE;

extern CColor BLUE;
extern CColor GREEN;

extern CColor YELLOW;
extern CColor WHITE;

extern CColor BLACK;