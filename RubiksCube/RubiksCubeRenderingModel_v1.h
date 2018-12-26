#pragma once

#include <string>
#include <vector>
using namespace std;

#include "Vector3.h"

namespace mm {

	typedef unsigned char byte;
	//typedef byte BYTE;

	enum Face
	{
		//Top = 0,
		Up = 0,
		//Bottom = 1,
		Down = 1,
		Left = 2,
		Right = 3,
		Front = 4,
		Back = 5,

		All = 6,
		None = 7
	};

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

		//static const ColorRGB RED;
		//static const ColorRGB ORANGE;

		//static const ColorRGB BLUE;
		//static const ColorRGB GREEN;

		//static const ColorRGB YELLOW;
		//static const ColorRGB WHITE;

		//static const ColorRGB BLACK;

		static const ColorRGB RGBColors[7];
	};

	class Cube
	{
	public:
		Cube() {}
		Cube(const Cube& copy) : faces_(copy.faces_) {}
		Cube(Color cTop, Color cBottom, Color cLeft,
			Color cRight, Color cFront, Color cBack);
		~Cube(void);
		Color GetFaceColor(Face eFace) const;
		//ColorRGB GetFaceColorRGB(Face eFace) const;
		void TiltUp();
		void TiltDown();
		void TurnLeft();
		void TurnRight();
		void TiltLeft();
		void TiltRight();


	protected:
		static const int FACE_COUNT = 6;
		vector<Color> faces_;
	};

	class RubiksCubeSolverUI;

	class CRubiksCube
	{
	public:
		CRubiksCube(int size);
		~CRubiksCube();
		CRubiksCube(const CRubiksCube& copy);

		void ResetCube();
		const Cube& GetCube(int x, int y, int z);
		void Rotate(int section, int turns);	// around y axis
		void Tilt(int section, int turns);	// around x axis
		void Turn(int section, int turns);	// around z axis
		void Randomize();
		bool IsSolved();

		void RotateWholeRubiksCube(int axis, int turns);

		//Apply algorithm
		int applyAlgorithm(const string& algorithm, bool animate = false, int steps = 0, RubiksCubeSolverUI* ui = nullptr);

		bool g_bRotating;
		bool g_bFlipRotation;
		CVector3 g_vRotationAxis;
		int g_nRotatingSection;
		int g_nRotationAngle;

		int getSize() { return size_; }

		string getScramblingAlgo();

	private:
		void applyStep(const char& face, bool isPrime, int numRotations, bool animate = false, int steps = 0, RubiksCubeSolverUI* ui = nullptr);
		void fixRubiksCubeFaces();

	private:
		//Cube*** *m_pCubes;
		vector< vector< vector<Cube> > > cubes_;
		int size_;

	private:
		bool IsValidCube(int x, int y, int z);
		Cube CreateCube(int x, int y, int z);
		bool IsFaceSolved(Face face);
	};

}