#pragma once

#include<string>
using namespace std;

namespace mm {

	class RubiksCubeSolverUI;

	class RubiksCubeSolver_v1
	{
	public:
		RubiksCubeSolver_v1(CRubiksCube& rubiksCube, bool animate = false, int steps = 0, RubiksCubeSolverUI* ui = nullptr);
		string solve(int& solutionSteps);

	private:
		void positionTheCube();
		void buildCross();
		void buildF2L();
		void buildOLL();
		void buildPLL();

		void buildCross_PlaceEdgePiece(const Color& targetColorFront, const Color& targetColorBottom);
		void buildF2L_PositionCornerPieces(const Color& targetColorFront, const Color& targetColorRight, const Color& targetColorBottom = Color::White);
		bool buildF2L_PositionEdgePieces(const Color& targetColorFront, const Color& targetColorRight);

	private:
		void applyAlgorithm(const string& step);

		CRubiksCube& rubiksCube_;
		string solution_;
		int solutionSteps_;

		bool animate_;
		int steps_;
		RubiksCubeSolverUI* ui_;
	};
}