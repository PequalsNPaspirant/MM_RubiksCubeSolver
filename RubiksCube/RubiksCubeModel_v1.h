#pragma once

#include <string>
#include <vector>
using namespace std;

#include <gl\gl.h>
#include <gl\glu.h>
#include "Vector3.h"
#include "RubiksCubeModel.h"

namespace mm {

	class RubiksCubeSolverUI;

	class RubiksCubeModel_v1 : public RubiksCubeModel
	{
	public:
		typedef unsigned char byte;

		enum Color
		{
			Yellow = 0,
			Red = 1,
			Blue = 2,
			Green = 3,
			Orange = 4,
			White = 5,
			Black = 6
		};

		enum Face
		{
			Up = 0,
			Down = 1,
			Left = 2,
			Right = 3,
			Front = 4,
			Back = 5,

			All = 6,
			None = 7
		};

		struct ColorRGB
		{
			ColorRGB(byte red, byte green, byte blue)
			{
				r = red;
				g = green;
				b = blue;
			}

			ColorRGB()
			{
				r = g = b = 0;
			}

			byte r;
			byte g;
			byte b;

			bool operator==(const ColorRGB& c)
			{
				return (r == c.r) && (g == c.g) && (b == c.b);
			}

			bool operator!=(const ColorRGB& c)
			{
				return !((r == c.r) && (g == c.g) && (b == c.b));
			}

			static const ColorRGB RGBColors[7];
		};

		class Cube
		{
		public:
			Cube() {}
			Cube(const Cube& copy)
				: faces_(copy.faces_)
			{}
			Cube(Color cTop, Color cBottom, Color cLeft,
				Color cRight, Color cFront, Color cBack);
			~Cube();
			Color GetFaceColor(Face eFace) const;
			void TiltUp();
			void TiltDown();
			void TurnLeft();
			void TurnRight();
			void TiltLeft();
			void TiltRight();


		private:
			static const int FACE_COUNT = 6;
			vector<Color> faces_;
		};

	public:
		RubiksCubeModel_v1(int size);
		~RubiksCubeModel_v1();
		RubiksCubeModel_v1(const RubiksCubeModel_v1& copy);

		void loadAllTextures();
		void ResetCube() override;
		int applyAlgorithm(const string& algorithm, bool animate, RubiksCubeSolverUI& ui) override;
		string getScramblingAlgo(int length, bool includeWholeCubeRotations) override;
		string solve(int& solutionSteps, unsigned long long& duration, bool animate, RubiksCubeSolverUI& ui) override;
		void render() override;
		void renderIndividualCube(const Cube& pCube, int x, int y, int z);
		bool isSolved() override;
		bool IsFaceSolved(Face face);

		unique_ptr<RubiksCubeModel> copy() override;
		string getModelName() override;
		int getDimension() override;
		
		void loadTexture(int nId, GLuint* texture);
		GLuint getTextureID(Color color);

		const Cube& GetCube(int x, int y, int z);
		void Rotate(int section, int turns);	// around y axis
		void Tilt(int section, int turns);	// around x axis
		void Turn(int section, int turns);	// around z axis
		void Randomize();
		int getSize() { return size_; }

	private:
		void applyStep(const char& face, bool isPrime, int numRotations, bool animate, RubiksCubeSolverUI& ui);
		void fixRubiksCubeFaces();
		bool IsValidCube(int x, int y, int z);
		Cube CreateCube(int x, int y, int z);

		vector< vector< vector<Cube> > > cubes_;
		int size_;
		bool g_bRotating;
		bool g_bFlipRotation;
		CVector3 g_vRotationAxis;
		int g_nRotatingSection;
		int g_nRotationAngle;
		GLuint g_pTextures[7];
		static const double CUBE_SIZE;

		//=======================================================================================================
		// Solver
		//=======================================================================================================

	public:

		class RubiksCubeSolver
		{
		public:
			RubiksCubeSolver(RubiksCubeModel_v1& rubiksCube, bool animate, RubiksCubeSolverUI& ui);
			string solve(int& solutionSteps);

		private:
			void positionTheCube();
			void buildCross();
			void buildF2L();
			void buildOLL();
			void buildPLL();

			void buildCross_PlaceEdgePiece(const Color& targetColorFront, const Color& targetColorBottom);
			void buildF2L_PositionCornerPieces(const Color& targetColorFront, const Color& targetColorRight, const Color& targetColorBottom = Color::White);
			bool buildF2L_PositionEdgePieces(const Color& targetColorFront, const Color& targetColorRight);

		private:
			void applyAlgorithm(const string& step);
			bool isEdgeCube(const Cube& currentCube, const Color& first, const Color& second);

			RubiksCubeModel_v1& rubiksCube_;
			string solution_;
			int solutionSteps_;

			bool animate_;
			RubiksCubeSolverUI& ui_;
		};
	};

}