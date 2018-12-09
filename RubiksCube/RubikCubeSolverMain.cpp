// RubiksCube.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "Camera.h"
#include "RubiksCubeModel_v1.h"
#include "Vector3.h"
#include "Globals.h"
#include "MessageHandling.h"

#include "RubiksCubeSimulator.h"
#include "RubiksCubeSolver_v1.h"

const int WND_WIDTH = 400;
const int WND_HEIGHT = 400;
const int SCREEN_DEPTH = 16; 
const double CUBE_SIZE = 2.0;
const float LINE_WIDTH = 2.0f;

// Global Variables:
HGLRC g_hRC;
HDC g_hDC;
TCHAR g_szWindowClass[MAX_LOADSTRING];				// the main window class name
GLuint g_pTextures[7];
HACCEL g_hAccelTable;

HWND CreateWnd(LPTSTR, INT, INT, DWORD, BOOL, HINSTANCE);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID InitOpenGl(HWND);
BOOL ChangeToFullScreen();
BOOL SetupPixelFormat(HDC);
VOID SizeOpenGlScreen(INT, INT);
WPARAM MainLoop();
VOID RenderScene();
VOID RenderRubiksCube();
VOID RenderCube(CCubeState* pCube, int x, int y, int z);
VOID SetFrustum(int nWidth, int nHeight);
VOID LoadTexture(int nId, GLuint* texture);
VOID Initialize();
VOID Scramble(const string& str);
VOID Solve();
GLuint GetTextureID(CColor color);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadString(NULL, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadString(NULL, IDC_RUBIKSCUBE, g_szWindowClass, MAX_LOADSTRING);

	g_hInstance = hInstance;
	g_hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_RUBIKSCUBE));

	g_hWnd = CreateWnd(g_szTitle, WND_WIDTH, WND_HEIGHT, 0, g_bFullScreen, g_hInstance);

	InitOpenGl(g_hWnd);

	//NewGame();
	Initialize();

	return (int)MainLoop();
}

VOID InitOpenGl(HWND hWnd)
{
	GetClientRect(hWnd, &g_rWnd);
	g_hDC = GetDC(hWnd);

	if (!SetupPixelFormat(g_hDC))
		PostQuitMessage(-1);

	g_hRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hRC);

	SizeOpenGlScreen(g_rWnd.right, g_rWnd.bottom);

	glPolygonMode(GL_FRONT, GL_FILL);
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glClearDepth(1.0f);											// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
	glLineWidth(LINE_WIDTH);									// Set outline width

	LoadTexture(IDB_WHITE, &g_pTextures[0]);
	LoadTexture(IDB_BLUE, &g_pTextures[1]);
	LoadTexture(IDB_ORANGE, &g_pTextures[2]);
	LoadTexture(IDB_RED, &g_pTextures[3]);
	LoadTexture(IDB_GREEN, &g_pTextures[4]);
	LoadTexture(IDB_YELLOW, &g_pTextures[5]);
	LoadTexture(IDB_BLACK, &g_pTextures[6]);
}

BOOL ChangeToFullScreen()
{
	DEVMODE dmSettings;									// Device Mode variable

	memset(&dmSettings,0,sizeof(dmSettings));			// Makes Sure Memory's Cleared

	// Get current settings -- This function fills our the settings
	// This makes sure NT and Win98 machines change correctly
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
		return FALSE;

	// selected screen width and height
	dmSettings.dmPelsWidth	= WND_WIDTH;
	dmSettings.dmPelsHeight	= WND_HEIGHT;
	
	// This function actually changes the screen to full screen
	// CDS_FULLSCREEN Gets Rid Of Start Bar.
	// We always want to get a result from this function to check if we failed
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	// Check if we didn't recieved a good return message From the function
	if(result != DISP_CHANGE_SUCCESSFUL)
		return FALSE;

	return TRUE;
}

BOOL SetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure
    pfd.nVersion = 1;									// Always set this to 1
														// Pass in the appropriate OpenGL flags
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask (this is ignored anyway)
    pfd.iPixelType = PFD_TYPE_RGBA;						// We want RGB and Alpha pixel type
    pfd.cColorBits = SCREEN_DEPTH;						// Here we use our #define for the color bits
    pfd.cDepthBits = SCREEN_DEPTH;						// Depthbits is ignored for RGBA, but we do it anyway
    pfd.cAccumBits = 0;									// No special bitplanes needed
    pfd.cStencilBits = 0;								// We desire no stencil bits
 
	// This gets us a pixel format that best matches the one passed in from the device
    if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
		return FALSE;
 
	// This sets the pixel format that we extracted from above
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
		return FALSE;
 
    return TRUE;
}

VOID SizeOpenGlScreen(int nWidth, int nHeight)
{
	if (nHeight == 0)
	{
		nHeight = 1;
	}

	glViewport(0, 0, nWidth, nHeight);
	glMatrixMode(GL_PROJECTION);
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SetFrustum(nWidth, nHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

VOID SetFrustum(int nWidth, int nHeight)
{
	GLdouble left, right;
	GLdouble top, bottom;

	if (nWidth < nHeight)
	{
		left = -1.0;
		right = 1.0;
		top = (double)nHeight / (double)nWidth;
		bottom = -top;

	}
	else
	{
		top = 1.0;
		bottom = -1.0;
		right = (double)nWidth / (double)nHeight;
		left = -right;
	}

	glFrustum(left, right, bottom, top, 5.0, 100.0);
}

VOID LoadTexture(int nId, GLuint* texture)
{
	// bitmap handle
	HBITMAP hBMP;

	// bitmap struct
	BITMAP   bmp;

	glGenTextures(1, texture);    // Create The Texture 
	hBMP = (HBITMAP)LoadImage(
		GetModuleHandle(NULL), 
		MAKEINTRESOURCE(nId), 
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION); 

	GetObject(hBMP, sizeof(bmp), &bmp); 

	// Pixel Storage Mode (Word Alignment / 4 Bytes) 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); 

	// bind to the texture ID
	glBindTexture(GL_TEXTURE_2D, *texture); 
		
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		3,
		bmp.bmWidth, bmp.bmHeight,
		0, 
		GL_BGR_EXT,
		GL_UNSIGNED_BYTE,
		bmp.bmBits
	); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
	DeleteObject(hBMP);
}

HWND CreateWnd(LPTSTR strTitle, int nWidth, int nHeight, DWORD dwStyle, 
			   BOOL bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASSEX wndClass;

	g_hArrow = LoadCursor(NULL, IDC_ARROW);
	g_hHand = LoadCursor(NULL, IDC_HAND);

	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUBIKSCUBE));
	wndClass.hCursor = g_hArrow;
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszClassName = g_szWindowClass;
	wndClass.cbClsExtra = 0;
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDC_RUBIKSCUBE);
	wndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUBIKSCUBE));

	RegisterClassEx(&wndClass);

	if (bFullScreen && !dwStyle)
	{
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();
	}
	else if (!dwStyle)
	{
		//dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MAXIMIZE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	g_hInstance = hInstance;

	RECT rWnd;
	rWnd.left = 0;
	rWnd.right = nWidth;
	rWnd.top = 0;
	rWnd.bottom = nHeight;

	AdjustWindowRectEx(&rWnd, dwStyle, TRUE, 0);

	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_RUBIKSCUBE));

	hWnd = CreateWindowEx(0, 
		g_szWindowClass, 
		g_szTitle, 
		dwStyle, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		rWnd.right - rWnd.left, 
		rWnd.bottom - rWnd.top, 
		NULL, hMenu, hInstance, NULL);

	if (!hWnd) return NULL;

	//ShowWindow(hWnd, SW_SHOWNORMAL);
	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);

	SetFocus(hWnd);

	return hWnd;
}

VOID DeInit()
{
	if (g_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
	}

	if (g_hDC)
		ReleaseDC(g_hWnd, g_hDC);

	if (g_bFullScreen)
		ChangeDisplaySettings(NULL, 0);

	UnregisterClass(g_szWindowClass, g_hInstance);
	PostQuitMessage(0);
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, RC_CHANGED, OnRubiksCubeChanged);
		//HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_MOUSELEAVE, OnMouseLeave);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, OnMouseWheel);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK AboutProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			EndDialog(hwndDlg, 0);
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}

WPARAM MainLoop()
{
	MSG msg;

	while(1)
	{	
		// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if (!TranslateAccelerator(msg.hwnd, g_hAccelTable, &msg))
			{
				// if the message wasn't to quit
				if(msg.message == WM_QUIT)
					break;

				// render the scene first if the 
				// cube state was changed
				else if (msg.message == RC_CHANGED)
					RenderScene();

				// find out what the message does
				TranslateMessage(&msg);

				// execute the message
				DispatchMessage(&msg);
			}
        }
		// if no message
		else
		{ 
			// Do computationally expensive things here.  We want to render the scene
			// every frame, so we call our rendering function here.  Even though the scene
			// doesn't change, it will bottle neck the message queue if we don't do something.
			// Usually WaitMessage() is used to make sure the app doesn't eat up the CPU.
			RenderScene();
        } 
	}

	return msg.wParam;
}


VOID RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The View
	
	CVector3 pos = g_cCamera.GetPosition();
	CVector3 lookAt = g_cCamera.GetLookAt();
	CVector3 up = g_cCamera.GetUp();

	gluLookAt(
		pos.x, pos.y, pos.z,
		lookAt.x, lookAt.y, lookAt.z,
		up.x, up.y, up.z
	);

#ifdef _DEBUG
	// Draw Axis
	glBegin(GL_LINES);
		// x
		glColor3f(1.0f, 0.6f, 0.0f); // orange
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(CUBE_SIZE * 3, 0.0, 0.0);
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glVertex3d(CUBE_SIZE * 3, 0.0, 0.0);
		glVertex3d(CUBE_SIZE * 4.5f, 0.0, 0.0);

		// y
		//glColor3f(0.0f, 1.0f, 0.0f);  // green
		glColor3f(1.0f, 1.0f, 1.0f);  // white
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, CUBE_SIZE * 3, 0.0);
		glColor3f(1.0f, 1.0f, 0.0f);  // yellow
		glVertex3d(0.0, CUBE_SIZE * 3, 0.0);
		glVertex3d(0.0, CUBE_SIZE * 4.5f, 0.0);

		// z
		glColor3f(0.0f, 1.0f, 0.0f);  // green
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, CUBE_SIZE * 3);
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glVertex3d(0.0, 0.0, CUBE_SIZE * 3);
		glVertex3d(0.0, 0.0, CUBE_SIZE * 4.5f);
	glEnd();
#endif
	
	glEnable(GL_LIGHTING);
	float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float* position = new float[4];

	g_cCamera.GetPosition().ToFloatArray(position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, color);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, color);
	glLightfv(GL_LIGHT1, GL_SHININESS, color);

	delete[] position;
	position = NULL;

	glEnable(GL_LIGHT1);

	float shininess = 5.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, color);
	//glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

	RenderRubiksCube();

	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHTING);

	SwapBuffers(g_hDC);
}

VOID RenderRubiksCube()
{
	glInitNames();

	for (int i = 0; i < RUBIKS_CUBE_SIZE; i++)
	{
		for (int j = 0; j < RUBIKS_CUBE_SIZE; j++)
		{
			for (int k = 0; k < RUBIKS_CUBE_SIZE; k++)
			{
				glPushMatrix();

				if (g_bRotating)
				{
					if (g_vRotationAxis.x && i == g_nRotatingSection ||
						g_vRotationAxis.y && j == g_nRotatingSection ||
						g_vRotationAxis.z && k == g_nRotatingSection)
					{
						int angle = g_bFlipRotation?-g_nRotationAngle:g_nRotationAngle;
						glRotated(angle, g_vRotationAxis.x, g_vRotationAxis.y, g_vRotationAxis.z);
					}
				}

				RenderCube(g_cCube.GetCube(i, j, k), i, j, k);

				glPopMatrix();
			}
		}
	}
}

VOID RenderCube(CCubeState* pCube, int x, int y, int z)
{
	glPushName(x);
	glPushName(y);
	glPushName(z);

	// scale to -1 to +1
	x--;
	y--;
	z--;

	glPushMatrix();

	glTranslated(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);

	CColor top = pCube->GetFaceColor(Top);
	CColor bottom = pCube->GetFaceColor(Bottom);
	CColor left = pCube->GetFaceColor(Left);
	CColor right = pCube->GetFaceColor(Right);
	CColor back = pCube->GetFaceColor(Back);
	CColor front = pCube->GetFaceColor(Front);
	
	glEnable(GL_TEXTURE_2D);	

	// Front Face
	glPushName((GLuint)Front);
	glBindTexture(GL_TEXTURE_2D, GetTextureID(front));
	glBegin(GL_QUADS);
		glColor3ub(front.r, front.g, front.b);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
	glEnd();
	glPopName();
		
	// Back Face
	glPushName((GLuint)Back);
	glBindTexture(GL_TEXTURE_2D, GetTextureID(back));
	glBegin(GL_QUADS);
		glColor3ub(back.r, back.g, back.b);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	glEnd();
	glPopName();
		
	// Top Face
	glPushName((GLuint)Top);
	glBindTexture(GL_TEXTURE_2D, GetTextureID(top));
	glBegin(GL_QUADS);
		glColor3ub(top.r, top.g, top.b);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
	glEnd();
	glPopName();
		
	// Bottom Face
	glPushName((GLuint)Bottom);
	glBindTexture(GL_TEXTURE_2D, GetTextureID(bottom));
	glBegin(GL_QUADS);
		glColor3ub(bottom.r, bottom.g, bottom.b);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
	glEnd();
	glPopName();
		
	// Right face
	glPushName((GLuint)Right);
	glBindTexture(GL_TEXTURE_2D, GetTextureID(right));
	glBegin(GL_QUADS);
		glColor3ub(right.r, right.g, right.b);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2d(1.0, 0.0); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
	glEnd();
	glPopName();
		
	// Left Face
	glPushName((GLuint)Left);
	glBindTexture(GL_TEXTURE_2D, GetTextureID(left));
	glBegin(GL_QUADS);
		glColor3ub(left.r, left.g, left.b);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
	glEnd();
	glPopName();	

	glPopName();
	glPopName();
	glPopName();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

INT GetSelectedObjects(int x, int y, GLsizei buffSize, GLuint* selectBuffer)
{
	GLint viewport[4];

	glSelectBuffer(buffSize,selectBuffer);
	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glGetIntegerv(GL_VIEWPORT,viewport);
	gluPickMatrix(x, viewport[3]- y, 1, 1, viewport);

	SetFrustum(g_rWnd.right, g_rWnd.bottom);

	glMatrixMode(GL_MODELVIEW);

	RenderScene();

	// restoring the original projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	
	return glRenderMode(GL_RENDER);
}

VOID GetCubeSelection(int *x, int *y, int *z, Face *face)
{
   GLuint names, *ptr, minZ, *ptrNames, numberOfNames;
   ptr = (GLuint *)g_pSelectBuffer;
   minZ = 0xffffffff;

   for (int i = 0; i < g_nHitCount && i < SELECT_BUFFER_SIZE; i++)
   {	
      names = *ptr;
	  ptr++;
	  if (*ptr < minZ)
	  {
		  numberOfNames = names;
		  minZ = *ptr;
		  ptrNames = ptr+2;
	  }
	  
	  ptr += names+2;
   }

   *x = ptrNames[0];
   *y = ptrNames[1];
   *z = ptrNames[2];
   *face = (Face)ptrNames[3];
}

CVector3 MapCoordinates(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	return CVector3(posX, posY, posZ);
}

VOID Initialize()
{
	g_cCamera.SetDistance(45.0);
	g_cCamera.SetPhi((float)(PI / 4));
	g_cCamera.SetTheta((float)(PI / 4));

	//g_cCube.Randomize();
}

GLuint GetTextureID(CColor color)
{
	if (color == CColor(0, 0, 0))
		return g_pTextures[6];
	else if (color == CColor(255, 255, 255))
		return g_pTextures[0];
	else if (color == CColor(0, 0, 255))
		return g_pTextures[1];
	else if (color == CColor(255, 165, 0))
		return g_pTextures[2];
	else if (color == CColor(255, 0, 0))
		return g_pTextures[3];
	else if (color == CColor(0, 255, 0))
		return g_pTextures[4];
	else if (color == CColor(255, 255, 0))
		return g_pTextures[5];
	else
		return g_pTextures[6];
}

VOID Scramble(const string& str)
{
	//RubiksCubeAlgoExecuter::test();
	RubiksCubeSimulator::scramble(str);
}

VOID Solve()
{
	RubiksCubeSolver_v1::solve();
}