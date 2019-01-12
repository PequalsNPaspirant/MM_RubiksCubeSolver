//=======================================================================================================//
//   Copyright (c) 2018 Maruti Mhetre                                                                    //
//   All rights reserved.                                                                                //
//=======================================================================================================//
//   Redistribution and use of this software in source and binary forms, with or without modification,   //
//   are permitted for personal, educational or non-commercial purposes provided that the following      //
//   conditions are met:                                                                                 //
//   1. Redistributions of source code must retain the above copyright notice, this list of conditions   //
//      and the following disclaimer.                                                                    //
//   2. Redistributions in binary form must reproduce the above copyright notice, this list of           //
//      conditions and the following disclaimer in the documentation and/or other materials provided     //
//      with the distribution.                                                                           //
//   3. Neither the name of the copyright holder nor the names of its contributors may be used to        //
//      endorse or promote products derived from this software without specific prior written            //
//      permission.                                                                                      //
//=======================================================================================================//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR      //
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND    //
//   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          //
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   //
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   //
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  //
//   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT   //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     //
//=======================================================================================================//

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
		string SolveOnCopy(unsigned int& solutionSteps, unsigned long long& duration, bool askForAnimation);
		string Solve(unsigned int& solutionSteps, unsigned long long& duration, bool animate);
		void testRubiksCube(bool animate);
		void fitToScreen();

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