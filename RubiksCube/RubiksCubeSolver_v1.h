#pragma once

namespace mm {

	class RubiksCubeSolver_v1
	{
	public:
		static void solve();

	private:
		static void positionTheCube();
		static void buildCross();
		static void buildF2L();
		static void buildOLL();
		static void buildPLL();
	};
}