#include "stdafx.h"
#include <iostream>

#include "RubiksCubeSolver_v1.h"
#include "RubiksCubeSimulator.h"
#include "Globals.h"
#include "CubeState.h"
#include "RubiksCubeModel_v1.h"

void RubiksCubeSolver_v1::solve()
{
	RubiksCubeSolver_v1::positionTheCube();
	RubiksCubeSolver_v1::buildCross();
	RubiksCubeSolver_v1::buildF2L();
	RubiksCubeSolver_v1::buildOLL();
	RubiksCubeSolver_v1::buildPLL();
}

bool isEdgeCube(CCubeState* currentCube, const CColor& first, const CColor& second)
{
	int firstCount = 0;
	int secondCount = 0;
	for (int i = 0; i < 6; ++i)
	{
		if (currentCube->GetFaceColor(Face(i)) == first)
			++firstCount;
		else if (currentCube->GetFaceColor(Face(i)) == second)
			++secondCount;
		else if (currentCube->GetFaceColor(Face(i)) != BLACK)
			return false;
	}
	
	return firstCount == 1 && secondCount == 1;
}
void buildCross_PlaceEdgePiece(const CColor& targetColorFront, const CColor& targetColorBottom)
{
	CCubeState* currentCube = nullptr;

	// Bring it from bottom later (y = 0) to top layer
	currentCube = g_cCube.GetCube(1, 0, 2);
	CColor c1 = currentCube->GetFaceColor(Face::Front);
	CColor c2 = currentCube->GetFaceColor(Face::Bottom);

	if (c1 == targetColorFront && c2 == targetColorBottom)
	{
		//Do nothing
	}
	if (c1 == targetColorBottom && c2 == targetColorFront)
	{
		RubiksCubeSimulator::executeAlgorithm("F2");
	}
	currentCube = g_cCube.GetCube(2, 0, 1);
	c1 = currentCube->GetFaceColor(Face::Right);
	c2 = currentCube->GetFaceColor(Face::Bottom);
	if((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
	{
		RubiksCubeSimulator::executeAlgorithm("R2");
	}
	currentCube = g_cCube.GetCube(1, 0, 0);
	c1 = currentCube->GetFaceColor(Face::Back);
	c2 = currentCube->GetFaceColor(Face::Bottom);
	if((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
	{
		RubiksCubeSimulator::executeAlgorithm("B2");
	}
	currentCube = g_cCube.GetCube(0, 0, 1);
	c1 = currentCube->GetFaceColor(Face::Left);
	c2 = currentCube->GetFaceColor(Face::Bottom);
	if((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
	{
		RubiksCubeSimulator::executeAlgorithm("L2");
		//RubiksCubeAlgoExecuter::executeAlgorithm("L'F'");
	}
	
	// Bring it from middle later (y = 1) to top layer
	currentCube = g_cCube.GetCube(0, 1, 0);
	c1 = currentCube->GetFaceColor(Face::Left);
	c2 = currentCube->GetFaceColor(Face::Back);
	if((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
	{
		RubiksCubeSimulator::executeAlgorithm("LU'L'");
	}
	currentCube = g_cCube.GetCube(0, 1, 2);
	c1 = currentCube->GetFaceColor(Face::Left);
	c2 = currentCube->GetFaceColor(Face::Front);
	if(c1 == targetColorBottom && c2 == targetColorFront)
	{
		RubiksCubeSimulator::executeAlgorithm("F'");
	}
	else if(c1 == targetColorFront && c2 == targetColorBottom)
	{
		RubiksCubeSimulator::executeAlgorithm("F");
	}
	currentCube = g_cCube.GetCube(2, 1, 2);
	c1 = currentCube->GetFaceColor(Face::Front);
	c2 = currentCube->GetFaceColor(Face::Right);
	if(c1 == targetColorFront && c2 == targetColorBottom)
	{
		RubiksCubeSimulator::executeAlgorithm("F");
	}
	else if (c1 == targetColorBottom && c2 == targetColorFront)
	{
		RubiksCubeSimulator::executeAlgorithm("F'");
	}
	currentCube = g_cCube.GetCube(2, 1, 0);
	c1 = currentCube->GetFaceColor(Face::Back);
	c2 = currentCube->GetFaceColor(Face::Right);
	if((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
	{
		RubiksCubeSimulator::executeAlgorithm("R'UR");
	}

	// Bring it from top later (y = 2) to bottom layer at appropriate position
	currentCube = g_cCube.GetCube(1, 2, 0);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Back);
	if(c1 == targetColorFront && c2 == targetColorBottom)
	{
		RubiksCubeSimulator::executeAlgorithm("B'R'URBF2");
	}
	else if (c1 == targetColorBottom && c2 == targetColorFront)
	{
		RubiksCubeSimulator::executeAlgorithm("U2F2");
	}

	currentCube = g_cCube.GetCube(0, 2, 1);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Left);
	if(c1 == targetColorFront && c2 == targetColorBottom)
	{
		//RubiksCubeAlgoExecuter::executeAlgorithm("LR'L'F2");
		RubiksCubeSimulator::executeAlgorithm("LF'L'");
	}
	else if (c1 == targetColorBottom && c2 == targetColorFront)
	{
		//RubiksCubeAlgoExecuter::executeAlgorithm("LF'L'");
		RubiksCubeSimulator::executeAlgorithm("U'F2");
	}

	currentCube = g_cCube.GetCube(1, 2, 2);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Front);
	if(c1 == targetColorFront && c2 == targetColorBottom)
	{
		RubiksCubeSimulator::executeAlgorithm("FRUR'F2");
	}
	else if (c1 == targetColorBottom && c2 == targetColorFront)
	{
		RubiksCubeSimulator::executeAlgorithm("F2");
	}
	currentCube = g_cCube.GetCube(2, 2, 1);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Right);
	if(c1 == targetColorFront && c2 == targetColorBottom)
	{
		RubiksCubeSimulator::executeAlgorithm("R'FR");
	}
	else if (c1 == targetColorBottom && c2 == targetColorFront)
	{
		RubiksCubeSimulator::executeAlgorithm("UF2");
	}
}

void RubiksCubeSolver_v1::positionTheCube()
{
	/*
	Make sure Cube is positioned in right way

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
	CCubeState* currentCube = nullptr;
	CColor c;

	// Check front face has blue center cube
	currentCube = g_cCube.GetCube(1, 1, 2);
	c = currentCube->GetFaceColor(Face::Front);
	if (c != BLUE)
	{
		if (c == GREEN)
			RubiksCubeSimulator::executeAlgorithm("Y2");
		else if (c == ORANGE)
			RubiksCubeSimulator::executeAlgorithm("Y'");
		else if (c == RED)
			RubiksCubeSimulator::executeAlgorithm("Y");
		else if (c == WHITE)
			RubiksCubeSimulator::executeAlgorithm("X");
		else if (c == YELLOW)
			RubiksCubeSimulator::executeAlgorithm("X'");
	}

	//Check right face
	// Do not disturb front face, so rotate around only z axis
	currentCube = g_cCube.GetCube(2, 1, 1);
	c = currentCube->GetFaceColor(Face::Right);
	if (c != RED)
	{
		if (c == ORANGE)
			RubiksCubeSimulator::executeAlgorithm("Z2");
		else if (c == GREEN)
			RubiksCubeSimulator::executeAlgorithm("Y");
		else if (c == BLUE)
			RubiksCubeSimulator::executeAlgorithm("Y'");
		else if (c == WHITE)
			RubiksCubeSimulator::executeAlgorithm("Z'");
		else if (c == YELLOW)
			RubiksCubeSimulator::executeAlgorithm("Z");
	}
	
	//Check top face
	currentCube = g_cCube.GetCube(1, 2, 1);
	c = currentCube->GetFaceColor(Face::Top);
	if (c != YELLOW)
	{
		if (c == WHITE)
			RubiksCubeSimulator::executeAlgorithm("X2");
		else if (c == GREEN)
			RubiksCubeSimulator::executeAlgorithm("X'");
		else if (c == BLUE)
			RubiksCubeSimulator::executeAlgorithm("X");
		else if (c == ORANGE)
			RubiksCubeSimulator::executeAlgorithm("Z");
		else if (c == RED)
			RubiksCubeSimulator::executeAlgorithm("Z'");
	}	
}

void RubiksCubeSolver_v1::buildCross()
{
	// Place blue-white at right position
	buildCross_PlaceEdgePiece(BLUE, WHITE);

	// Place red at right position
	RubiksCubeSimulator::executeAlgorithm("UY'D'");
	buildCross_PlaceEdgePiece(RED, WHITE);

	// Place green at right position
	RubiksCubeSimulator::executeAlgorithm("UY'D'");
	buildCross_PlaceEdgePiece(GREEN, WHITE);

	// Place orange at right position
	RubiksCubeSimulator::executeAlgorithm("UY'D'");
	buildCross_PlaceEdgePiece(ORANGE, WHITE);

	RubiksCubeSimulator::executeAlgorithm("UY'D'");
}

void buildF2L_PositionCornerPieces(const CColor& targetColorFront, const CColor& targetColorRight, const CColor& targetColorBottom = WHITE)
{
	CCubeState* currentCube = nullptr;
	CColor c1, c2, c3;

	// Check bottom layer and bring target to top layer at (2, 2, 2)
	currentCube = g_cCube.GetCube(0, 0, 0);
	c1 = currentCube->GetFaceColor(Face::Back);
	c2 = currentCube->GetFaceColor(Face::Left);
	c3 = currentCube->GetFaceColor(Face::Bottom);
	if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("LU2L'");


	currentCube = g_cCube.GetCube(0, 0, 2);
	c1 = currentCube->GetFaceColor(Face::Front);
	c2 = currentCube->GetFaceColor(Face::Left);
	c3 = currentCube->GetFaceColor(Face::Bottom);
	if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("FU'F'U'");

	currentCube = g_cCube.GetCube(2, 0, 2);
	c1 = currentCube->GetFaceColor(Face::Front);
	c2 = currentCube->GetFaceColor(Face::Right);
	c3 = currentCube->GetFaceColor(Face::Bottom);
	if (c1 == targetColorFront || c2 == targetColorRight || c3 == targetColorBottom)
	{
		//do nothing
	}
	else if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("F'UF");


	currentCube = g_cCube.GetCube(2, 0, 0);
	c1 = currentCube->GetFaceColor(Face::Back);
	c2 = currentCube->GetFaceColor(Face::Right);
	c3 = currentCube->GetFaceColor(Face::Bottom);
	if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("R'URU");

	// Check top layer and bring target to (2, 2, 2)
	currentCube = g_cCube.GetCube(0, 2, 0);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Left);
	c3 = currentCube->GetFaceColor(Face::Back);
	if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("U2");

	currentCube = g_cCube.GetCube(0, 2, 2);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Left);
	c3 = currentCube->GetFaceColor(Face::Front);
	if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("U'");

	currentCube = g_cCube.GetCube(2, 2, 0);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Back);
	c3 = currentCube->GetFaceColor(Face::Right);
	if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
		&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
		)
		RubiksCubeSimulator::executeAlgorithm("U");

	// Target is now in top layer at (2, 2, 2)
	currentCube = g_cCube.GetCube(2, 2, 2);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Front);
	c3 = currentCube->GetFaceColor(Face::Right);

	if(c1 == targetColorFront && c2 == targetColorBottom && c3 == targetColorRight)
		RubiksCubeSimulator::executeAlgorithm("F'U'F");
	else if(c1 == targetColorRight && c2 == targetColorFront && c3 == targetColorBottom)
		RubiksCubeSimulator::executeAlgorithm("RUR'");
	else if(c1 == targetColorBottom && c2 == targetColorRight && c3 == targetColorFront)
		RubiksCubeSimulator::executeAlgorithm("RUUR'U'RUR'");
	else
	{
		//assert
		int i = 0;
		++i;
	}
}

bool buildF2L_PositionEdgePieces(const CColor& targetColorFront, const CColor& targetColorRight)
{
	CCubeState* currentCube = nullptr;
	CColor c1, c2;
	bool retVal = true;
	string algo1("URU'R'U'F'UF");
	string algo2("U'F'UFURU'R'");

	//Check if aleady at position
	currentCube = g_cCube.GetCube(2, 1, 2);
	c1 = currentCube->GetFaceColor(Face::Front);
	c2 = currentCube->GetFaceColor(Face::Right);
	if (c1 == targetColorFront && c2 == targetColorRight)
		return true;
	else if(c1 == targetColorRight && c2 == targetColorFront) // If piece is stuck at right position but in wrong orientation
		RubiksCubeSimulator::executeAlgorithm(algo1);

	// Check top layer
	currentCube = g_cCube.GetCube(1, 2, 0);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Back);
	if ((c1 == targetColorFront || c2 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight))
		RubiksCubeSimulator::executeAlgorithm("U");

	currentCube = g_cCube.GetCube(0, 2, 1);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Left);
	if ((c1 == targetColorFront || c2 == targetColorFront)
		&& (c1 == targetColorRight || c2 == targetColorRight))
		RubiksCubeSimulator::executeAlgorithm("U'");

	
	currentCube = g_cCube.GetCube(1, 2, 2);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Front);
	if (c1 == targetColorRight && c2 == targetColorFront)
		RubiksCubeSimulator::executeAlgorithm(algo1);
	else if (c1 == targetColorFront && c2 == targetColorRight)
		RubiksCubeSimulator::executeAlgorithm("U'" + algo2);
	else
		retVal = false;

	if (retVal)
		return retVal;

	retVal = true;
	currentCube = g_cCube.GetCube(2, 2, 1);
	c1 = currentCube->GetFaceColor(Face::Top);
	c2 = currentCube->GetFaceColor(Face::Right);
	if (c1 == targetColorFront && c2 == targetColorRight)
		RubiksCubeSimulator::executeAlgorithm(algo2);
	else if (c1 == targetColorRight && c2 == targetColorFront)
		RubiksCubeSimulator::executeAlgorithm("U" + algo1);
	else
		retVal = false;

	//If we fail, check if any edge piece is stuck in second layer
	if (!retVal)
	{
		currentCube = g_cCube.GetCube(2, 1, 2);
		c1 = currentCube->GetFaceColor(Face::Front);
		c2 = currentCube->GetFaceColor(Face::Right);
		if(c1 != YELLOW && c2 != YELLOW)
			RubiksCubeSimulator::executeAlgorithm(algo1);
	}

	return retVal;
}

void RubiksCubeSolver_v1::buildF2L()
{
	//position corner pieces
	buildF2L_PositionCornerPieces(BLUE, RED, WHITE);

	RubiksCubeSimulator::executeAlgorithm("UY'D'");
	buildF2L_PositionCornerPieces(RED, GREEN, WHITE);

	RubiksCubeSimulator::executeAlgorithm("UY'D'");
	buildF2L_PositionCornerPieces(GREEN, ORANGE, WHITE);

	RubiksCubeSimulator::executeAlgorithm("UY'D'");
	buildF2L_PositionCornerPieces(ORANGE, BLUE, WHITE);

	RubiksCubeSimulator::executeAlgorithm("UY'D'");

	//position edge pieces
	int done = 0;
	while (done != 15)
	{
		if (buildF2L_PositionEdgePieces(BLUE, RED))
			done |= 1;

		RubiksCubeSimulator::executeAlgorithm("UY'D'");
		if (buildF2L_PositionEdgePieces(RED, GREEN))
			done |= 2;

		RubiksCubeSimulator::executeAlgorithm("UY'D'");
		if (buildF2L_PositionEdgePieces(GREEN, ORANGE))
			done |= 4;

		RubiksCubeSimulator::executeAlgorithm("UY'D'");
		if (buildF2L_PositionEdgePieces(ORANGE, BLUE))
			done |= 8;

		RubiksCubeSimulator::executeAlgorithm("UY'D'");
	}
}

void RubiksCubeSolver_v1::buildOLL()
{
	// Step 1

	while (true)
	{
		CCubeState* currentCube = nullptr;
		CColor c, c1, c2, c3, c4;
		string algo("FRUR'U'F'");

		/*
		Top Face
		*   c1  *
		c4  c   c2
		*   c3  *
		
		*/

		//Check if aleady at position
		currentCube = g_cCube.GetCube(1, 2, 1);
		c = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(1, 2, 0);
		c1 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 1);
		c2 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(1, 2, 2);
		c3 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(0, 2, 1);
		c4 = currentCube->GetFaceColor(Face::Top);
		if (c1 == YELLOW
			&& c2 == YELLOW
			&& c3 == YELLOW
			&& c4 == YELLOW) // We are sure that c is YELLOW
			break;

		if(c1 == YELLOW && c3 == YELLOW)
			RubiksCubeSimulator::executeAlgorithm("U");

		if (c2 == YELLOW && c4 == YELLOW)
		{
			RubiksCubeSimulator::executeAlgorithm(algo);
			continue;
		}

		if (c1 == YELLOW && c2 == YELLOW)
			RubiksCubeSimulator::executeAlgorithm("U'");
		else if(c2 == YELLOW && c3 == YELLOW)
			RubiksCubeSimulator::executeAlgorithm("U2");
		else if(c3 == YELLOW && c4 == YELLOW)
			RubiksCubeSimulator::executeAlgorithm("U");

		if (c1 == YELLOW && c4 == YELLOW)
		{
			RubiksCubeSimulator::executeAlgorithm(algo + algo);
			continue;
		}

		// Do the sequence once if none of above was executed
		RubiksCubeSimulator::executeAlgorithm(algo);
	}

	// Step 2
	while (true)
	{
		CCubeState* currentCube = nullptr;
		CColor c1, c2, c3, c4, c5, c6, c7, c8, c9;
		CColor s1, s2, s3, s4, s5, s6, s7, s8;
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
		currentCube = g_cCube.GetCube(0, 2, 0);
		c1 = currentCube->GetFaceColor(Face::Top);
		s1 = currentCube->GetFaceColor(Face::Left);
		s2 = currentCube->GetFaceColor(Face::Back);
		currentCube = g_cCube.GetCube(1, 2, 0);
		c2 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 0);
		c3 = currentCube->GetFaceColor(Face::Top);
		s3 = currentCube->GetFaceColor(Face::Back);
		s4 = currentCube->GetFaceColor(Face::Right);
		currentCube = g_cCube.GetCube(0, 2, 1);
		c4 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(1, 2, 1);
		c5 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 1);
		c6 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(0, 2, 2);
		c7 = currentCube->GetFaceColor(Face::Top);
		s7 = currentCube->GetFaceColor(Face::Front);
		s8 = currentCube->GetFaceColor(Face::Left);
		currentCube = g_cCube.GetCube(1, 2, 2);
		c8 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 2);
		c9 = currentCube->GetFaceColor(Face::Top);
		s5 = currentCube->GetFaceColor(Face::Right);
		s6 = currentCube->GetFaceColor(Face::Front);
		if (c1 == YELLOW
			&& c2 == YELLOW
			&& c3 == YELLOW
			&& c4 == YELLOW
			&& c5 == YELLOW
			&& c6 == YELLOW
			&& c7 == YELLOW
			&& c8 == YELLOW
			&& c9 == YELLOW)
			break;

		int numYellowCorners = 0;
		if (c1 == YELLOW)
			++numYellowCorners;
		if (c3 == YELLOW)
			++numYellowCorners;
		if (c7 == YELLOW)
			++numYellowCorners;
		if (c9 == YELLOW)
			++numYellowCorners;

		int* n1 = nullptr;
		switch (numYellowCorners)
		{
		case 0:
			if(s6 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U");
			else if(s4 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U2");
			else if(s2 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U'");
			else if(s8 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm(""); // do nothing
			else
			{
				int* n = nullptr;
				*n = 10; //we should not come here
			}
			break;
		case 1: // After this you will go to case 1 again or you would be done
			if(c9 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U");
			else if(c3 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U2");
			else if(c1 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U'");
			else if(c7 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm(""); // do nothing
			else
			{
				int* n = nullptr;
				*n = 10; //we should not come here
			}
			break;
		case 2: // After this you will go to case 1 
			if(s5 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U");
			else if(s3 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U2");
			else if(s2 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm("U'");
			else if(s7 == YELLOW)
				RubiksCubeSimulator::executeAlgorithm(""); // do nothing
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
		RubiksCubeSimulator::executeAlgorithm(algo);
	}
}

void RubiksCubeSolver_v1::buildPLL()
{
	//Step 1
	while (true)
	{
		CCubeState* currentCube = nullptr;
		//CColor c1, c2, c3, c4, c5, c6, c7, c8, c9;
		CColor s1, s2, s3, s4, s5, s6, s7, s8;
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
		currentCube = g_cCube.GetCube(0, 2, 0);
		//c1 = currentCube->GetFaceColor(Face::Top);
		s1 = currentCube->GetFaceColor(Face::Left);
		s2 = currentCube->GetFaceColor(Face::Back);
		//currentCube = g_cCube.GetCube(1, 2, 0);
		//c2 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 0);
		//c3 = currentCube->GetFaceColor(Face::Top);
		s3 = currentCube->GetFaceColor(Face::Back);
		s4 = currentCube->GetFaceColor(Face::Right);
		//currentCube = g_cCube.GetCube(0, 2, 1);
		//c4 = currentCube->GetFaceColor(Face::Top);
		//currentCube = g_cCube.GetCube(1, 2, 1);
		//c5 = currentCube->GetFaceColor(Face::Top);
		//currentCube = g_cCube.GetCube(2, 2, 1);
		//c6 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(0, 2, 2);
		//c7 = currentCube->GetFaceColor(Face::Top);
		s7 = currentCube->GetFaceColor(Face::Front);
		s8 = currentCube->GetFaceColor(Face::Left);
		//currentCube = g_cCube.GetCube(1, 2, 2);
		//c8 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 2);
		//c9 = currentCube->GetFaceColor(Face::Top);
		s5 = currentCube->GetFaceColor(Face::Right);
		s6 = currentCube->GetFaceColor(Face::Front);
		if (s2 == s3
			&& s4 == s5
			&& s6 == s7
			&& s8 == s1)
		{
			//Match centers and corners before proceeding
			//Get centers
			CColor o1, o2, o3, o4;
			currentCube = g_cCube.GetCube(1, 1, 0);
			o1 = currentCube->GetFaceColor(Face::Back);
			currentCube = g_cCube.GetCube(2, 1, 1);
			o2 = currentCube->GetFaceColor(Face::Right);
			currentCube = g_cCube.GetCube(1, 1, 2);
			o3 = currentCube->GetFaceColor(Face::Front);
			currentCube = g_cCube.GetCube(0, 1, 1);
			o4 = currentCube->GetFaceColor(Face::Left);

			if(o1 == s4)
				RubiksCubeSimulator::executeAlgorithm("U'");
			else if(o1 == s6)
				RubiksCubeSimulator::executeAlgorithm("U2");
			else if(o1 == s8)
				RubiksCubeSimulator::executeAlgorithm("U");

			break;
		}
		
		//Rotate the complete cube to set the face "having two corner piece color same" as front face
		if (s4 == s5)
			RubiksCubeSimulator::executeAlgorithm("UY'D'");
		else if (s2 == s3)
			RubiksCubeSimulator::executeAlgorithm("U2Y2D2");
		else if (s1 == s8)
			RubiksCubeSimulator::executeAlgorithm("U'YD");

		RubiksCubeSimulator::executeAlgorithm(algo);
	}

	//Step 2
	while (true)
	{
		CCubeState* currentCube = nullptr;
		//CColor c1, c2, c3, c4, c5, c6, c7, c8, c9;
		//CColor s1, s2, s3, s4, s5, s6, s7, s8;
		CColor e1, e2, e3, e4;
		CColor s4, s6, s8;
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
		CColor o1, o2, o3, o4;
		currentCube = g_cCube.GetCube(1, 1, 0);
		o1 = currentCube->GetFaceColor(Face::Back);
		currentCube = g_cCube.GetCube(2, 1, 1);
		o2 = currentCube->GetFaceColor(Face::Right);
		currentCube = g_cCube.GetCube(1, 1, 2);
		o3 = currentCube->GetFaceColor(Face::Front);
		currentCube = g_cCube.GetCube(0, 1, 1);
		o4 = currentCube->GetFaceColor(Face::Left);

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
		//currentCube = g_cCube.GetCube(0, 2, 0);
		//c1 = currentCube->GetFaceColor(Face::Top);
		//s1 = currentCube->GetFaceColor(Face::Left);
		//s2 = currentCube->GetFaceColor(Face::Back);
		currentCube = g_cCube.GetCube(1, 2, 0);
		//c2 = currentCube->GetFaceColor(Face::Top);
		e1 = currentCube->GetFaceColor(Face::Back);
		//currentCube = g_cCube.GetCube(2, 2, 0);
		//c3 = currentCube->GetFaceColor(Face::Top);
		//s3 = currentCube->GetFaceColor(Face::Back);
		//s4 = currentCube->GetFaceColor(Face::Right);
		currentCube = g_cCube.GetCube(0, 2, 1);
		//c4 = currentCube->GetFaceColor(Face::Top);
		e4 = currentCube->GetFaceColor(Face::Left);
		//currentCube = g_cCube.GetCube(1, 2, 1);
		//c5 = currentCube->GetFaceColor(Face::Top);
		currentCube = g_cCube.GetCube(2, 2, 1);
		//c6 = currentCube->GetFaceColor(Face::Top);
		e2 = currentCube->GetFaceColor(Face::Right);
		//currentCube = g_cCube.GetCube(0, 2, 2);
		//c7 = currentCube->GetFaceColor(Face::Top);
		//s7 = currentCube->GetFaceColor(Face::Front);
		//s8 = currentCube->GetFaceColor(Face::Left);
		currentCube = g_cCube.GetCube(1, 2, 2);
		//c8 = currentCube->GetFaceColor(Face::Top);
		e3 = currentCube->GetFaceColor(Face::Front);
		//currentCube = g_cCube.GetCube(2, 2, 2);
		//c9 = currentCube->GetFaceColor(Face::Top);
		//s5 = currentCube->GetFaceColor(Face::Right);
		//s6 = currentCube->GetFaceColor(Face::Front);
		
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
			RubiksCubeSimulator::executeAlgorithm("U'YD");
		else if (e3 == o3)
			RubiksCubeSimulator::executeAlgorithm("U2Y2D2");
		else if (e4 == o4)
			RubiksCubeSimulator::executeAlgorithm("UY'D'");

		RubiksCubeSimulator::executeAlgorithm(algo);
	}
}