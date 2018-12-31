#pragma once

#include <string>
using namespace std;

#include <gl\gl.h>
#include "Camera.h"
#include "RubiksCubeModel.h"

namespace mm {

	//forward declaration
	class RubiksCubeSolverUI;

	class RubiksCubeSolverScene
	{
	public:
		RubiksCubeSolverScene(RubiksCubeSolverUI& refUI, const string& modelName, int size);

		RubiksCubeSolverScene(const RubiksCubeSolverScene&) = delete;
		RubiksCubeSolverScene& operator=(const RubiksCubeSolverScene&) = delete;
		RubiksCubeSolverScene(RubiksCubeSolverScene&&) = delete;
		RubiksCubeSolverScene& operator=(RubiksCubeSolverScene&&) = delete;

		unique_ptr<RubiksCubeModel> replaceModelBy(const string& modelName, int size);
		unique_ptr<RubiksCubeModel> replaceModelBy(unique_ptr<RubiksCubeModel>&& newModel);

		/**/void initOpenGl(int nWidth, int nHeight);
		/**//**/void sizeOpenGlScreen(int nWidth, int nHeight);
		/**//**//**/void setFrustum(int nWidth, int nHeight);
		/**/void initScene();

		/**/void renderScene();
	
		INT getSelectedObjects(int x, int y, int nWidth, int nHeight);
		//void getCubeSelection(int *x, int *y, int *z, Face *face, int g_nHitCount);
		CVector3 mapCoordinates(int x, int y);

		void Reset();
		string getScramblingAlgo(int length);
		void applyAlgorithmToCube(const string& algo, bool animate);
		string Solve(int& solutionSteps, unsigned long long& duration, bool animate);
		string SolveOnCopy(int& solutionSteps, unsigned long long& duration);

	private:
		//const double CUBE_SIZE = 2.0;
		const float LINE_WIDTH = 2.0f;
		static const GLsizei SELECT_BUFFER_SIZE = 128;
		static GLuint g_pSelectBuffer[SELECT_BUFFER_SIZE];

	public:
		unique_ptr<RubiksCubeModel> rubicCubeModel_;
		CCamera g_cCamera;

		RubiksCubeSolverUI& refUI_;
	};

}