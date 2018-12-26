#include "stdafx.h"
#include <iostream>
#include <cassert>

//#include "RubiksCubeSolverUI.h"
#include "RubiksCubeRenderingModel_v1.h"
#include "RubiksCubeRenderingModel_v2.h"
#include "RubiksCubeSolver_v2.h"
#include "RubiksCubeSimulator.h"
//#include "Globals.h"


namespace mm {

	RubiksCubeSolver_v2::RubiksCubeSolver_v2(CRubiksCube_v2& rubiksCube, bool animate /*= false*/, int steps /*= 0*/, RubiksCubeSolverUI* ui /*= nullptr*/)
		: rubiksCube_(rubiksCube),
		solutionSteps_(0),
		animate_(animate),
		steps_(steps),
		ui_(ui)
	{
	}

	string RubiksCubeSolver_v2::solve(int& solutionSteps)
	{
		solutionSteps_ = 0;
		solution_ = "";

		RubiksCubeSolver_v2::positionTheCube();
		RubiksCubeSolver_v2::buildCross();
		RubiksCubeSolver_v2::buildF2L();
		RubiksCubeSolver_v2::buildOLL();
		RubiksCubeSolver_v2::buildPLL();

		//verify
		assert(rubiksCube_.IsSolved());

		solutionSteps = solutionSteps_;
		return solution_;
	}

	void RubiksCubeSolver_v2::applyAlgorithm(const string& step)
	{
		solution_ += step;
		solutionSteps_ += rubiksCube_.applyAlgorithm(step, animate_, steps_, ui_);
	}

	bool isEdgeCube(const Cube_v2& currentCube, const Color& first, const Color& second)
	{
		int firstCount = 0;
		int secondCount = 0;
		for (int i = 0; i < 6; ++i)
		{
			if (currentCube.GetFaceColor(Face(i)) == first)
				++firstCount;
			else if (currentCube.GetFaceColor(Face(i)) == second)
				++secondCount;
			else if (currentCube.GetFaceColor(Face(i)) != Color::Black)
				return false;
		}

		return firstCount == 1 && secondCount == 1;
	}

	void RubiksCubeSolver_v2::buildCross_PlaceEdgePiece(const Color& targetColorFront, const Color& targetColorBottom)
	{
		//Cube_v2* currentCube = nullptr;

		// Bring it from bottom later (y = 0) to top layer
		Cube_v2 currentCube = rubiksCube_.GetCube(1, 0, 2);
		Color c1 = currentCube.GetFaceColor(Face::Front);
		Color c2 = currentCube.GetFaceColor(Face::Down);

		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			//Do nothing
		}
		if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("FF");
		}
		currentCube = rubiksCube_.GetCube(2, 0, 1);
		c1 = currentCube.GetFaceColor(Face::Right);
		c2 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("RR");
		}
		currentCube = rubiksCube_.GetCube(1, 0, 0);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("BB");
		}
		currentCube = rubiksCube_.GetCube(0, 0, 1);
		c1 = currentCube.GetFaceColor(Face::Left);
		c2 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("LL");
			//RubiksCubeAlgoExecuter::executeAlgorithm("L'F'");
		}

		// Bring it from middle later (y = 1) to top layer
		currentCube = rubiksCube_.GetCube(0, 1, 0);
		c1 = currentCube.GetFaceColor(Face::Left);
		c2 = currentCube.GetFaceColor(Face::Back);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("LU'L'");
		}
		currentCube = rubiksCube_.GetCube(0, 1, 2);
		c1 = currentCube.GetFaceColor(Face::Left);
		c2 = currentCube.GetFaceColor(Face::Front);
		if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("F'");	
		}
		else if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("F");	
		}
		currentCube = rubiksCube_.GetCube(2, 1, 2);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Right);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("F");	
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("F'");	
		}
		currentCube = rubiksCube_.GetCube(2, 1, 0);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Right);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("R'UR");		
		}

		// Bring it from top later (y = 2) to bottom layer at appropriate position
		currentCube = rubiksCube_.GetCube(1, 2, 0);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Back);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("B'R'URBFF");	
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("UUFF");	
		}

		currentCube = rubiksCube_.GetCube(0, 2, 1);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			//RubiksCubeAlgoExecuter::executeAlgorithm("LR'L'F2");
			applyAlgorithm("LF'L'");	
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			//RubiksCubeAlgoExecuter::executeAlgorithm("LF'L'");
			applyAlgorithm("U'FF");	
		}

		currentCube = rubiksCube_.GetCube(1, 2, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Front);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("FRUR'FF");	
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("FF");	
		}
		currentCube = rubiksCube_.GetCube(2, 2, 1);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Right);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("R'FR");
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("UFF");
		}
	}

	void RubiksCubeSolver_v2::positionTheCube()
	{
		/*
		Make sure Cube_v2 is positioned in right way

		// Z-axis : Back -> Front // green  -> blue
		// X-axis : Left -> Right // orange -> red
		// Y-axis : Down -> Up    // white  -> yellow

			yellow
			  Y
			  |
			  . --> X red
			 /
			Z
		 blue
		*/

		// At the most 3 out of below 3 if clauses are executed
		Cube_v2 currentCube;
		Color c;

		// Check front face has blue center cube
		currentCube = rubiksCube_.GetCube(1, 1, 2);
		c = currentCube.GetFaceColor(Face::Front);
		if (c != Color::Blue)
		{
			if (c == Color::Green)
				applyAlgorithm("YY");
			else if (c == Color::Orange)
				applyAlgorithm("Y'");
			else if (c == Color::Red)
				applyAlgorithm("Y");
			else if (c == Color::White)
				applyAlgorithm("X");
			else if (c == Color::Yellow)
				applyAlgorithm("X'");
		}

		//Check right face
		// Do not disturb front face, so rotate around only z axis
		currentCube = rubiksCube_.GetCube(2, 1, 1);
		c = currentCube.GetFaceColor(Face::Right);
		if (c != Color::Red)
		{
			if (c == Color::Orange)
				applyAlgorithm("ZZ");
			else if (c == Color::Green)
				applyAlgorithm("Y");
			else if (c == Color::Blue)
				applyAlgorithm("Y'");
			else if (c == Color::White)
				applyAlgorithm("Z'");
			else if (c == Color::Yellow)
				applyAlgorithm("Z");
		}

		//Check top face
		currentCube = rubiksCube_.GetCube(1, 2, 1);
		c = currentCube.GetFaceColor(Face::Up);
		if (c != Color::Yellow)
		{
			if (c == Color::White)
				applyAlgorithm("XX");
			else if (c == Color::Green)
				applyAlgorithm("X'");
			else if (c == Color::Blue)
				applyAlgorithm("X");
			else if (c == Color::Orange)
				applyAlgorithm("Z");
			else if (c == Color::Red)
				applyAlgorithm("Z'");	
		}
	}

	void RubiksCubeSolver_v2::buildCross()
	{
		// Place blue-white at right position
		buildCross_PlaceEdgePiece(Color::Blue, Color::White);

		// Place red at right position
		applyAlgorithm("Y'");
		buildCross_PlaceEdgePiece(Color::Red, Color::White);

		// Place green at right position
		applyAlgorithm("Y'");
		buildCross_PlaceEdgePiece(Color::Green, Color::White);

		// Place orange at right position
		applyAlgorithm("Y'");
		buildCross_PlaceEdgePiece(Color::Orange, Color::White);

		applyAlgorithm("Y'");
	}

	void RubiksCubeSolver_v2::buildF2L_PositionCornerPieces(const Color& targetColorFront, const Color& targetColorRight, const Color& targetColorBottom /*= Color::White*/)
	{
		Cube_v2 currentCube;
		Color c1, c2, c3;

		// Check bottom layer and bring target to top layer at (2, 2, 2)
		currentCube = rubiksCube_.GetCube(0, 0, 0);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("LUUL'");


		currentCube = rubiksCube_.GetCube(0, 0, 2);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("FU'F'U'");

		currentCube = rubiksCube_.GetCube(2, 0, 2);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Right);
		c3 = currentCube.GetFaceColor(Face::Down);
		if (c1 == targetColorFront || c2 == targetColorRight || c3 == targetColorBottom)
		{
			//do nothing
		}
		else if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("F'UF");


		currentCube = rubiksCube_.GetCube(2, 0, 0);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Right);
		c3 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("R'URU");

		// Check top layer and bring target to (2, 2, 2)
		currentCube = rubiksCube_.GetCube(0, 2, 0);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Back);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("UU");

		currentCube = rubiksCube_.GetCube(0, 2, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Front);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("U'");

		currentCube = rubiksCube_.GetCube(2, 2, 0);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Back);
		c3 = currentCube.GetFaceColor(Face::Right);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("U");

		// Target is now in top layer at (2, 2, 2)
		currentCube = rubiksCube_.GetCube(2, 2, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Front);
		c3 = currentCube.GetFaceColor(Face::Right);

		if (c1 == targetColorFront && c2 == targetColorBottom && c3 == targetColorRight)
			applyAlgorithm("F'U'F");
		else if (c1 == targetColorRight && c2 == targetColorFront && c3 == targetColorBottom)
			applyAlgorithm("RUR'");
		else if (c1 == targetColorBottom && c2 == targetColorRight && c3 == targetColorFront)
			applyAlgorithm("RUUR'U'RUR'");
		else
		{
			//assert
			int i = 0;
			++i;
		}
	}

	bool RubiksCubeSolver_v2::buildF2L_PositionEdgePieces(const Color& targetColorFront, const Color& targetColorRight)
	{
		Cube_v2 currentCube;
		Color c1, c2;
		bool retVal = true;
		string algo1("URU'R'U'F'UF");
		string algo2("U'F'UFURU'R'");

		//Check if aleady at position
		currentCube = rubiksCube_.GetCube(2, 1, 2);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Right);
		if (c1 == targetColorFront && c2 == targetColorRight)
			return true;
		else if (c1 == targetColorRight && c2 == targetColorFront) // If piece is stuck at right position but in wrong orientation
			applyAlgorithm(algo1);

		// Check top layer
		currentCube = rubiksCube_.GetCube(1, 2, 0);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Back);
		if ((c1 == targetColorFront || c2 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight))
			applyAlgorithm("U");

		currentCube = rubiksCube_.GetCube(0, 2, 1);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		if ((c1 == targetColorFront || c2 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight))
			applyAlgorithm("U'");


		currentCube = rubiksCube_.GetCube(1, 2, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Front);
		if (c1 == targetColorRight && c2 == targetColorFront)
			applyAlgorithm(algo1);

		else if (c1 == targetColorFront && c2 == targetColorRight)
			applyAlgorithm("U'" + algo2);
		else
			retVal = false;

		if (retVal)
			return retVal;

		retVal = true;
		currentCube = rubiksCube_.GetCube(2, 2, 1);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Right);
		if (c1 == targetColorFront && c2 == targetColorRight)
			applyAlgorithm(algo2);
		else if (c1 == targetColorRight && c2 == targetColorFront)
			applyAlgorithm("U" + algo1);
		else
			retVal = false;

		//If we fail, check if any edge piece is stuck in second layer
		if (!retVal)
		{
			currentCube = rubiksCube_.GetCube(2, 1, 2);
			c1 = currentCube.GetFaceColor(Face::Front);
			c2 = currentCube.GetFaceColor(Face::Right);
			if (c1 != Color::Yellow && c2 != Color::Yellow)
				applyAlgorithm(algo1);
		}

		return retVal;
	}

	void RubiksCubeSolver_v2::buildF2L()
	{
		//position corner pieces
		buildF2L_PositionCornerPieces(Color::Blue, Color::Red, Color::White);

		applyAlgorithm("Y'");
		buildF2L_PositionCornerPieces(Color::Red, Color::Green, Color::White);

		applyAlgorithm("Y'");
		buildF2L_PositionCornerPieces(Color::Green, Color::Orange, Color::White);

		applyAlgorithm("Y'");
		buildF2L_PositionCornerPieces(Color::Orange, Color::Blue, Color::White);

		applyAlgorithm("Y'");

		//position edge pieces
		int numIterations = 0;
		int done = 0;
		while (done != 15)
		{
			++numIterations;

			if (buildF2L_PositionEdgePieces(Color::Blue, Color::Red))
				done |= 1;

			applyAlgorithm("Y'");
			if (buildF2L_PositionEdgePieces(Color::Red, Color::Green))
				done |= 2;

			applyAlgorithm("Y'");
			if (buildF2L_PositionEdgePieces(Color::Green, Color::Orange))
				done |= 4;

			applyAlgorithm("Y'");
			if (buildF2L_PositionEdgePieces(Color::Orange, Color::Blue))
				done |= 8;

			applyAlgorithm("Y'");
		}
	}

	void RubiksCubeSolver_v2::buildOLL()
	{
		// Step 1

		while (true)
		{
			Cube_v2 currentCube;
			Color c, c1, c2, c3, c4;
			string algo("FRUR'U'F'");

			/*
			Top Face
			*   c1  *
			c4  c   c2
			*   c3  *

			*/

			//Check if aleady at position
			currentCube = rubiksCube_.GetCube(1, 2, 1);
			c = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(1, 2, 0);
			c1 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 1);
			c2 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(1, 2, 2);
			c3 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(0, 2, 1);
			c4 = currentCube.GetFaceColor(Face::Up);
			if (c1 == Color::Yellow
				&& c2 == Color::Yellow
				&& c3 == Color::Yellow
				&& c4 == Color::Yellow) // We are sure that c is Color::Yellow
				break;

			if (c1 == Color::Yellow && c3 == Color::Yellow)
				applyAlgorithm("U");

			if (c2 == Color::Yellow && c4 == Color::Yellow)
			{
				applyAlgorithm(algo);
				continue;
			}

			if (c1 == Color::Yellow && c2 == Color::Yellow)
				applyAlgorithm("U'");
			else if (c2 == Color::Yellow && c3 == Color::Yellow)
				applyAlgorithm("UU");
			else if (c3 == Color::Yellow && c4 == Color::Yellow)
				applyAlgorithm("U");

			if (c1 == Color::Yellow && c4 == Color::Yellow)
			{
				applyAlgorithm(algo + algo);
				continue;
			}

			// Do the sequence once if none of above was executed
			applyAlgorithm(algo);
		}

		// Step 2
		while (true)
		{
			Cube_v2 currentCube;
			Color c1, c2, c3, c4, c5, c6, c7, c8, c9;
			Color s1, s2, s3, s4, s5, s6, s7, s8;
			string algo("RUR'URUUR'");

			/*
			Top Face
			s2      s3
		 s1 c1  c2  c3 s4
			c4  c5  c6
		 s8 c7  c8  c9 s5
			s7      s6
			*/

			//Check if aleady at position
			currentCube = rubiksCube_.GetCube(0, 2, 0);
			c1 = currentCube.GetFaceColor(Face::Up);
			s1 = currentCube.GetFaceColor(Face::Left);
			s2 = currentCube.GetFaceColor(Face::Back);
			currentCube = rubiksCube_.GetCube(1, 2, 0);
			c2 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 0);
			c3 = currentCube.GetFaceColor(Face::Up);
			s3 = currentCube.GetFaceColor(Face::Back);
			s4 = currentCube.GetFaceColor(Face::Right);
			currentCube = rubiksCube_.GetCube(0, 2, 1);
			c4 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(1, 2, 1);
			c5 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 1);
			c6 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(0, 2, 2);
			c7 = currentCube.GetFaceColor(Face::Up);
			s7 = currentCube.GetFaceColor(Face::Front);
			s8 = currentCube.GetFaceColor(Face::Left);
			currentCube = rubiksCube_.GetCube(1, 2, 2);
			c8 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 2);
			c9 = currentCube.GetFaceColor(Face::Up);
			s5 = currentCube.GetFaceColor(Face::Right);
			s6 = currentCube.GetFaceColor(Face::Front);
			if (c1 == Color::Yellow
				&& c2 == Color::Yellow
				&& c3 == Color::Yellow
				&& c4 == Color::Yellow
				&& c5 == Color::Yellow
				&& c6 == Color::Yellow
				&& c7 == Color::Yellow
				&& c8 == Color::Yellow
				&& c9 == Color::Yellow)
				break;

			int numYellowCorners = 0;
			if (c1 == Color::Yellow)
				++numYellowCorners;
			if (c3 == Color::Yellow)
				++numYellowCorners;
			if (c7 == Color::Yellow)
				++numYellowCorners;
			if (c9 == Color::Yellow)
				++numYellowCorners;

			int* n1 = nullptr;
			switch (numYellowCorners)
			{
			case 0:
				if (s6 == Color::Yellow)
					applyAlgorithm("U");
				else if (s4 == Color::Yellow)
					applyAlgorithm("UU");
				else if (s2 == Color::Yellow)
					applyAlgorithm("U'");
				else if (s8 == Color::Yellow)
					applyAlgorithm(""); // do nothing
				else
				{
					int* n = nullptr;
					*n = 10; //we should not come here
				}
				break;
			case 1: // After this you will go to case 1 again or you would be done
				if (c9 == Color::Yellow)
					applyAlgorithm("U");
				else if (c3 == Color::Yellow)
					applyAlgorithm("UU");
				else if (c1 == Color::Yellow)
					applyAlgorithm("U'");
				else if (c7 == Color::Yellow)
					applyAlgorithm(""); // do nothing
				else
				{
					int* n = nullptr;
					*n = 10; //we should not come here
				}
				break;
			case 2: // After this you will go to case 1 
				if (s5 == Color::Yellow)
					applyAlgorithm("U");
				else if (s3 == Color::Yellow)
					applyAlgorithm("UU");
				else if (s2 == Color::Yellow)
					applyAlgorithm("U'");
				else if (s7 == Color::Yellow)
					applyAlgorithm(""); // do nothing
				else
				{
					int* n = nullptr;
					*n = 10; //we should not come here
				}
				break;
			case 3:
				*n1 = 10; //we should not come here
				break;
			default:
				*n1 = 10; //we should not come here
				break;
			}

			// Do the sequence once and continue
			applyAlgorithm(algo);
		}
	}

	void RubiksCubeSolver_v2::buildPLL()
	{
		//Step 1
		while (true)
		{
			Cube_v2 currentCube;
			//Color c1, c2, c3, c4, c5, c6, c7, c8, c9;
			Color s1, s2, s3, s4, s5, s6, s7, s8;
			string algo("RB'RFFR'BRFFRR");

			/*
			Top Face
			s2  o1  s3
		 s1 c1  c2  c3 s4
		 o4	c4  c5  c6 o2
		 s8 c7  c8  c9 s5
			s7  o3  s6
			*/

			//Check if aleady at position
			currentCube = rubiksCube_.GetCube(0, 2, 0);
			//c1 = currentCube.GetFaceColor(Face::Up);
			s1 = currentCube.GetFaceColor(Face::Left);
			s2 = currentCube.GetFaceColor(Face::Back);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 0);
			//c2 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 0);
			//c3 = currentCube.GetFaceColor(Face::Up);
			s3 = currentCube.GetFaceColor(Face::Back);
			s4 = currentCube.GetFaceColor(Face::Right);
			//currentCube = Scene::getInstance().g_cCube.GetCube(0, 2, 1);
			//c4 = currentCube.GetFaceColor(Face::Up);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 1);
			//c5 = currentCube.GetFaceColor(Face::Up);
			//currentCube = Scene::getInstance().g_cCube.GetCube(2, 2, 1);
			//c6 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(0, 2, 2);
			//c7 = currentCube.GetFaceColor(Face::Up);
			s7 = currentCube.GetFaceColor(Face::Front);
			s8 = currentCube.GetFaceColor(Face::Left);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 2);
			//c8 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 2);
			//c9 = currentCube.GetFaceColor(Face::Up);
			s5 = currentCube.GetFaceColor(Face::Right);
			s6 = currentCube.GetFaceColor(Face::Front);
			if (s2 == s3
				&& s4 == s5
				&& s6 == s7
				&& s8 == s1)
			{
				//Match centers and corners before proceeding
				//Get centers
				Color o1, o2, o3, o4;
				currentCube = rubiksCube_.GetCube(1, 1, 0);
				o1 = currentCube.GetFaceColor(Face::Back);
				currentCube = rubiksCube_.GetCube(2, 1, 1);
				o2 = currentCube.GetFaceColor(Face::Right);
				currentCube = rubiksCube_.GetCube(1, 1, 2);
				o3 = currentCube.GetFaceColor(Face::Front);
				currentCube = rubiksCube_.GetCube(0, 1, 1);
				o4 = currentCube.GetFaceColor(Face::Left);

				if (o1 == s4)
					applyAlgorithm("U'");
				else if (o1 == s6)
					applyAlgorithm("UU");
				else if (o1 == s8)
					applyAlgorithm("U");

				break;
			}

			//Rotate the complete cube to set the face "having two corner piece color same" as front face
			if (s4 == s5)
				applyAlgorithm("Y'");
			else if (s2 == s3)
				applyAlgorithm("UUYYDD");
			else if (s1 == s8)
				applyAlgorithm("U'YD");

			applyAlgorithm(algo);
		}

		//Step 2
		while (true)
		{
			Cube_v2 currentCube;
			//Color c1, c2, c3, c4, c5, c6, c7, c8, c9;
			//Color s1, s2, s3, s4, s5, s6, s7, s8;
			Color e1, e2, e3, e4;
			Color s4, s6, s8;
			string algo("RU'RURURU'R'U'RR");

			//Get centers
			/*
				o1
				e1
			c1  c2  c3
	 o4  e4	c4  c5  c6 e2 o2
			c7  c8  c9
				e3
				o3
			*/
			Color o1, o2, o3, o4;
			currentCube = rubiksCube_.GetCube(1, 1, 0);
			o1 = currentCube.GetFaceColor(Face::Back);
			currentCube = rubiksCube_.GetCube(2, 1, 1);
			o2 = currentCube.GetFaceColor(Face::Right);
			currentCube = rubiksCube_.GetCube(1, 1, 2);
			o3 = currentCube.GetFaceColor(Face::Front);
			currentCube = rubiksCube_.GetCube(0, 1, 1);
			o4 = currentCube.GetFaceColor(Face::Left);

			/*
			Top Face
				o1
				e1
			c1  c2  c3
	 o4  e4	c4  c5  c6 e2 o2
			c7  c8  c9
				e3
				o3
			*/

			//Check if aleady at position
			//currentCube = Scene::getInstance().g_cCube.GetCube(0, 2, 0);
			//c1 = currentCube.GetFaceColor(Face::Up);
			//s1 = currentCube.GetFaceColor(Face::Left);
			//s2 = currentCube.GetFaceColor(Face::Back);
			currentCube = rubiksCube_.GetCube(1, 2, 0);
			//c2 = currentCube.GetFaceColor(Face::Up);
			e1 = currentCube.GetFaceColor(Face::Back);
			//currentCube = Scene::getInstance().g_cCube.GetCube(2, 2, 0);
			//c3 = currentCube.GetFaceColor(Face::Up);
			//s3 = currentCube.GetFaceColor(Face::Back);
			//s4 = currentCube.GetFaceColor(Face::Right);
			currentCube = rubiksCube_.GetCube(0, 2, 1);
			//c4 = currentCube.GetFaceColor(Face::Up);
			e4 = currentCube.GetFaceColor(Face::Left);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 1);
			//c5 = currentCube.GetFaceColor(Face::Up);
			currentCube = rubiksCube_.GetCube(2, 2, 1);
			//c6 = currentCube.GetFaceColor(Face::Up);
			e2 = currentCube.GetFaceColor(Face::Right);
			//currentCube = Scene::getInstance().g_cCube.GetCube(0, 2, 2);
			//c7 = currentCube.GetFaceColor(Face::Up);
			//s7 = currentCube.GetFaceColor(Face::Front);
			//s8 = currentCube.GetFaceColor(Face::Left);
			currentCube = rubiksCube_.GetCube(1, 2, 2);
			//c8 = currentCube.GetFaceColor(Face::Up);
			e3 = currentCube.GetFaceColor(Face::Front);
			//currentCube = Scene::getInstance().g_cCube.GetCube(2, 2, 2);
			//c9 = currentCube.GetFaceColor(Face::Up);
			//s5 = currentCube.GetFaceColor(Face::Right);
			//s6 = currentCube.GetFaceColor(Face::Front);

			//Match centers with corners, they may be misaligned after few iterations of last algo
			//if(o1 == s4)
			//	RubiksCubeAlgoExecuter::executeAlgorithm("U'");
			//else if(o1 == s6)
			//	RubiksCubeAlgoExecuter::executeAlgorithm("U2");
			//else if(o1 == s8)
			//	RubiksCubeAlgoExecuter::executeAlgorithm("U");

			if (e1 == o1
				&& e2 == o2
				&& e3 == o3
				&& e4 == o4)
			{
				break;
			}

			if (e2 == o2)
				applyAlgorithm("U'YD");
			else if (e3 == o3)
				applyAlgorithm("UUYYDD");
			else if (e4 == o4)
				applyAlgorithm("Y'");

			applyAlgorithm(algo);
		}
	}

}