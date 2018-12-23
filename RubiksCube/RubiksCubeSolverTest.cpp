#include "stdafx.h"

#include <string>
using namespace std;

#include "RubiksCubeSolverTest.h"

namespace mm {

	string RubiksCubeSolverTest::getScramblingAlgo()
	{
		char charSet[9] = { 'F', 'Z', 'B', 'L', 'X', 'R', 'U', 'Y', 'D' };
		string retVal;
		for (int i = 0; i < 25; ++i)
		{
			int index = rand() % 9;
			retVal += charSet[index];

			if (rand() % 2 == 0)
				retVal += '\'';
			else if (rand() % 10 == 0)
				retVal += '2';
		}

		return retVal;
	}

	void testSimulator()
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
}