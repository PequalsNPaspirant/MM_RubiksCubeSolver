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
#include <atomic>
#include <thread>
//#include <mutex>
using namespace std;

#include "RubiksCubeSolverScene.h"
#include "RubiksCubeSolverTest.h"

namespace mm {

#define MAX_LOADSTRING 100
#define RC_CHANGED WM_APP + 100

	//enum class AnimationSpeed
	//{
	//	verySlow = 0,
	//	slow,
	//	moderate,
	//	fast,
	//	veryFast
	//};

	class RubiksCubeSolverGUI
	{
		//=================== start of approved interface ==============

	public:
		RubiksCubeSolverGUI();
		RubiksCubeSolverGUI(const RubiksCubeSolverGUI&) = delete;
		RubiksCubeSolverGUI& operator=(const RubiksCubeSolverGUI&) = delete;
		RubiksCubeSolverGUI(RubiksCubeSolverGUI&&) = delete;
		RubiksCubeSolverGUI& operator=(RubiksCubeSolverGUI&&) = delete;
		~RubiksCubeSolverGUI();
		void initialize(HWND hWnd);
		void Scramble(bool animateIn)
		{
			animate_ = animateIn;
			firstGenCommand_ = firstGenerationCommands::eScramble;
			activateRenderingThread();
		}
		void Solve(bool animateIn)
		{
			animate_ = animateIn;
			firstGenCommand_ = firstGenerationCommands::eSolve;
			activateRenderingThread();
		}
		void runTests(bool animateIn)
		{
			animate_ = animateIn;
			firstGenCommand_ = firstGenerationCommands::eRunTests;
			activateRenderingThread();
		}
		void fitToScreen()
		{
			firstGenCommand_ = firstGenerationCommands::eFitToScreen;
			activateRenderingThread();
		}
		void setRubiksCubeSize(unsigned int size)
		{
			firstGenCommand_ = firstGenerationCommands::eResizeRubiksCube;
			size_ = size;
			activateRenderingThread();
		}
		void setAnimationSpeed(unsigned int speed)
		{
			firstGenCommand_ = firstGenerationCommands::eNoCommand;
			secondGenCommand_ = secondGenerationCommands::eSetAnimationSpeed;
			animationSpeed_ = speed;
			framesPerRotation_ = (106 - animationSpeed_) / 2;
			sleepTimeMilliSec_ = (106 - animationSpeed_) / 2;
			/*
			case ID_ANIMATIONSPEED_VERYSLOW:
			//framesPerRotation = 40;
			//sleepTimeMilliSec = 40;
			rubiksCubeSolverGui_.setAnimationSpeed(10);
			break;
			case ID_ANIMATIONSPEED_SLOW:
			//framesPerRotation = 30;
			//sleepTimeMilliSec = 30;
			rubiksCubeSolverGui_.setAnimationSpeed(25);
			break;
			case ID_ANIMATIONSPEED_MODERATE:
			//framesPerRotation = 20;
			//sleepTimeMilliSec = 20;
			rubiksCubeSolverGui_.setAnimationSpeed(50);
			break;
			case ID_ANIMATIONSPEED_FAST:
			//framesPerRotation = 10;
			//sleepTimeMilliSec = 10;
			rubiksCubeSolverGui_.setAnimationSpeed(75);
			break;
			case ID_ANIMATIONSPEED_VERYFAST:
			//framesPerRotation = 3;
			//sleepTimeMilliSec = 3;
			rubiksCubeSolverGui_.setAnimationSpeed(100);
			break;
			*/
			activateRenderingThread();
		}
		void resetRubiksCube()
		{
			firstGenCommand_ = firstGenerationCommands::eNoCommand;
			secondGenCommand_ = secondGenerationCommands::eResetRubiksCube;
			resetRubiksCube_ = true;
			activateRenderingThread();
		}
		unique_ptr<RubiksCubeModel> replaceModelBy(const string& modelName, int size, bool animate);
		unique_ptr<RubiksCubeModel> replaceModelBy(unique_ptr<RubiksCubeModel>&& newModel, bool animate);
		string Solve(unsigned int& solutionSteps, unsigned long long& duration, bool animate);

	private:
		bool activateRenderingThread();
		void ScrambleImpl();
		string SolveOnCopy(unsigned int& solutionSteps, unsigned long long& duration, bool askForAnimation);
		void runRubiksCubeTests();
		void fitToScreenImpl();

		enum class firstGenerationCommands
		{
			eNoCommand = 0,
			eScramble,
			eSolve,
			eRunTests,
			eFitToScreen,
			eResizeRubiksCube,
			eMax
		};
		enum class secondGenerationCommands
		{
			eNoCommand = 0,
			eSetAnimationSpeed,
			eResetRubiksCube,
			eMax
		};

		firstGenerationCommands firstGenCommand_{ firstGenerationCommands::eNoCommand };
		secondGenerationCommands secondGenCommand_{ secondGenerationCommands::eNoCommand };
		bool animate_{ false };
		string currentModelName_;
		unsigned int size_{ 3 };
		unsigned int animationSpeed_{ 50 }; //Range from 0 (slowest) to 100 (fastest)

		//=================== end of approved interface ==============

	public:
		//static RubiksCubeSolverGUI& getInstance();
		//void createMainWindow(HINSTANCE hInstance);
		///**/bool changeToFullScreen();
		//void createMenu();
		
		void render();
		void createGraphicsArea();
		
		void commandHandler();
		/**/bool setupPixelFormat(HDC hdc);
		//void createMessageWindow();
		//WPARAM enterMainLoop();
		void redrawWindow();
		//bool CreateYesNoDialog(const string& message);
		void CreateOkDialog(const string& message);
		void applyAlgorithm(const string& algo, bool animate);
		bool isSolved();
		int getFramesPerRotation() { return framesPerRotation_; }
		void setFramesPerRotation(int val) { framesPerRotation_ = val; }
		int getSleepTimeMilliSec() { return sleepTimeMilliSec_; }
		void setSleepTimeMilliSec(int val) { sleepTimeMilliSec_ = val; }
		bool getResetRubiksCube() { return resetRubiksCube_; }
		void setResetRubiksCube(bool newValue)
		{
			//std::unique_lock<std::mutex> lock(mutex_); //not required to synchronize
			resetRubiksCube_ = newValue;
		}
		void displayMessage(const string& message = "");
		//void displayMessage(int scramblingSteps, const string& scramblingAlgo, int solutionSteps, const string& solution, unsigned long long duration);
		//void displayMessage_currentLine(int left, int top, const string& line);

		//Menu Handlers
		void Reset(bool animate);
		
		//Windows callbacks
		//static LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		//static LRESULT CALLBACK AboutProcCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	

		//unused functions
		//void deInit();

	//private:
		/* void Cls_OnMouseLeave(HWND hWnd, int x, int y, UINT keyFlags) */
//#define HANDLE_WM_MOUSELEAVE(hWnd, wParam, lParam, fn) \
//    ((fn)((hWnd)), 0L)
//#define HANDLE_RC_CHANGED(hWnd, wParam, lParam, fn) \
//	((fn)((hWnd)), 0L)

		void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
		void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
		void OnDestroy(HWND hWnd);
		void OnMouseMove(int rotate, int tilt);
		void OnMouseWheel(float distance);
		void OnSize(int cx, int cy);
		void OnMouseLeave(HWND hWnd);
		void OnRubiksCubeChanged(HWND hWnd);
		void OnPaint(HWND hWnd);
		//void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
		BOOL OnEraseBackground(HWND hwnd, HDC hdc);

		HGLRC g_hRC;
		//TCHAR g_szWindowClass[MAX_LOADSTRING]; // the main window class name
		HACCEL g_hAccelTable;
		//int WND_WIDTH = 800;
		//int WND_HEIGHT = 800;
		int messageWndHeight;
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
		HMENU hMenu;
		HDC g_hDC;
		HWND g_hWndMessage;
		HDC g_hDCMessage;
		HINSTANCE g_hInstance; // current instance
		int framesPerRotation_;
		int sleepTimeMilliSec_;
		//int charHeight;
		//int charWidth;
		unsigned int rubiksCubeSize_;

		//int selMenuAnimationSpeed;
		//int selMenuRubiksCubeSize;

		

		RubiksCubeSolverScene scene_;
		RubiksCubeSolverTest tester_;

		std::thread renderingThread_;
		std::atomic<bool> renderNow_{ false };
		//No need to have it atomic variable. No need to have a lock.
		//The reading thread may read stale value which is OK. It will just have delayed responce.
		//std::atomic<bool> breakOperation_{ false };
		//std::mutex mutex_;
		bool resetRubiksCube_{ false }; 
		//int commandId_{ -1 };
		//first generation commands should be run independently. 
		//Second generation commands can be run while other first or second geenration commands are running.
		int commandGeneration_;
	};

}