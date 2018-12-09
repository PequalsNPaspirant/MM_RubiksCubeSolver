#include "stdafx.h"

#include <string>
using namespace std;

#include "MessageHandling.h"
#include "Resource.h"

extern VOID DeInit();
extern VOID SizeOpenGlScreen(INT, INT);
extern INT GetSelectedObjects(int x, int y, GLsizei buffSize, GLuint* selectBuffer);
extern VOID GetCubeSelection(int *x, int *y, int *z, Face *face);
extern CVector3 MapCoordinates(int x, int y);
extern LRESULT CALLBACK AboutProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,LPARAM lParam);
extern VOID Scramble(const string& str);
extern VOID Solve();

void OnRubiksCubeChanged(HWND hWnd)
{
	// check for solution
	if (g_cCube.IsSolved())
	{
		TCHAR solvedMsg[MAX_LOADSTRING];
		LoadString(g_hInstance, IDS_SOLVED, solvedMsg, MAX_LOADSTRING);

		//MessageBox(g_hWnd, solvedMsg, g_szTitle, MB_OK);
	}
}

//
//  Process WM_LBUTTONDOWN message for window/dialog: 
//
void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	// set up tracking for when mouse leaves window
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = hWnd;
	TrackMouseEvent(&tme);

	g_bMouseDown = TRUE;

	if ((g_nHitCount = GetSelectedObjects(x, y, SELECT_BUFFER_SIZE, g_pSelectBuffer)) > 0)
		g_vMouseDown = MapCoordinates(x, y);

	g_nPrevX = x;
	g_nPrevY = y;
}

//
//  Process WM_LBUTTONUP message for window/dialog: 
//
void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	g_bMouseDown = FALSE;

	/*
	if (g_bRotating)
	{
		if (abs(g_nRotationAngle) >= 45)
		{
			int turns = 1;

			if (g_bFlipRotation)
				g_nRotationAngle *= -1;

			if (g_nRotationAngle < 0)
				turns = 3;
			else if (g_nRotationAngle > 0)
				turns = 1;

			if (g_vRotationAxis.x)
				g_cCube.Tilt(g_nRotatingSection, turns);
			else if (g_vRotationAxis.y)
				g_cCube.Rotate(g_nRotatingSection, turns);
			else if (g_vRotationAxis.z)
				g_cCube.Turn(g_nRotatingSection, turns);
		}

		g_bRotating = FALSE;		
		g_bFlipRotation = FALSE;

		PostMessage(g_hWnd, RC_CHANGED, 0, 0);
	}
	*/
}

//
//  Process WM_DESTROY message for window/dialog: 
//
void OnDestroy(HWND hWnd)
{
	DeInit();
}

//
//  Process WM_MOUSEMOVE message for window/dialog: 
//
void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
{
	if (!g_bMouseDown)
	{
		/*
		if ((g_nHitCount = GetSelectedObjects(x, y, SELECT_BUFFER_SIZE, g_pSelectBuffer)) > 0)
			SetCursor(g_hHand);
		else
			SetCursor(g_hArrow);
			*/
	}

	// moving camera
	else if (g_nHitCount == 0 && g_bMouseDown)
	{
		if (x < g_nPrevX)
			g_cCamera.Rotate(-5);
		else if (x > g_nPrevX)
			g_cCamera.Rotate(5);

		if (y < g_nPrevY)
			g_cCamera.Tilt(5);
		else if (y > g_nPrevY)
			g_cCamera.Tilt(-5);
	}
	/*
	// rotating section
	else if (g_nHitCount > 0 && g_bMouseDown && !g_bRotating)
	{
		int deltaX = abs(x - g_nPrevX);
		int deltaY = abs(y - g_nPrevY);
		int i, j , k = 0;
		Face face;
		
		GetCubeSelection(&i, &j, &k, &face);

		if (deltaX > 3 || deltaY > 3)
		{
			if (deltaX >= deltaY)
			{
				if (face == Top || face == Bottom)
				{
					float phi = g_cCamera.GetPhi();

					if (phi < 45 || (phi >= 135 && phi <= 225))
					{
						g_vRotationAxis = CVector3(1, 0, 0);
						g_nRotatingSection = i;
						g_vMouseDown.x = 0;		
						
						g_bFlipRotation = !g_bFlipRotation;
					}

					else
					{
						g_vRotationAxis = CVector3(0, 0, 1);
						g_nRotatingSection = k;
						g_vMouseDown.z = 0;		
					}	
				}
				else if (face == Left)
				{
					g_vRotationAxis = CVector3(0, 1, 0);
					g_nRotatingSection = j;
					g_vMouseDown.y = 0;
				}
				else if (face == Right)
				{
					g_vRotationAxis = CVector3(0, 1, 0);
					g_nRotatingSection = j;
					g_vMouseDown.y = 0;
				}
				else if (face == Front)
				{
					g_vRotationAxis = CVector3(0, 1, 0);
					g_nRotatingSection = j;
					g_vMouseDown.y = 0;
				}
				else if (face == Back)
				{
					g_vRotationAxis = CVector3(0, 1, 0);
					g_nRotatingSection = j;
					g_vMouseDown.y = 0;
				}

				if (x - g_nPrevX < 0)
					g_bFlipRotation = !g_bFlipRotation;
			}

			else
			{
				if (face == Top || face == Bottom)
				{
					float phi = g_cCamera.GetPhi();

					if (phi < 45 || (phi >= 135 && phi <= 225))
					{
						
						g_vRotationAxis = CVector3(0, 0, 1);
						g_nRotatingSection = k;
						g_vMouseDown.z = 0;	
					}

					else
					{							
						g_vRotationAxis = CVector3(1, 0, 0);
						g_nRotatingSection = i;
						g_vMouseDown.x = 0;	
					}	
							
					g_bFlipRotation = !g_bFlipRotation;
				}
				else if (face == Left)
				{
					g_vRotationAxis = CVector3(0, 0, 1);
					g_nRotatingSection = k;
					g_vMouseDown.z = 0;
				}
				else if (face == Right)
				{
					g_vRotationAxis = CVector3(0, 0, 1);
					g_nRotatingSection = k;
					g_vMouseDown.z = 0;
					g_bFlipRotation = !g_bFlipRotation;
				}
				else if (face == Front)
				{
					g_vRotationAxis = CVector3(1, 0, 0);
					g_nRotatingSection = i;
					g_vMouseDown.x = 0;
				}
				else if (face == Back)
				{
					g_vRotationAxis = CVector3(1, 0, 0);
					g_nRotatingSection = i;
					g_vMouseDown.x = 0;					
					g_bFlipRotation = !g_bFlipRotation;
				}

				if (y - g_nPrevY < 0)
					g_bFlipRotation = !g_bFlipRotation;
			}

			if (g_cCamera.IsFlipped())
				g_bFlipRotation = !g_bFlipRotation;

			if (g_cCamera.GetPhi() >= 90 && g_cCamera.GetPhi() <= 270 && (face == Top || face == Bottom))
				g_bFlipRotation = !g_bFlipRotation;

			g_bRotating = TRUE;
		}
	}

	if (g_bRotating)
	{
		CVector3 pos = MapCoordinates(x, y);

		if (g_vRotationAxis.x)
			pos.x = 0;
		else if (g_vRotationAxis.y)
			pos.y = 0;
		else if (g_vRotationAxis.z)
			pos.z = 0;

		double angle = g_vMouseDown.GetAngle(pos);

		g_nRotationAngle = (int)(angle * 180 / PI);
		
		if (g_nRotationAngle > 90)
			g_nRotationAngle = 90;
		else if (g_nRotationAngle < -90)
			g_nRotationAngle = -90;
	}
	*/
	g_nPrevX = x;
	g_nPrevY = y;
}

//
//  Process WM_MOUSEWHEEL message for window/dialog: 
//
void OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{
	int rotations = zDelta / WHEEL_DELTA;
	float distance = rotations * 1.0f;

	g_cCamera.Move(distance);
}

//
//  Process WM_SIZE message for window/dialog: 
//
void OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	if (!g_bFullScreen)
	{
		SizeOpenGlScreen(cx, cy);
		GetClientRect(hWnd, &g_rWnd);
	}
}

void OnMouseLeave(HWND hWnd)
{
	g_bMouseDown = FALSE;
	//g_nRotationAngle = 0;
}

string getScramblingAlgo()
{
	char charSet[9] = {'F', 'Z', 'B', 'L', 'X', 'R', 'U', 'Y', 'D'};
	string retVal;
	for (int i = 0; i < 25; ++i)
	{
		int index = rand() % 9;
		retVal += charSet[index];

		if (rand() % 2 == 0)
			retVal += '\'';
		else if (rand() % 10 == 0)
			retVal += '2';
	}

	return retVal;
}

//
//  Process WM_COMMAND message for window/dialog: 
//
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// menu
	if (id == IDM_FILE_SCRAMBLE)
	{
		//wstring str(L"Scramble using these steps? - ");
		//TCHAR newGameMsg[MAX_LOADSTRING];
		//LoadString(g_hInstance, IDS_NEWGAME, newGameMsg, MAX_LOADSTRING);

		string str = getScramblingAlgo();
		wstring wstr(str.begin(), str.end());
		wstr = L"Scramble using these steps? - " + wstr;
		if (MessageBox(hwnd, wstr.c_str(),
			g_szTitle, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
		{
			Scramble(str);
		}
	}
	else if (id == IDM_FILE_SOLVE)
	{
		Solve();
	}

	else if (id == IDM_ABOUT)
	{
		DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), 
			g_hWnd, reinterpret_cast<DLGPROC>(AboutProc));
	}

	else if (id == IDM_EXIT)
	{
		PostQuitMessage(0);
	}
}

