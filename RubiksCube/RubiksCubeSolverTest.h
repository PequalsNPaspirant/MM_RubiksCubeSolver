#pragma once

#include <string>
using namespace std;

namespace mm {

	class RubiksCubeSolverUI;

	struct testInfo
	{
		string modelName;
		int size;
		string scrambleAlgo;
		string solution;
	};

	class RubiksCubeSolverTest
	{
	public:
		RubiksCubeSolverTest(RubiksCubeSolverUI& refUI)
			: refUI_(refUI)
		{}

		bool testRubiksCube(bool animate);

	private:
		vector<testInfo> generateSanityTestInfo();

		RubiksCubeSolverUI& refUI_;
	};
	
}