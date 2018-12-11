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


}