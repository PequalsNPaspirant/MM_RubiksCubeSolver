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

		double x_;
		double y_;
		double z_;
	};

	class Cube_v2
	{
	public:
		Cube_v2() {}
		Cube_v2(const Cube_v2& copy) : faces_(copy.faces_), location_(copy.location_) {}
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

	protected:
		static const int FACE_COUNT = 6;
		vector<Color> faces_;
		Location_v2 location_;
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

	private:
		void applyStep(const char& face, bool isPrime, int numRotations, bool animate = false, int steps = 0, RubiksCubeSolverUI* ui = nullptr);
		void fixRubiksCubeFaces();

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