#include "StdAfx.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
using namespace std;

#include "RubiksCubeModel.h"

namespace mm {

	//Factory function declarations
	unique_ptr<RubiksCubeModel> createRubiksCubeModel_v1(int size);
	unique_ptr<RubiksCubeModel> createRubiksCubeModel_v2(int size);
	unique_ptr<RubiksCubeModel> createRubiksCubeModel_v3(int size);
	unique_ptr<RubiksCubeModel> createRubiksCubeModel_v4(int size);

	unordered_map<string, fptrRubiksCubeModelCreator> RubiksCubeModelFactory::rubiksCubeFactoryMap_ = {
		unordered_map<string, fptrRubiksCubeModelCreator>::value_type("RubiksCubeModel_v1", createRubiksCubeModel_v1),
		unordered_map<string, fptrRubiksCubeModelCreator>::value_type("RubiksCubeModel_v2", createRubiksCubeModel_v2),
		unordered_map<string, fptrRubiksCubeModelCreator>::value_type("RubiksCubeModel_v3", createRubiksCubeModel_v3),
		unordered_map<string, fptrRubiksCubeModelCreator>::value_type("RubiksCubeModel_v4", createRubiksCubeModel_v4)
	};

	unique_ptr<RubiksCubeModel> RubiksCubeModelFactory::getRubiksCubeModel(const string& modelName, int size)
	{
		return (*rubiksCubeFactoryMap_[modelName])(size);
	}

	void RubiksCubeModelFactory::addModelCreatorToFactory(const string& modelName, fptrRubiksCubeModelCreator fptr)
	{
		rubiksCubeFactoryMap_[modelName] = fptr;
	}
}