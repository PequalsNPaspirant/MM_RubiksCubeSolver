// RubiksCube.cpp : Defines the entry point for the application.
//
#include "stdafx.h"

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "RubiksCubeSolverUI.h"
#include "RubiksCubeSimulator.h"
#include "RubiksCubeSolver_v1.h"
#include "RubiksCubeSolverTest.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	mm::Scene& s = mm::Scene::getInstance();
	s.createWindow(hInstance);
	WPARAM w = s.enterMainLoop();
	return int(w);
}

namespace mm {

	GLuint Scene::g_pSelectBuffer[Scene::SELECT_BUFFER_SIZE];

	Scene& Scene::getInstance()
	{
		static Scene s;
		return s;
	}

	Scene::Scene()
		: WND_WIDTH(400),
		WND_HEIGHT(400)
	{
	}

	void Scene::createWindow(HINSTANCE hInstance)
	{
		LoadString(NULL, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
		LoadString(NULL, IDC_RUBIKSCUBE, g_szWindowClass, MAX_LOADSTRING);
		g_hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_RUBIKSCUBE));
		g_hInstance = hInstance;
		g_hWnd = createWindow(g_szTitle, WND_WIDTH, WND_HEIGHT, 0, g_bFullScreen, g_hInstance);
		initOpenGl(g_hWnd);
		initScene();
	}

	HWND Scene::createWindow(LPTSTR strTitle, int nWidth, int nHeight, DWORD dwStyle,
		BOOL bFullScreen, HINSTANCE hInstance)
	{
		HWND hWnd;
		WNDCLASSEX wndClass;

		g_hArrow = LoadCursor(NULL, IDC_ARROW);
		g_hHand = LoadCursor(NULL, IDC_HAND);

		ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = Scene::WndProcCallback;
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
			changeToFullScreen();
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

	bool Scene::changeToFullScreen()
	{
		DEVMODE dmSettings;									// Device Mode variable

		memset(&dmSettings, 0, sizeof(dmSettings));			// Makes Sure Memory's Cleared

															// Get current settings -- This function fills our the settings
															// This makes sure NT and Win98 machines change correctly
		if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
			return FALSE;

		// selected screen width and height
		dmSettings.dmPelsWidth = WND_WIDTH;
		dmSettings.dmPelsHeight = WND_HEIGHT;

		// This function actually changes the screen to full screen
		// CDS_FULLSCREEN Gets Rid Of Start Bar.
		// We always want to get a result from this function to check if we failed
		int result = ChangeDisplaySettings(&dmSettings, CDS_FULLSCREEN);

		// Check if we didn't recieved a good return message From the function
		if (result != DISP_CHANGE_SUCCESSFUL)
			return FALSE;

		return TRUE;
	}

	void Scene::initOpenGl(HWND hWnd)
	{
		GetClientRect(hWnd, &g_rWnd);
		g_hDC = GetDC(hWnd);

		if (!setupPixelFormat(g_hDC))
			PostQuitMessage(-1);

		g_hRC = wglCreateContext(g_hDC);
		wglMakeCurrent(g_hDC, g_hRC);

		sizeOpenGlScreen(g_rWnd.right, g_rWnd.bottom);

		glPolygonMode(GL_FRONT, GL_FILL);
		glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

		//glEnable(GL_COLOR_MATERIAL);
		//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		glClearDepth(1.0f);											// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
		glLineWidth(LINE_WIDTH);									// Set outline width

		loadTexture(IDB_WHITE, &g_pTextures[0]);
		loadTexture(IDB_BLUE, &g_pTextures[1]);
		loadTexture(IDB_ORANGE, &g_pTextures[2]);
		loadTexture(IDB_RED, &g_pTextures[3]);
		loadTexture(IDB_GREEN, &g_pTextures[4]);
		loadTexture(IDB_YELLOW, &g_pTextures[5]);
		loadTexture(IDB_BLACK, &g_pTextures[6]);
	}

	void Scene::loadTexture(int nId, GLuint* texture)
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

	void Scene::sizeOpenGlScreen(int nWidth, int nHeight)
	{
		if (nHeight == 0)
		{
			nHeight = 1;
		}

		glViewport(0, 0, nWidth, nHeight);
		glMatrixMode(GL_PROJECTION);
		//glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		setFrustum(nWidth, nHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void Scene::setFrustum(int nWidth, int nHeight)
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

	void Scene::initScene()
	{
		g_cCamera.SetDistance(45.0);
		g_cCamera.SetPhi((float)(PI / 4));
		g_cCamera.SetTheta((float)(PI / 4));

		//g_cCube.Randomize();
	}

	bool Scene::setupPixelFormat(HDC hdc)
	{
		PIXELFORMATDESCRIPTOR pfd = { 0 };
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
		if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE)
			return FALSE;

		// This sets the pixel format that we extracted from above
		if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE)
			return FALSE;

		return TRUE;
	}

	WPARAM Scene::enterMainLoop()
	{
		MSG msg;

		while (1)
		{
			// Check if there was a message
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (!TranslateAccelerator(msg.hwnd, g_hAccelTable, &msg))
				{
					// if the message wasn't to quit
					if (msg.message == WM_QUIT)
						break;

					// render the scene first if the 
					// cube state was changed
					else if (msg.message == RC_CHANGED)
						renderScene();

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
				renderScene();
			}
		}

		return msg.wParam;
	}

	void Scene::renderScene()
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
		float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

		renderRubiksCube();

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHTING);

		SwapBuffers(g_hDC);
	}

	void Scene::renderRubiksCube()
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
							int angle = g_bFlipRotation ? -g_nRotationAngle : g_nRotationAngle;
							glRotated(angle, g_vRotationAxis.x, g_vRotationAxis.y, g_vRotationAxis.z);
						}
					}

					renderIndividualCube(g_cCube.GetCube(i, j, k), i, j, k);

					glPopMatrix();
				}
			}
		}
	}

	void Scene::renderIndividualCube(CCubeState* pCube, int x, int y, int z)
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
		glBindTexture(GL_TEXTURE_2D, getTextureID(front));
		glBegin(GL_QUADS);
		glColor3ub(front.r, front.g, front.b);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
		glEnd();
		glPopName();

		// Back Face
		glPushName((GLuint)Back);
		glBindTexture(GL_TEXTURE_2D, getTextureID(back));
		glBegin(GL_QUADS);
		glColor3ub(back.r, back.g, back.b);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
		glPopName();

		// Top Face
		glPushName((GLuint)Top);
		glBindTexture(GL_TEXTURE_2D, getTextureID(top));
		glBegin(GL_QUADS);
		glColor3ub(top.r, top.g, top.b);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glEnd();
		glPopName();

		// Bottom Face
		glPushName((GLuint)Bottom);
		glBindTexture(GL_TEXTURE_2D, getTextureID(bottom));
		glBegin(GL_QUADS);
		glColor3ub(bottom.r, bottom.g, bottom.b);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glEnd();
		glPopName();

		// Right face
		glPushName((GLuint)Right);
		glBindTexture(GL_TEXTURE_2D, getTextureID(right));
		glBegin(GL_QUADS);
		glColor3ub(right.r, right.g, right.b);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
		glPopName();

		// Left Face
		glPushName((GLuint)Left);
		glBindTexture(GL_TEXTURE_2D, getTextureID(left));
		glBegin(GL_QUADS);
		glColor3ub(left.r, left.g, left.b);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glEnd();
		glPopName();

		glPopName();
		glPopName();
		glPopName();

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	GLuint Scene::getTextureID(CColor color)
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

	// ======================== Callback functions ======================== //

	LRESULT CALLBACK Scene::WndProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

	LRESULT CALLBACK Scene::AboutProcCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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






	void Scene::deInit()
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

	INT Scene::getSelectedObjects(int x, int y, GLsizei buffSize, GLuint* selectBuffer)
	{
		GLint viewport[4];

		glSelectBuffer(buffSize, selectBuffer);
		glRenderMode(GL_SELECT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glGetIntegerv(GL_VIEWPORT, viewport);
		gluPickMatrix(x, viewport[3] - y, 1, 1, viewport);

		setFrustum(g_rWnd.right, g_rWnd.bottom);

		glMatrixMode(GL_MODELVIEW);

		renderScene();

		// restoring the original projection matrix
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glFlush();

		return glRenderMode(GL_RENDER);
	}

	CVector3 Scene::mapCoordinates(int x, int y)
	{
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		return CVector3(posX, posY, posZ);
	}


	// ============================== Message Handling ================================= //


	void Scene::OnRubiksCubeChanged(HWND hWnd)
	{
		mm::Scene& s = mm::Scene::getInstance();

		// check for solution
		if (s.g_cCube.IsSolved())
		{
			TCHAR solvedMsg[MAX_LOADSTRING];
			LoadString(s.g_hInstance, IDS_SOLVED, solvedMsg, MAX_LOADSTRING);

			//MessageBox(g_hWnd, solvedMsg, g_szTitle, MB_OK);
		}
	}

	//
	//  Process WM_LBUTTONDOWN message for window/dialog: 
	//
	void Scene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
	{
		mm::Scene& s = mm::Scene::getInstance();

		// set up tracking for when mouse leaves window
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		TrackMouseEvent(&tme);

		s.g_bMouseDown = TRUE;

		if ((s.g_nHitCount = s.getSelectedObjects(x, y, s.SELECT_BUFFER_SIZE, s.g_pSelectBuffer)) > 0)
			s.g_vMouseDown = s.mapCoordinates(x, y);

		s.g_nPrevX = x;
		s.g_nPrevY = y;
	}

	//
	//  Process WM_LBUTTONUP message for window/dialog: 
	//
	void Scene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
	{
		mm::Scene& s = mm::Scene::getInstance();

		s.g_bMouseDown = FALSE;

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
	void Scene::OnDestroy(HWND hWnd)
	{
		mm::Scene& s = mm::Scene::getInstance();

		s.deInit();
	}

	//
	//  Process WM_MOUSEMOVE message for window/dialog: 
	//
	void Scene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
	{
		mm::Scene& s = mm::Scene::getInstance();

		if (!s.g_bMouseDown)
		{
			/*
			if ((g_nHitCount = GetSelectedObjects(x, y, SELECT_BUFFER_SIZE, g_pSelectBuffer)) > 0)
			SetCursor(g_hHand);
			else
			SetCursor(g_hArrow);
			*/
		}

		// moving camera
		else if (s.g_nHitCount == 0 && s.g_bMouseDown)
		{
			if (x < s.g_nPrevX)
				s.g_cCamera.Rotate(-5);
			else if (x > s.g_nPrevX)
				s.g_cCamera.Rotate(5);

			if (y < s.g_nPrevY)
				s.g_cCamera.Tilt(5);
			else if (y > s.g_nPrevY)
				s.g_cCamera.Tilt(-5);
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
		s.g_nPrevX = x;
		s.g_nPrevY = y;
	}

	void Scene::getCubeSelection(int *x, int *y, int *z, Face *face)
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
				ptrNames = ptr + 2;
			}

			ptr += names + 2;
		}

		*x = ptrNames[0];
		*y = ptrNames[1];
		*z = ptrNames[2];
		*face = (Face)ptrNames[3];
	}

	//
	//  Process WM_MOUSEWHEEL message for window/dialog: 
	//
	void Scene::OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys)
	{
		mm::Scene& s = mm::Scene::getInstance();

		int rotations = zDelta / WHEEL_DELTA;
		float distance = rotations * 1.0f;

		s.g_cCamera.Move(distance);
	}

	//
	//  Process WM_SIZE message for window/dialog: 
	//
	void Scene::OnSize(HWND hWnd, UINT state, int cx, int cy)
	{
		mm::Scene& s = mm::Scene::getInstance();

		if (!s.g_bFullScreen)
		{
			s.sizeOpenGlScreen(cx, cy);
			GetClientRect(hWnd, &s.g_rWnd);
		}
	}

	void Scene::OnMouseLeave(HWND hWnd)
	{
		mm::Scene& s = mm::Scene::getInstance();

		s.g_bMouseDown = FALSE;
		//g_nRotationAngle = 0;
	}

	//
	//  Process WM_COMMAND message for window/dialog: 
	//
	void Scene::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
	{
		mm::Scene& s = mm::Scene::getInstance();

		// menu
		if (id == IDM_FILE_SCRAMBLE)
		{
			//wstring str(L"Scramble using these steps? - ");
			//TCHAR newGameMsg[MAX_LOADSTRING];
			//LoadString(g_hInstance, IDS_NEWGAME, newGameMsg, MAX_LOADSTRING);

			string str = RubiksCubeSolverTest::getScramblingAlgo();
			wstring wstr(str.begin(), str.end());
			wstr = L"Scramble using these steps? - " + wstr;
			if (MessageBox(hwnd, wstr.c_str(),
				s.g_szTitle, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
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
			DialogBox(s.g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX),
				s.g_hWnd, reinterpret_cast<DLGPROC>(Scene::AboutProcCallback));
		}

		else if (id == IDM_EXIT)
		{
			PostQuitMessage(0);
		}
	}

	void Scene::Scramble(const string& str)
	{
		//RubiksCubeAlgoExecuter::test();
		RubiksCubeSimulator::scramble(str);
	}

	void Scene::Solve()
	{
		RubiksCubeSolver_v1::solve();
	}


}

