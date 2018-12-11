#include <string>
using namespace std;

#include <gl\gl.h>
#include "Camera.h"
#include "RubiksCubeRenderingModel_v1.h"

namespace mm {

#define MAX_LOADSTRING 100
#define RC_CHANGED WM_APP + 100

	class Scene
	{
	public:
		//ensure single instance
		static Scene& getInstance();
		void createWindow(HINSTANCE hInstance);
	private:
		Scene();
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;

	private:
		/**/HWND createWindow(LPTSTR strTitle, int nWidth, int nHeight, DWORD dwStyle, BOOL bFullScreen, HINSTANCE hInstance);
		/**//**/bool changeToFullScreen();
		/**/void initOpenGl(HWND hWnd);
		/**//**/bool setupPixelFormat(HDC hdc);
		/**//**/void sizeOpenGlScreen(int nWidth, int nHeight);
		/**//**//**/void setFrustum(int nWidth, int nHeight);
		/**//**/void loadTexture(int nId, GLuint* texture);
		/**/void initScene();

	public:
		WPARAM enterMainLoop();
		/**/void renderScene();
		//private:
	public:
		/**//**/void renderRubiksCube();
		/**//**//**/void renderIndividualCube(CCubeState* pCube, int x, int y, int z);
		/**//**//**//**/GLuint getTextureID(CColor color);

	public:
		static LRESULT CALLBACK WndProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK AboutProcCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
		//private:
	public:
		void deInit();
		INT getSelectedObjects(int x, int y, GLsizei buffSize, GLuint* selectBuffer);
		void getCubeSelection(int *x, int *y, int *z, Face *face);
		CVector3 mapCoordinates(int x, int y);

	private:
		HGLRC g_hRC;
		HDC g_hDC;
		TCHAR g_szWindowClass[MAX_LOADSTRING];				// the main window class name
		GLuint g_pTextures[7];
		HACCEL g_hAccelTable;

		const int WND_WIDTH = 400;
		const int WND_HEIGHT = 400;
		const int SCREEN_DEPTH = 16;
		const double CUBE_SIZE = 2.0;
		const float LINE_WIDTH = 2.0f;

	private:
		/* void Cls_OnMouseLeave(HWND hWnd, int x, int y, UINT keyFlags) */
#define HANDLE_WM_MOUSELEAVE(hWnd, wParam, lParam, fn) \
    ((fn)((hWnd)), 0L)
#define HANDLE_RC_CHANGED(hWnd, wParam, lParam, fn) \
	((fn)((hWnd)), 0L)

		static void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
		static void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
		static void OnDestroy(HWND hWnd);
		static void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);
		static void OnMouseWheel(HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys);
		static void OnSize(HWND hWnd, UINT state, int cx, int cy);
		static void OnMouseLeave(HWND hWnd);
		static void OnRubiksCubeChanged(HWND hWnd);
		static void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

		static void Scramble(const string& str);
		static void Solve();

	public:
		CRubiksCube g_cCube;
		BOOL g_bRotating;
		BOOL g_bFlipRotation;
		CVector3 g_vRotationAxis;
		int g_nRotatingSection;
		int g_nRotationAngle;

	private:
		static const GLsizei SELECT_BUFFER_SIZE = 128;
		static GLuint g_pSelectBuffer[SELECT_BUFFER_SIZE];

		bool g_bMouseDown;
		CCamera g_cCamera;		
		int g_nPrevX;
		int g_nPrevY;
		
		bool g_bFullScreen;
		RECT g_rWnd;
		CVector3 g_vMouseDown;
		int g_nHitCount;
		HCURSOR g_hArrow;
		HCURSOR g_hHand;
		TCHAR g_szTitle[MAX_LOADSTRING];					// The title bar text
		HWND g_hWnd;
		HINSTANCE g_hInstance;								// current instance
	};

}