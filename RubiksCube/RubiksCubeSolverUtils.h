#pragma once

#include <string>
using namespace std;

#include "RubiksCubeSolverUI.h"

namespace mm {

	class RubiksCubeSolverUtils
	{
	public:
		static void RunTimeAssert(bool expression, const string& msg = "")
		{
			if (!expression)
			{
				//cout << msg;
				bool bContinue = RubiksCubeSolverUI::getInstance().CreateYesNoDialog("Assertion failure: " + msg + "\nDo you want to continue?");
				if (!bContinue)
				{
					int* nullPointer = nullptr;
					*nullPointer = 0;
				}
			}
		}
	};


}