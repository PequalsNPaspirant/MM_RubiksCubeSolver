//=======================================================================================================//
//   Copyright (c) 2018 Maruti Mhetre                                                                    //
//   All rights reserved.                                                                                //
//=======================================================================================================//
//   Redistribution and use of this software in source and binary forms, with or without modification,   //
//   are permitted for personal, educational or non-commercial purposes provided that the following      //
//   conditions are met:                                                                                 //
//   1. Redistributions of source code must retain the above copyright notice, this list of conditions   //
//      and the following disclaimer.                                                                    //
//   2. Redistributions in binary form must reproduce the above copyright notice, this list of           //
//      conditions and the following disclaimer in the documentation and/or other materials provided     //
//      with the distribution.                                                                           //
//   3. Neither the name of the copyright holder nor the names of its contributors may be used to        //
//      endorse or promote products derived from this software without specific prior written            //
//      permission.                                                                                      //
//=======================================================================================================//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR      //
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND    //
//   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          //
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   //
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   //
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  //
//   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT   //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     //
//=======================================================================================================//

#include "stdafx.h"

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <cassert>
#include <fstream> //for file read/write
#include <sstream> //for stringstream
#include <iomanip> //for std::put_time
using namespace std;

#include "RubiksCubeSolverTest.h"
#include "RubiksCubeModel.h"
#include "RubiksCubeModel_v1.h"
#include "RubiksCubeModel_v2.h"
#include "RubiksCubeSolverUI.h"
#include "RubiksCubeSolverUtils.h"

namespace mm {

	bool RubiksCubeSolverTest::testRubiksCube(bool animate)
	{
		vector<testInfoAggregate> testInfoAggregateSet{
			{ "RubiksCubeModel_v3", 2 },
			{ "RubiksCubeModel_v4", 2 },

			{ "RubiksCubeModel_v1", 3 },
			{ "RubiksCubeModel_v2", 3 },
			{ "RubiksCubeModel_v3", 3 },
			{ "RubiksCubeModel_v4", 3 },

			{ "RubiksCubeModel_v4", 4 }
		};

		//Specific test cases to test
		//testInfoSet.push_back({ "RubiksCubeModel_v2", 3, "X", "X'" });
		//testInfoSet.push_back({ "RubiksCubeModel_v2", 3, "D2", "D'2" });
		//testInfoSet.push_back({ "RubiksCubeModel_v3", 2, "F", "" });
		//testInfoSet.push_back({ "RubiksCubeModel_v3", 2, "D'", "" });
		//testInfoSet.push_back({ "RubiksCubeModel_v4", 3, "D", "" });
		//testInfoSet.push_back({ "RubiksCubeModel_v4", 3, "D'", "" });
		//testInfoSet.push_back({ "RubiksCubeModel_v4", 3, "D2", "" });
		//testInfoSet.push_back({ "RubiksCubeModel_v4", 3, "LRDFL'", "" });

		unique_ptr<RubiksCubeModel> originalModel = refUI_.replaceModelBy("RubiksCubeModel_v1", 3, false);

		int numTestCases = executeAllTests(testInfoAggregateSet, animate);
		refUI_.CreateOkDialog("All " + to_string(numTestCases) + " tests are successfully completed!");

		refUI_.replaceModelBy(std::move(originalModel), true);

		return true;
	}

	int RubiksCubeSolverTest::executeAllTests(vector<testInfoAggregate>& testInfoAggregateSet, bool animate)
	{
		vector<int> lengths{ 5, 10, 15, 20, 25, 30, 50, 75, 100, 200 };

		struct AlgoPairs
		{
			string scramble;
			string solution;
		};

		//50 hardcoded scrambling algos
		vector<AlgoPairs> scrambleAlgos{
			{ "U", "U'" },
			{ "D", "D'" },
			{ "L", "L'" },
			{ "R", "R'" },
			{ "F", "F'" },
			{ "B", "B'" },
			{ "X", "X'" },
			{ "Y", "Y'" },
			{ "Z", "Z'" },

			{ "U'", "U" },
			{ "D'", "D" },
			{ "L'", "L" },
			{ "R'", "R" },
			{ "F'", "F" },
			{ "B'", "B" },
			{ "X'", "X" },
			{ "Y'", "Y" },
			{ "Z'", "Z" },

			{ "U2", "U'2" },
			{ "D2", "D'2" },
			{ "L2", "L'2" },
			{ "R2", "R'2" },
			{ "F2", "F'2" },
			{ "B2", "B'2" },
			{ "X2", "X'2" },
			{ "Y2", "Y'2" },
			{ "Z2", "Z'2" },

			{ "U'2",  "U2" },
			{ "D'2",  "D2" },
			{ "L'2",  "L2" },
			{ "R'2",  "R2" },
			{ "F'2",  "F2" },
			{ "B'2",  "B2" },
			{ "X'2",  "X2" },
			{ "Y'2",  "Y2" },
			{ "Z'2",  "Z2" },

			{ "FB", "B'F'" },
			{ "LR", "R'L'" },
			{ "UD", "D'U'" },
			{ "F'B'", "BF" },
			{ "L'R'", "RL" },
			{ "U'D'", "DU" },

			{ "F2B2", "B'2F'2" },
			{ "L2R2", "R'2L'2" },
			{ "U2D2", "D'2U'2" },

			{ "FLU", "U'L'F'" },
			{ "F'L'U'", "ULF" },
			{ "BRD", "D'R'B'" },
			{ "B'R'D'", "DRB" },
			{ "FLUBRD", "D'R'B'U'L'F'" },
		};

		//Add 10 x 100 = 1000 random srambling algos independent of Model
		bool includeNonStandardRotations = false;
		unique_ptr<RubiksCubeModel> model = RubiksCubeModelFactory::getRubiksCubeModel("RubiksCubeModel_v2", 3);
		const int numAlgoOfEachLength = 100;
		for (int len : lengths)
			for (int i = 0; i < numAlgoOfEachLength; ++i)
				scrambleAlgos.push_back({ model->getScramblingAlgo(len, includeNonStandardRotations), "" });

		vector<testInfo> testInfoSetCommon;
		vector<testInfo> testInfoSetModelSpecific;
		ofstream testResultsFile;
		testResultsFile.open("../test/RubiksCubeTestResults_" + getCurrentLocalTimeInNanoSeconds2() + ".csv");

		for (testInfoAggregate& modelinfo : testInfoAggregateSet)
		{
			modelinfo.nsAggregateDuration_ = 0;
			modelinfo.numTestCases_ = scrambleAlgos.size();
		}

		if (testResultsFile.is_open())
		{
			//All above 50 + 1000 scrambling algos are tested on every model
			unsigned int testNum = 0;
			for (const AlgoPairs& algoPair : scrambleAlgos)
			{
				for (testInfoAggregate& modelinfo : testInfoAggregateSet)
				{
					testInfoSetCommon.emplace_back(modelinfo.modelName_, modelinfo.size_, algoPair.scramble, algoPair.solution);
					executeTest(*testInfoSetCommon.rbegin(), testResultsFile, animate, ++testNum);
					modelinfo.nsAggregateDuration_ += testInfoSetCommon.rbegin()->nsDuration_;
				}
			}			
			writeResultsToCSVFile(testResultsFile, testInfoAggregateSet);

			//numModex x 10 x 100 = numModex x 1000  Model specific scrambling algos
			includeNonStandardRotations = true;
			testNum = 0;
			for (testInfoAggregate& modelinfo : testInfoAggregateSet)
			{
				modelinfo.nsAggregateDuration_ = 0;
				modelinfo.numTestCases_ = 0;
				for (int len : lengths)
				{
					for (int i = 0; i < numAlgoOfEachLength; ++i)
					{
						testInfoSetModelSpecific.emplace_back(modelinfo.modelName_, modelinfo.size_, model->getScramblingAlgo(len, includeNonStandardRotations), "");
						executeTest(*testInfoSetModelSpecific.rbegin(), testResultsFile, animate, ++testNum);
						modelinfo.nsAggregateDuration_ += testInfoSetModelSpecific.rbegin()->nsDuration_;
						modelinfo.numTestCases_ += 1;
					}
				}
			}
			writeResultsToCSVFile(testResultsFile, testInfoAggregateSet);

			writeResultsToCSVFile(testResultsFile, testInfoSetCommon);
			writeResultsToCSVFile(testResultsFile, testInfoSetModelSpecific);

			testResultsFile.close();
		}

		return testInfoSetCommon.size() + testInfoSetModelSpecific.size();
	}

	void RubiksCubeSolverTest::executeTest(testInfo& info, ofstream& testResultsFile, bool animate, unsigned int testNum)
	{			
		if (animate)
			refUI_.CreateOkDialog("Test No.: " + to_string(testNum)
				+ "\nModel Name: " + info.modelName_
				+ "\nSize: " + to_string(info.size_));
		unique_ptr<RubiksCubeModel> oldModel = refUI_.replaceModelBy(info.modelName_, info.size_, animate);
		oldModel.reset(nullptr);
		refUI_.Reset(animate); //TODO: replaceModelBy() has a bug. it does not print the cube on screen properly. Reset() is temporary workaround.
		RubiksCubeSolverUtils::RunTimeAssert(refUI_.isSolved());

		if (animate)
			refUI_.CreateOkDialog("Applying scrambling algo: " + info.scrambleAlgo_);
		refUI_.applyAlgorithm(info.scrambleAlgo_, animate);
		//RubiksCubeSolverUtils::RunTimeAssert(!refUI_.isSolved()); //Rubik Cube may or may not be scrmabled depending upon the scrambling algo

		if (!info.idealSolution_.empty())
		{
			if (animate)
				refUI_.CreateOkDialog("Applying Ideal solution: " + info.idealSolution_);
			refUI_.applyAlgorithm(info.idealSolution_, animate);
			RubiksCubeSolverUtils::RunTimeAssert(refUI_.isSolved());

			if (animate)
				refUI_.CreateOkDialog("Going back to scrambled position: " + info.scrambleAlgo_);
			//refUI_.replaceModelBy(modelinfo.modelName, modelinfo.size);
			refUI_.Reset(animate);
			refUI_.applyAlgorithm(info.scrambleAlgo_, false);
			if (animate)
				refUI_.redrawWindow();
			//RubiksCubeSolverUtils::RunTimeAssert(!refUI_.isSolved()); //Rubik Cube may or may not be scrmabled depending upon the scrambling algo
		}

		if (animate)
			refUI_.CreateOkDialog("Solving again: ");
		//int numSteps;
		//unsigned long long duration;
		info.actualSolution_ = refUI_.Solve(info.numSteps_, info.nsDuration_, animate);
		RubiksCubeSolverUtils::RunTimeAssert(refUI_.isSolved());
	}

	void RubiksCubeSolverTest::writeResultsToCSVFile(ofstream& testResultsFile, const vector<testInfo>& testInfoSet)
	{
		if (!testResultsFile.is_open())
			return;

		string columns("ModelName,Size,Steps,Duration (ns),Scrambling Algorithm,Ideal Solution,Actual Solution");
		testResultsFile.write(columns.c_str(), columns.length());

		for (const testInfo& info : testInfoSet)
		{
			testResultsFile.write("\n", 1);

			string modelName(info.modelName_);
			testResultsFile.write(modelName.c_str(), modelName.length());
			
			testResultsFile.write(",", 1);
			string size(to_string(info.size_));
			testResultsFile.write(size.c_str(), size.length());
			
			testResultsFile.write(",", 1);
			string numStepsStr(to_string(info.numSteps_));
			testResultsFile.write(numStepsStr.c_str(), numStepsStr.length());
			
			testResultsFile.write(",", 1);
			std::stringstream buffer;
			constexpr int maxPrecision = 4;
			buffer.precision(maxPrecision);
			buffer << std::fixed;
			buffer.imbue(std::locale(""));
			buffer << info.nsDuration_;
			string durationStr = "\"" + buffer.str() + "\"";
			testResultsFile.write(durationStr.c_str(), durationStr.length());
			
			testResultsFile.write(",", 1);
			testResultsFile.write(info.scrambleAlgo_.c_str(), info.scrambleAlgo_.length());
			
			testResultsFile.write(",", 1);
			testResultsFile.write(info.idealSolution_.c_str(), info.idealSolution_.length());
			
			testResultsFile.write(",", 1);
			testResultsFile.write(info.actualSolution_.c_str(), info.actualSolution_.length());

			testResultsFile.flush();
		}
		testResultsFile.write("\n\n", 2);
	}

	void RubiksCubeSolverTest::writeResultsToCSVFile(ofstream& testResultsFile, const vector<testInfoAggregate>& testInfoAggregateSet)
	{
		if (!testResultsFile.is_open())
			return;

		string columns("ModelName,Size,Total test cases,Aggregate Duration (ns)");
		testResultsFile.write(columns.c_str(), columns.length());
		for (const testInfoAggregate& modelinfo : testInfoAggregateSet)
		{
			testResultsFile.write("\n", 1);

			string modelName(modelinfo.modelName_);
			testResultsFile.write(modelName.c_str(), modelName.length());
			
			testResultsFile.write(",", 1);
			string size(to_string(modelinfo.size_));
			testResultsFile.write(size.c_str(), size.length());
			
			testResultsFile.write(",", 1);
			string numTestCases(to_string(modelinfo.numTestCases_));
			testResultsFile.write(numTestCases.c_str(), numTestCases.length());
			
			testResultsFile.write(",", 1);
			std::stringstream buffer;
			constexpr int maxPrecision = 4;
			buffer.precision(maxPrecision);
			buffer << std::fixed;
			buffer.imbue(std::locale(""));
			buffer << modelinfo.nsAggregateDuration_;
			string durationStr = "\"" + buffer.str() + "\"";
			testResultsFile.write(durationStr.c_str(), durationStr.length());

			testResultsFile.flush();
		}
		testResultsFile.write("\n\n", 2);
	}

	string RubiksCubeSolverTest::getCurrentLocalTimeInNanoSeconds2()
	{
		//This function takes aorund 50 to 70 microseconds to execute
		chrono::system_clock::time_point now = chrono::system_clock::now();

		std::time_t now_t = chrono::system_clock::to_time_t(now);
		std::tm nowTm;
		localtime_s(&nowTm, &now_t);
		std::stringstream bufferStream;
		bufferStream << std::put_time(&nowTm, "%Y-%m-%d_%H-%M-%S");

		long long duration = (chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch())).count() % (long long)1000000000;

		//duration is in nanoseconds, so it can have max 9 digits, 2 commas and 1 dot
		string fractional_seconds(".000,000,000");
		for (int index = 11; duration > 0; --index)
		{
			if (index == 8 || index == 4)
				continue;
			fractional_seconds[index] = '0' + (duration % 10);
			duration /= 10;
		}

		string retVal(bufferStream.str() + fractional_seconds);
		return std::move(retVal);
	}


}