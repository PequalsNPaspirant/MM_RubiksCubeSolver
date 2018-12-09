#include "stdafx.h"
#include <iostream>

#include "RubiksCubeSimulator.h"
#include "Globals.h"

VOID RenderScene();
//VOID RenderRubiksCube();

void RubiksCubeSimulator::test()
{
	//RubiksCubeAlgoExecuter::executeAlgorithm("FB");
	//RubiksCubeAlgoExecuter::executeAlgorithm("LR");
	//RubiksCubeAlgoExecuter::executeAlgorithm("UD");

	//RubiksCubeAlgoExecuter::executeAlgorithm("F'B'");
	//RubiksCubeAlgoExecuter::executeAlgorithm("L'R'");
	//RubiksCubeAlgoExecuter::executeAlgorithm("U'D'");

	//RubiksCubeAlgoExecuter::executeAlgorithm("F2B2");
	//RubiksCubeAlgoExecuter::executeAlgorithm("L2R2");
	//RubiksCubeAlgoExecuter::executeAlgorithm("U2D2");

	//RubiksCubeAlgoExecuter::executeAlgorithm("D F' R L' F L D' B' U' F R' F' U L' U2 R L B");
	//RubiksCubeAlgoExecuter::executeAlgorithm("B' L' R' U2 L U' F R F' U B D L' F' L R' F D'");
}

void RubiksCubeSimulator::scramble(const string& str)
{
	//RubiksCubeAlgoExecuter::executeAlgorithm("D F' R L' F L D' B' U' F R' F' U L' U2 R L B");
	RubiksCubeSimulator::executeAlgorithm(str);
	
	// testing permutations in bottom layer
	//RubiksCubeAlgoExecuter::executeAlgorithm("D");
	//RubiksCubeAlgoExecuter::executeAlgorithm("D2");
	//RubiksCubeAlgoExecuter::executeAlgorithm("D'");

	// testing permutations in middle layer
	//RubiksCubeAlgoExecuter::executeAlgorithm("F'");
	//RubiksCubeAlgoExecuter::executeAlgorithm("F");
	//RubiksCubeAlgoExecuter::executeAlgorithm("F'R2");

	// testing permutations in top layer

	//cout << "\nscrambled...";
	Sleep(500);
}

void RubiksCubeSimulator::executeAlgorithm(const string& algorithm)
{
	g_bRotating = TRUE;
	g_bFlipRotation = FALSE;

	for (int i = 0; i < algorithm.length(); ++i)
	{
		char face = algorithm[i];
		if (face == ' ')
			continue;

		if(face >= 'a')
			face = face - 32; // Convert into Upper case char

		// Check if prime operation
		bool isPrime = false;
		int nextCharIndex = i + 1;
		if (nextCharIndex < algorithm.length() && algorithm[nextCharIndex] == '\'')
		{
			isPrime = true;
			++i;
		}
		// Check if multiple rotations
		nextCharIndex = i + 1;
		int numRotations = 1;
		if(nextCharIndex < algorithm.length() && '0' <= algorithm[nextCharIndex] && algorithm[nextCharIndex] <= '9')
		{
			numRotations = algorithm[i + 1] - '0';
			++i;
		}

		executeAlgorithm(face, isPrime, numRotations);
	}
	//g_bRotating = FALSE; // TODO: Find why this resets the cube
}

void RubiksCubeSimulator::executeAlgorithm(const char& face, bool isPrime, int numRotations)
{
	//cout << "\nApplying move: " << face;
	//if(isPrime)
	//	cout << "\'";
	//if(numRotations > 1)
	//	cout << numRotations;
	
	// Z-axis : Back -> Front // green  -> blue
	// X-axis : Left -> Right // orange -> red
	// Y-axis : Down -> Up    // white  -> yellow
	/*
	    yellow
          Y
          |
          . --> X red
         /
        Z
	 blue
	*/

	int angle = 90;

	switch (face)
	{
	case 'F':
		g_vRotationAxis = CVector3(0, 0, 1);
		g_nRotatingSection = 2;
		angle = -90;
		break;

	case 'Z':
		g_vRotationAxis = CVector3(0, 0, 1);
		g_nRotatingSection = 1;
		angle = 90;
		break;

	case 'B':
		g_vRotationAxis = CVector3(0, 0, 1);
		g_nRotatingSection = 0;
		angle = 90;
		break;

	case 'L':
		g_vRotationAxis = CVector3(1, 0, 0);
		g_nRotatingSection = 0;
		angle = 90;
		break;

	case 'X':
		g_vRotationAxis = CVector3(1, 0, 0);
		g_nRotatingSection = 1;
		angle = 90;
		break;

	case 'R':
		g_vRotationAxis = CVector3(1, 0, 0);
		g_nRotatingSection = 2;
		angle = -90;
		break;

	case 'U':
		g_vRotationAxis = CVector3(0, 1, 0);
		g_nRotatingSection = 2;
		angle = -90;
		break;

	case 'Y':
		g_vRotationAxis = CVector3(0, 1, 0);
		g_nRotatingSection = 1;
		angle = 90;
		break;

	case 'D':
		g_vRotationAxis = CVector3(0, 1, 0);
		g_nRotatingSection = 0;
		angle = 90;
		break;

	default:
		//assert
		break;
	}

	angle = angle * numRotations;
	if (isPrime)
		angle = -angle;

	g_nRotationAngle = 0;
	int steps = 20;
	int step = (angle - g_nRotationAngle) / steps;
	for(int i = 0; i < steps; ++i)
	{
		g_nRotationAngle += step;
		RenderScene();
		Sleep(1);
	}
	//If after above loop, the target angle is not achieved
	if (g_nRotationAngle != angle)
	{
		g_nRotationAngle = angle;
		RenderScene();
	}

	//Fix cube position and Reset all parameters
	fixRubiksCubeFaces();
	g_nRotationAngle = 0;
	g_nRotatingSection = -1;
}

// Borrowed this logic from void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
void RubiksCubeSimulator::fixRubiksCubeFaces()
{
	int turns = 0;
	if (g_nRotationAngle == 0)
		turns = 0;
	else if (g_nRotationAngle == 90)
		turns = 1;
	else if (g_nRotationAngle == 180 || g_nRotationAngle == -180)
		turns = 2;
	else if (g_nRotationAngle == -90)
		turns = 3;

	if (g_vRotationAxis.x)
		g_cCube.Tilt(g_nRotatingSection, turns);
	else if (g_vRotationAxis.y)
		g_cCube.Rotate(g_nRotatingSection, turns);
	else if (g_vRotationAxis.z)
		g_cCube.Turn(g_nRotatingSection, turns);
}

void RubiksCubeSimulator::rotateWholeCube(int axis, int turns)
{
	
}
