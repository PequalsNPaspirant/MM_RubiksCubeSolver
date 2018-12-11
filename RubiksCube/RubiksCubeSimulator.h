#pragma once

#include <string>
using namespace std;

namespace mm {

	class RubiksCubeSimulator
	{
	public:
		static void test();
		static void scramble(const string& str);
		static void executeAlgorithm(const string& algorithm);
		static void rotateWholeCube(int axis, int turns);

	private:
		static void executeAlgorithm(const char& face, bool isPrime, int numRotations);
		static void fixRubiksCubeFaces();
	};

}