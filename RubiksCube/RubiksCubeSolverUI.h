#pragma once

#include <string>
using namespace std;

#include "RubiksCubeSolverScene.h"
#include "RubiksCubeSolverTest.h"

namespace mm {

#define MAX_LOADSTRING 100
#define RC_CHANGED WM_APP + 100

	class RubiksCubeSolverUI
	{
	public:
		static RubiksCubeSolverUI& getInstance();
		void createWindow(HINSTANCE hInstance);
		WPARAM enterMainLoop();
		void redrawWindow();
		bool CreateYesNoDialog(const string& message);
		void CreateOkDialog(const string& message);
		void applyAlgorithm(const string& algo, bool animate);
		unique_ptr<RubiksCubeModel> replaceModelBy(const string& modelName, int size, bool animate);
		unique_ptr<RubiksCubeModel> replaceModelBy(unique_ptr<RubiksCubeModel>&& newModel, bool animate);
		bool isSolved();
		/**/HWND createWindow(LPTSTR strTitle, int nWidth, int nHeight, DWORD dwStyle, BOOL bFullScreen, HINSTANCE hInstance);
		/**//**/bool changeToFullScreen();
		/**//**/bool setupPixelFormat(HDC hdc);
		int getFramesPerRotation() { return framesPerRotation_; }
		int getSleepTimeMilliSec() { return sleepTimeMilliSec_; }

		//Menu Handlers
		void Reset(bool animate);
		void Scramble();
		string SolveOnCopy(int& solutionSteps, unsigned long long& duration, bool askForAnimation);
		string Solve(int& solutionSteps, unsigned long long& duration, bool animate);
		void testRubiksCube(bool animate);

		//Windows callbacks
		static LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK AboutProcCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//deleted functions
		RubiksCubeSolverUI();
		RubiksCubeSolverUI(const RubiksCubeSolverUI&) = delete;
		RubiksCubeSolverUI& operator=(const RubiksCubeSolverUI&) = delete;
		RubiksCubeSolverUI(RubiksCubeSolverUI&&) = delete;
		RubiksCubeSolverUI& operator=(RubiksCubeSolverUI&&) = delete;

		//unused functions
		void deInit();

	private:
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

		HGLRC g_hRC;
		HDC g_hDC;
		TCHAR g_szWindowClass[MAX_LOADSTRING]; // the main window class name
		HACCEL g_hAccelTable;
		const int WND_WIDTH = 400;
		const int WND_HEIGHT = 400;
		const int SCREEN_DEPTH = 16;
		bool g_bMouseDown;
		int g_nPrevX;
		int g_nPrevY;
		bool g_bFullScreen;
		RECT g_rWnd;
		CVector3 g_vMouseDown;
		int g_nHitCount;
		HCURSOR g_hArrow;
		HCURSOR g_hHand;
		TCHAR g_szTitle[MAX_LOADSTRING]; // The title bar text
		HWND g_hWnd;
		HINSTANCE g_hInstance; // current instance
		int framesPerRotation_;
		int sleepTimeMilliSec_;

		RubiksCubeSolverScene scene_;
		RubiksCubeSolverTest tester_;
	};

}