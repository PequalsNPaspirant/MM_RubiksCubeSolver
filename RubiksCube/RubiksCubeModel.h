#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
using namespace std;

namespace mm {

	class RubiksCubeSolverUI;

	class RubiksCubeModel
	{
	public:
		virtual void loadAllTextures() = 0;
		virtual void ResetCube() = 0;
		virtual int applyAlgorithm(const string& algorithm, bool animate, RubiksCubeSolverUI& ui) = 0;
		virtual string getScramblingAlgo(int length) = 0;
		virtual string solve(int& solutionSteps, unsigned long long& duration, bool animate, RubiksCubeSolverUI& ui) = 0;
		virtual void render() = 0;
		virtual bool isSolved() = 0;

		virtual unique_ptr<RubiksCubeModel> copy() = 0;
		virtual string getModelName() = 0;
		virtual int getDimension() = 0;

		virtual ~RubiksCubeModel() = 0
		{
		}
	};

	typedef unique_ptr<RubiksCubeModel> (*fptrRubiksCubeModelCreator)(int size);

	class RubiksCubeModelFactory
	{
	public:
		static unique_ptr<RubiksCubeModel> getRubiksCubeModel(const string& modelName, int size);
		static void addModelCreatorToFactory(const string& modelName, fptrRubiksCubeModelCreator fptr);

	private:
		static unordered_map<string, fptrRubiksCubeModelCreator> rubiksCubeFactoryMap_;
	};
}