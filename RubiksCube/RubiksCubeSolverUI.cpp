// RubiksCube.cpp : Defines the entry point for the application.
//
#include "stdafx.h"

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>

#include "RubiksCubeSolverUI.h"
#include "RubiksCubeSolverTest.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	mm::RubiksCubeSolverUI& mainWindow = mm::RubiksCubeSolverUI::getInstance();
	mainWindow.createWindow(hInstance);
	WPARAM w = mainWindow.enterMainLoop();
	return int(w);
}

namespace mm {

	RubiksCubeSolverUI& RubiksCubeSolverUI::getInstance()
	{
		static RubiksCubeSolverUI mainWindow;
		return mainWindow;
	}

	RubiksCubeSolverUI::RubiksCubeSolverUI()
		: WND_WIDTH(400),
		WND_HEIGHT(400),
		//scene_(*this, "RubiksCubeModel_v1", 3),
		scene_(*this, "RubiksCubeModel_v2", 3),
		framesPerRotation_(20),
		sleepTimeMilliSec_(5),
		tester_(*this)
	{
	}

	void RubiksCubeSolverUI::createWindow(HINSTANCE hInstance)
	{
		LoadString(NULL, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
		LoadString(NULL, IDC_RUBIKSCUBE, g_szWindowClass, MAX_LOADSTRING);
		g_hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_RUBIKSCUBE));
		g_hInstance = hInstance;
		g_hWnd = createWindow(g_szTitle, WND_WIDTH, WND_HEIGHT, 0, g_bFullScreen, g_hInstance);

		GetClientRect(g_hWnd, &g_rWnd);
		g_hDC = GetDC(g_hWnd);

		g_hRC = wglCreateContext(g_hDC);
		wglMakeCurrent(g_hDC, g_hRC);

		scene_.initOpenGl(g_rWnd.right, g_rWnd.bottom);
		scene_.initScene();
	}

	HWND RubiksCubeSolverUI::createWindow(LPTSTR strTitle, int nWidth, int nHeight, DWORD dwStyle,
		BOOL bFullScreen, HINSTANCE hInstance)
	{
		HWND hWnd;
		WNDCLASSEX wndClass;

		g_hArrow = LoadCursor(NULL, IDC_ARROW);
		g_hHand = LoadCursor(NULL, IDC_HAND);

		ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = RubiksCubeSolverUI::WndProcCallback;
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

		AdjustWindowRectEx(&rWnd, dwStyle, true, 0);

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

		ShowWindow(hWnd, SW_SHOWNORMAL);
		//ShowWindow(hWnd, SW_MAXIMIZE);
		UpdateWindow(hWnd);

		SetFocus(hWnd);

		GetClientRect(hWnd, &g_rWnd);
		g_hDC = GetDC(hWnd);

		if (!setupPixelFormat(g_hDC))
			PostQuitMessage(-1);

		return hWnd;
	}

	void RubiksCubeSolverUI::redrawWindow()
	{
		scene_.renderScene();
		SwapBuffers(g_hDC);
	}

	bool RubiksCubeSolverUI::CreateYesNoDialog(const string& message)
	{
		wstring wMessage(message.begin(), message.end());
		if (MessageBox(g_hWnd, wMessage.c_str(), g_szTitle, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
			return true;
		else
			return false;
	}

	void RubiksCubeSolverUI::CreateOkDialog(const string& message)
	{
		wstring wMessage(message.begin(), message.end());
		MessageBox(g_hWnd, wMessage.c_str(), g_szTitle, MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);
	}

	void RubiksCubeSolverUI::applyAlgorithm(const string& algo, bool animate)
	{
		scene_.applyAlgorithmToCube(algo, animate);
	}

	void RubiksCubeSolverUI::replaceModelBy(const string& modelName, int size)
	{
		scene_.replaceModelBy(modelName, size);
		redrawWindow();
	}

	bool RubiksCubeSolverUI::isSolved()
	{
		return scene_.rubicCubeModel_->isSolved();
	}

	bool RubiksCubeSolverUI::changeToFullScreen()
	{
		DEVMODE dmSettings;	// Device Mode variable

		memset(&dmSettings, 0, sizeof(dmSettings)); // Makes Sure Memory's Cleared

		// Get current settings -- This function fills our the settings
		// This makes sure NT and Win98 machines change correctly
		if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmSettings))
			return false;

		// selected screen width and height
		dmSettings.dmPelsWidth = WND_WIDTH;
		dmSettings.dmPelsHeight = WND_HEIGHT;

		// This function actually changes the screen to full screen
		// CDS_FULLSCREEN Gets Rid Of Start Bar.
		// We always want to get a result from this function to check if we failed
		int result = ChangeDisplaySettings(&dmSettings, CDS_FULLSCREEN);

		// Check if we didn't recieved a good return message From the function
		if (result != DISP_CHANGE_SUCCESSFUL)
			return false;

		return true;
	}

	bool RubiksCubeSolverUI::setupPixelFormat(HDC hdc)
	{
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		int pixelformat;

		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the structure
		pfd.nVersion = 1; // Always set this to 1
															
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // Pass in the appropriate OpenGL flags
		pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask (this is ignored anyway)
		pfd.iPixelType = PFD_TYPE_RGBA;						// We want RGB and Alpha pixel type
		pfd.cColorBits = SCREEN_DEPTH;						// Here we use our #define for the color bits
		pfd.cDepthBits = SCREEN_DEPTH;						// Depthbits is ignored for RGBA, but we do it anyway
		pfd.cAccumBits = 0;									// No special bitplanes needed
		pfd.cStencilBits = 0;								// We desire no stencil bits

		// This gets us a pixel format that best matches the one passed in from the device
		if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == false)
			return false;

		// This sets the pixel format that we extracted from above
		if (SetPixelFormat(hdc, pixelformat, &pfd) == false)
			return false;

		return true;
	}

	WPARAM RubiksCubeSolverUI::enterMainLoop()
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
					{
						redrawWindow();
					}

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
				redrawWindow();
			}
		}

		return msg.wParam;
	}

	// ======================== Callback functions ======================== //

	LRESULT CALLBACK RubiksCubeSolverUI::WndProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		RubiksCubeSolverUI& mainWindow = RubiksCubeSolverUI::getInstance();

		switch (uMsg)
		{
			HANDLE_MSG(hWnd, WM_COMMAND, mainWindow.OnCommand);
			HANDLE_MSG(hWnd, RC_CHANGED, mainWindow.OnRubiksCubeChanged);
			//HANDLE_MSG(hWnd, WM_PAINT, mainWindow.OnPaint);
			HANDLE_MSG(hWnd, WM_LBUTTONDOWN, mainWindow.OnLButtonDown);
			HANDLE_MSG(hWnd, WM_LBUTTONUP, mainWindow.OnLButtonUp);
			HANDLE_MSG(hWnd, WM_MOUSEMOVE, mainWindow.OnMouseMove);
			HANDLE_MSG(hWnd, WM_SIZE, mainWindow.OnSize);
			HANDLE_MSG(hWnd, WM_MOUSELEAVE, mainWindow.OnMouseLeave);
			HANDLE_MSG(hWnd, WM_MOUSEWHEEL, mainWindow.OnMouseWheel);
			HANDLE_MSG(hWnd, WM_DESTROY, mainWindow.OnDestroy);
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	LRESULT CALLBACK RubiksCubeSolverUI::AboutProcCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_COMMAND:
			switch (wParam)
			{
			case IDOK:
				EndDialog(hwndDlg, 0);
				return true;
			}
			break;
		}

		return false;
	}

	void RubiksCubeSolverUI::deInit()
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

	// ============================== Message Handling ================================= //

	void RubiksCubeSolverUI::OnRubiksCubeChanged(HWND hWnd)
	{
		// check for solution
		//if (scene_.g_cCube.IsSolved())
		{
			TCHAR solvedMsg[MAX_LOADSTRING];
			LoadString(g_hInstance, IDS_SOLVED, solvedMsg, MAX_LOADSTRING);
			//MessageBox(g_hWnd, solvedMsg, g_szTitle, MB_OK);
		}
	}

	//  Process WM_LBUTTONDOWN message for window/dialog: 
	void RubiksCubeSolverUI::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
	{
		// set up tracking for when mouse leaves window
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		TrackMouseEvent(&tme);

		g_bMouseDown = true;

		if ((g_nHitCount = scene_.getSelectedObjects(x, y, g_rWnd.right, g_rWnd.bottom)) > 0)
			g_vMouseDown = scene_.mapCoordinates(x, y);

		g_nPrevX = x;
		g_nPrevY = y;
	}

	//  Process WM_LBUTTONUP message for window/dialog: 
	void RubiksCubeSolverUI::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
	{
		g_bMouseDown = false;

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

		g_bRotating = false;
		g_bFlipRotation = false;

		PostMessage(g_hWnd, RC_CHANGED, 0, 0);
		}
		*/
	}

	//  Process WM_DESTROY message for window/dialog: 
	void RubiksCubeSolverUI::OnDestroy(HWND hWnd)
	{
		deInit();
	}

	//  Process WM_MOUSEMOVE message for window/dialog: 
	void RubiksCubeSolverUI::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
	{
		if (!g_bMouseDown)
		{
			/*
			if ((g_nHitCount = GetSelectedObjects(x, y, g_rWnd.right, g_rWnd.bottom)) > 0)
			SetCursor(g_hHand);
			else
			SetCursor(g_hArrow);
			*/
		}

		// moving camera
		else if (g_nHitCount == 0 && g_bMouseDown)
		{
			if (x < g_nPrevX)
				scene_.g_cCamera.Rotate(-5);
			else if (x > g_nPrevX)
				scene_.g_cCamera.Rotate(5);

			if (y < g_nPrevY)
				scene_.g_cCamera.Tilt(5);
			else if (y > g_nPrevY)
				scene_.g_cCamera.Tilt(-5);
		}
		/*
		// rotating section
		else if (g_nHitCount > 0 && g_bMouseDown && !g_bRotating)
		{
		int deltaX = abs(x - g_nPrevX);
		int deltaY = abs(y - g_nPrevY);
		int i, j , k = 0;
		Face face;

		GetCubeSelection(&i, &j, &k, &face, g_nHitCount);

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

		g_bRotating = true;
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

	//  Process WM_MOUSEWHEEL message for window/dialog: 
	void RubiksCubeSolverUI::OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys)
	{
		int rotations = zDelta / WHEEL_DELTA;
		float distance = rotations * 1.0f;

		scene_.g_cCamera.Move(distance);
	}

	//  Process WM_SIZE message for window/dialog: 
	void RubiksCubeSolverUI::OnSize(HWND hWnd, UINT state, int cx, int cy)
	{
		if (!g_bFullScreen)
		{
			scene_.sizeOpenGlScreen(cx, cy);
			GetClientRect(hWnd, &g_rWnd);
		}
	}

	void RubiksCubeSolverUI::OnMouseLeave(HWND hWnd)
	{
		g_bMouseDown = false;
		//g_nRotationAngle = 0;
	}

	//  Process WM_COMMAND message for window/dialog: 
	void RubiksCubeSolverUI::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
	{
		// menu
		if (id == IDM_FILE_RESET)
		{
			Reset();
		}
		else if (id == IDM_FILE_SCRAMBLE)
		{
			Scramble();
		}
		else if (id == IDM_FILE_SOLVE)
		{
			int solutionSteps;
			unsigned long long duration;
			bool askForAnimation = true;
			string solution = SolveOnCopy(solutionSteps, duration, askForAnimation);
		}
		else if (id == IDM_FILE_SOLVE_ANIM)
		{
			int solutionSteps;
			unsigned long long duration;
			bool animate = true;
			string solution = Solve(solutionSteps, duration, animate);
		}
		else if (id == ID_RUBIKSCUBE_TEST)
		{
			bool animate = false;
			testRubiksCube(animate);
		}
		else if(id == ID_RUBIKSCUBE_TEST_ANIM)
		{
			bool animate = true;
			testRubiksCube(animate);
		}
		else if (id == IDM_ABOUT)
		{
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX),
				g_hWnd, reinterpret_cast<DLGPROC>(RubiksCubeSolverUI::AboutProcCallback));
		}
		else if (id == IDM_EXIT)
		{
			PostQuitMessage(0);
		}
	}

	void RubiksCubeSolverUI::Reset()
	{
		scene_.Reset();
		redrawWindow();
	}

	void RubiksCubeSolverUI::Scramble()
	{
		string algo = scene_.getScramblingAlgo(25);
		wstring wAlgo(algo.begin(), algo.end());
		wstring wMessage = L"Scramble using following Algorithm?";
		wMessage = wMessage
			+ L"\nAlgorithm      : " + wAlgo
			+ L"\nNumber of steps: " + to_wstring(25);
		if (MessageBox(g_hWnd, wMessage.c_str(),
			g_szTitle, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
		{
			scene_.applyAlgorithmToCube(algo, true);
		}
	}

	string RubiksCubeSolverUI::SolveOnCopy(int& solutionSteps, unsigned long long& duration, bool askForAnimation)
	{
		string solution = scene_.SolveOnCopy(solutionSteps, duration);
		
		if(!askForAnimation)
			return solution;

		wstring wDuration = to_wstring(duration % 1000);
		duration /= 1000;
		while (duration > 0)
		{
			wDuration = L"," + wDuration;
			wDuration = to_wstring(duration % 1000) + wDuration;
			duration /= 1000;
		}

		wstring wSolution(solution.begin(), solution.end());
		wstring wMessage = L"Solution: " + wSolution
			+ L"\nNumber of steps: " + to_wstring(solutionSteps)
			+ L"\nTime required: " + wDuration + L" ns" 
			+ L"\nDo you want to see animation of solution?";
		if (MessageBox(g_hWnd, wMessage.c_str(),
			g_szTitle, MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
		{
			scene_.applyAlgorithmToCube(solution, true);
		}

		return solution;
	}

	string RubiksCubeSolverUI::Solve(int& solutionSteps, unsigned long long& duration, bool animate)
	{
		string solution = scene_.Solve(solutionSteps, duration, animate);
		return solution;
	}

	void RubiksCubeSolverUI::testRubiksCube(bool animate)
	{
		tester_.testRubiksCube(animate);
	}
}

