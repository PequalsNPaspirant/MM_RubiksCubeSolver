#pragma once

#include <string>
using namespace std;

#include <gl\gl.h>
#include "Camera.h"
#include "RubiksCubeRenderingModel_v1.h"
#include "RubiksCubeRenderingModel_v2.h"

namespace mm {

	class RubiksCubeSolverScene
	{
	public:
		RubiksCubeSolverScene();

	private:
		RubiksCubeSolverScene(const RubiksCubeSolverScene&) = delete;
		RubiksCubeSolverScene& operator=(const RubiksCubeSolverScene&) = delete;
		RubiksCubeSolverScene(RubiksCubeSolverScene&&) = delete;
		RubiksCubeSolverScene& operator=(RubiksCubeSolverScene&&) = delete;

	public:
		/**/void initOpenGl(int nWidth, int nHeight);
		/**//**/void sizeOpenGlScreen(int nWidth, int nHeight);
		/**//**//**/void setFrustum(int nWidth, int nHeight);
		/**//**/void loadTexture(int nId, GLuint* texture);
		/**/void initScene();
		//void deInit();

	public:
		/**/void renderScene();
		//private:
	public:
		/**//**/void renderRubiksCube();
		/**//**//**/void renderIndividualCube(const Cube& pCube, int x, int y, int z);
		/**//**/void renderRubiksCube_v2();
		/**//**//**/void renderIndividualCube_v2(const Cube_v2& pCube, const Location_v2& location);
		/**//**//**//**/GLuint getTextureID(Color color);
	
	public:
		INT getSelectedObjects(int x, int y, int nWidth, int nHeight);
		void getCubeSelection(int *x, int *y, int *z, Face *face, int g_nHitCount);
		CVector3 mapCoordinates(int x, int y);

	private:
		//HGLRC g_hRC;
		//HDC g_hDC;
		GLuint g_pTextures[7];
		//HACCEL g_hAccelTable;

		//const int WND_WIDTH = 400;
		//const int WND_HEIGHT = 400;
		//const int SCREEN_DEPTH = 16;
		const double CUBE_SIZE = 2.0;
		const float LINE_WIDTH = 2.0f;

	public:
		CRubiksCube g_cCube;
		CRubiksCube_v2 g_cCube_v2;

		//bool g_bRotating;
		//bool g_bFlipRotation;
		//CVector3 g_vRotationAxis;
		//int g_nRotatingSection;
		//int g_nRotationAngle;

	private:
		static const GLsizei SELECT_BUFFER_SIZE = 128;
		static GLuint g_pSelectBuffer[SELECT_BUFFER_SIZE];

	public:
		//bool g_bMouseDown;
		CCamera g_cCamera;		
		//int g_nPrevX;
		//int g_nPrevY;
		
		//bool g_bFullScreen;
		//RECT g_rWnd;
		//CVector3 g_vMouseDown;
		//int g_nHitCount;
		//HCURSOR g_hArrow;
		//HCURSOR g_hHand;
		//HWND g_hWnd;
		//HINSTANCE g_hInstance;								// current instance
	};

}