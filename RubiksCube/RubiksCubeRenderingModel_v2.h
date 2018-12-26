#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
using namespace std;

#include "Vector3.h"
#include "RubiksCubeRenderingModel_v1.h"

namespace mm {

	typedef unsigned char byte;
	//typedef byte BYTE;

	//TODO: Define enum Axis
	/*
	enum Axis
	{
		XAxis = 0,
		YAxis = 1,
		ZAxis = 2
	};
	*/
	/*
	enum Face
	{
		Top = 0,
		Bottom = 1,
		Left = 2,
		Right = 3,
		Front = 4,
		Back = 5
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
	*/

	//enum Groups
	//{
	//	None = 0,

	//	L = 1,
	//	R = 2,

	//	D = 4,
	//	U = 8,

	//	B = 16,
	//	F = 32,

	//	All = 64 - 1 //Stores All flags from 1 to 32
	//};

	class Utility
	{
	public:
		//static Groups getGroup(char layer, int index)
		//{

		//}

		//static const CVector3& getAxis(char layer, int index)
		//{
		//	static vector<CVector3*> data{};
		//}
	};

	struct Location_v2
	{
		Location_v2()
			: x_(0.0), y_(0.0), z_(0.0)
		{}
		Location_v2(double x, double y, double z)
			: x_(x), y_(y), z_(z)
		{}
		Location_v2(const Location_v2&) = default;
		Location_v2(Location_v2&&) = default;
		Location_v2& operator=(const Location_v2&) = default;
		Location_v2& operator=(Location_v2&&) = default;

		bool operator==(const Location_v2& rhs) const
		{
			return fabs(x_ - rhs.x_) < 0.0001
				&& fabs(y_ - rhs.y_) < 0.0001
				&& fabs(z_ - rhs.z_) < 0.0001;
		}

		bool operator!=(const Location_v2& rhs) const
		{
			return !(*this == rhs);
		}

		void rotate(CVector3 rotationAxis, double rotationAngle)
		{
			static vector<vector<double>> rotationMatrix(4, vector<double>(4));

			// Initialize rotation matrix
			for (int Row = 0; Row < 4; Row++)
				for (int Column = 0; Column < 4; Column++)
					if (Row == Column)
						rotationMatrix[Row][Column] = 1.0;
					else
						rotationMatrix[Row][Column] = 0.0;

			if (rotationAxis == CVector3::XAxis)
			{
				rotationMatrix[1][1] = cos(rotationAngle);
				rotationMatrix[1][2] = sin(rotationAngle);
				rotationMatrix[2][1] = -sin(rotationAngle);
				rotationMatrix[2][2] = cos(rotationAngle);
			}
			else if (rotationAxis == CVector3::YAxis)
			{
				rotationMatrix[0][0] = cos(rotationAngle);
				rotationMatrix[0][2] = -sin(rotationAngle);
				rotationMatrix[2][0] = sin(rotationAngle);
				rotationMatrix[2][2] = cos(rotationAngle);
			}
			else if (rotationAxis == CVector3::ZAxis)
			{
				rotationMatrix[0][0] = cos(rotationAngle);
				rotationMatrix[0][1] = sin(rotationAngle);
				rotationMatrix[1][0] = -sin(rotationAngle);
				rotationMatrix[1][1] = cos(rotationAngle);
			}

			for (int Row = 0; Row < 4; Row++)
				for (int Column = 0; Column < 4; Column++)
					if (fabs(rotationMatrix[Row][Column]) < 0.000001)
						rotationMatrix[Row][Column] = 0.0;

			vector< vector<double>> geomVecMatrix(1, vector<double>(4, 1.0));
			geomVecMatrix[0][0] = x_;
			geomVecMatrix[0][1] = y_;
			geomVecMatrix[0][2] = z_;
			vector< vector<double>> result(1, vector<double>(4, 0.0));

			for (int i = 0; i < 1; i++)
				for (int j = 0; j < 4; j++)
					for (int k = 0; k < 4; k++) // OR use Matrix2.m_row. Both are equal.
						result[i][j] += geomVecMatrix[i][k] * rotationMatrix[k][j];

			x_ = result[0][0];
			y_ = result[0][1];
			z_ = result[0][2];
		}

		//int recalcGroup(int size)
		//{
		//	double extend = (size - 1) / 2.0;
		//	int group = 0;
		//	if (fabs(x_ - (-extend)) < 0.0001)
		//		group |= Groups::L;
		//	else if (fabs(x_ - extend) < 0.0001)
		//		group |= Groups::R;

		//	if (fabs(y_ - (-extend)) < 0.0001)
		//		group |= Groups::D;
		//	else if (fabs(y_ - extend) < 0.0001)
		//		group |= Groups::U;

		//	if (fabs(z_ - (-extend)) < 0.0001)
		//		group |= Groups::B;
		//	else if (fabs(z_ - extend) < 0.0001)
		//		group |= Groups::F;

		//	return group;
		//}

		double x_;
		double y_;
		double z_;
	};

	

	class Cube_v2
	{
	public:
		Cube_v2() {}
		Cube_v2(const Cube_v2& copy) 
			: faces_(copy.faces_), 
			location_(copy.location_)
			//group_(copy.group_)
		{}
		Cube_v2(Cube_v2&&) = default;
		Cube_v2& operator=(const Cube_v2&) = default;
		Cube_v2& operator=(Cube_v2&&) = default;
		Cube_v2(Color cTop, Color cBottom, Color cLeft,
			Color cRight, Color cFront, Color cBack, const Location_v2& location);
		~Cube_v2();
		Color GetFaceColor(Face eFace) const;
		//ColorRGB GetFaceColorRGB(Face eFace) const;
		void TiltUp();
		void TiltDown();
		void TurnLeft();
		void TurnRight();
		void TiltLeft();
		void TiltRight();

		const Location_v2& getLocation() const { return location_; }

		void rotate(CVector3 rotationAxis, double rotationAngle);
		bool belongsTo(Face rotatingSection, int layerIndex, int size) const;

	//private:
		static const int FACE_COUNT = 6;
		vector<Color> faces_;
		Location_v2 location_;
		//int group_;
	};

	class RubiksCubeSolverUI;

	class CRubiksCube_v2
	{
	public:
		CRubiksCube_v2(int size);
		~CRubiksCube_v2();
		CRubiksCube_v2(const CRubiksCube_v2& copy);

		void ResetCube();
		const Cube_v2& GetCube(double x, double y, double z);
		//const Cube_v2& GetCube(Face layer0, Face layer1, Face layer2);

		//void Rotate(int section, int turns);	// around y axis
		//void Tilt(int section, int turns);	// around x axis
		//void Turn(int section, int turns);	// around z axis
		void Rotate(CVector3 rotationAxis, Face rotatingSection, int layerIndex, double rotationAngle);
		//void Randomize();
		bool IsSolved();

		//void RotateWholeRubiksCube(int axis, int turns); This function is not required. Rotate can do it.

		//Apply algorithm
		int applyAlgorithm(const string& algorithm, bool animate = false, int steps = 0, RubiksCubeSolverUI* ui = nullptr);

		bool g_bRotating;
		bool g_bFlipRotation;
		CVector3 g_vRotationAxis;
		Face g_nRotatingSection;
		int g_nLayerIndex;
		double g_nRotationAngle;

		int getSize() { return size_; }

		string getScramblingAlgo();

	private:
		void applyStep(const char& face, bool isPrime, int numRotations, bool animate = false, int steps = 0, RubiksCubeSolverUI* ui = nullptr);
		void fixRubiksCubeFaces();

		//const CVector3& getRotationAxis(Groups rotationSection); //TODO: add this to localise group <--> Axis relation

		template <class T>
		static inline void hash_combine(std::size_t& seed, const T& v)  // Similar to boost::hash_combine<T>
		{
			/*
			The magic number below is the reciprocal of the golden ratio.
			Reference:
			https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
			http://burtleburtle.net/bob/hash/doobs.html
			phi = (1 + sqrt(5)) / 2
			2^32 / phi = 0x9e3779b9
			*/
			std::hash<T> hasher{};
			seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		struct Hasher
		{
			size_t operator()(const Location_v2& key) const noexcept
			{
				std::size_t seed = 0;
				hash_combine(seed, key.x_);
				hash_combine(seed, key.y_);
				hash_combine(seed, key.z_);
				return seed;
			}
		};

	//private:
	public:
		//Cube*** *m_pCubes;
		//vector< vector< vector<Cube> > > cubes_; // Total elements = size_ * size_ * size_
		unordered_map<Location_v2, unique_ptr<Cube_v2>, Hasher> cubes_; // Total elements = size_ * size_ * size_ - ( (size_-2) * (size_-2) * (size_-2) )
		//vector< vector<Cube*> > layerF_; //Front layer //Total elements = size_ * size_
		//vector< vector<Cube*> > layerB_; //Back layer
		//vector< vector<Cube*> > layerL_; //Left layer
		//vector< vector<Cube*> > layerR_; //Right layer
		//vector< vector<Cube*> > layerU_; //Upper layer
		//vector< vector<Cube*> > layerD_; //Down layer
		int size_;

	private:
		bool IsValidCube(int x, int y, int z);
		unique_ptr<Cube_v2> CreateCube(double x, double y, double z, const Location_v2& location);
		bool IsFaceSolved(Face face);
	};

}