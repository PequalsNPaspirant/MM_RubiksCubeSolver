#include "stdafx.h"

#include <string>
using namespace std;

#include "RubiksCubeSolverTest.h"

namespace mm {

	void test()
	{
		//Testing
		//scene_.g_cCube.applyAlgorithm("U", true, 20, this);

		string algo;
		algo = "U";
		algo = "D";
		algo = "L";
		algo = "R";
		algo = "F";
		algo = "B";
		algo = "X";
		algo = "Y";
		algo = "Z";

		algo = "U'";
		algo = "D'";
		algo = "L'";
		algo = "R'";
		algo = "F'";
		algo = "B'";
		algo = "X'";
		algo = "Y'";
		algo = "Z'";

		algo = "U2";
		algo = "D2";
		algo = "L2";
		algo = "R2";
		algo = "F2";
		algo = "B2";
		algo = "X2";
		//algo = "Y2";
		//algo = "Z2";

		//algo = "FB");
		//algo = "LR");
		//algo = "UD");
		//algo = "F'B'");
		//algo = "L'R'");
		//algo = "U'D'");
		//algo = "F2B2");
		//algo = "L2R2");
		//algo = "U2D2");
		//algo = "D F' R L' F L D' B' U' F R' F' U L' U2 R L B");
		//algo = "B' L' R' U2 L U' F R F' U B D L' F' L R' F D'");

		//scene_.g_cCube_v2.applyAlgorithm(algo, true, 20, this);

	}
	//string RubiksCubeSolverTest::getScramblingAlgo()
	//{
	//	char charSet[9] = { 'F', 'Z', 'B', 'L', 'X', 'R', 'U', 'Y', 'D' };
	//	string retVal;
	//	for (int i = 0; i < 25; ++i)
	//	{
	//		int index = rand() % 9;
	//		retVal += charSet[index];

	//		if (rand() % 2 == 0)
	//			retVal += '\'';
	//		else if (rand() % 10 == 0)
	//			retVal += '2';
	//	}

	//	return retVal;
	//}
}