#include "StdAfx.h"

#include <time.h>
using namespace std;

#include "RubiksCubeRenderingModel_v1.h"
#include "RubiksCubeSolverUI.h"

namespace mm {

	const int FACE_COUNT = 6;

	const ColorRGB ColorRGB::RED{ 255, 0, 0 };
	const ColorRGB ColorRGB::ORANGE{ 255, 165, 0 };

	const ColorRGB ColorRGB::BLUE{ 0, 0, 255 };
	const ColorRGB ColorRGB::GREEN{ 0, 255, 0 };

	const ColorRGB ColorRGB::YELLOW{ 255, 255, 0 };
	const ColorRGB ColorRGB::WHITE{ 255, 255, 255 };

	const ColorRGB ColorRGB::BLACK{ 0, 0, 0 };

	Cube::Cube(Color cTop, Color cBottom, Color cLeft, Color cRight, Color cFront, Color cBack)
		: faces_(FACE_COUNT)
	{
		//faces_ = new Color[FACE_COUNT];

		faces_[Top] = cTop;
		faces_[Bottom] = cBottom;
		faces_[Left] = cLeft;
		faces_[Right] = cRight;
		faces_[Front] = cFront;
		faces_[Back] = cBack;
	}

	Cube::~Cube(void)
	{
		//delete[] faces_;
		//faces_ = NULL;
	}

	Color Cube::GetFaceColor(Face eFace) const
	{
		return faces_[eFace];
	}

	ColorRGB Cube::GetFaceColorRGB(Face eFace) const
	{
		switch(GetFaceColor(eFace))
		{
		case Yellow:
			return ColorRGB::YELLOW;
		case Red:
			return ColorRGB::RED;
		case Blue:
			return ColorRGB::BLUE;
		case Green:
			return ColorRGB::GREEN;
		case Orange:
			return ColorRGB::ORANGE;
		case White:
			return ColorRGB::WHITE;
		case Black :
			return ColorRGB::BLACK;
		default:
			//Assert
			return ColorRGB::BLACK;
		}
	}

	void Cube::TiltUp()
	{
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Back];

		faces_[Top] = faces_[Front];
		faces_[Back] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Front] = temp1;
	}

	void Cube::TiltDown()
	{
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Front];

		faces_[Top] = faces_[Back];
		faces_[Front] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Back] = temp1;
	}

	void Cube::TurnLeft()
	{
		Color temp1 = faces_[Front];
		Color temp2 = faces_[Left];

		faces_[Front] = faces_[Right];
		faces_[Left] = temp1;

		temp1 = faces_[Back];
		faces_[Back] = temp2;

		faces_[Right] = temp1;
	}

	void Cube::TurnRight()
	{
		Color temp1 = faces_[Front];
		Color temp2 = faces_[Right];

		faces_[Front] = faces_[Left];
		faces_[Right] = temp1;

		temp1 = faces_[Back];
		faces_[Back] = temp2;

		faces_[Left] = temp1;
	}

	void Cube::TiltLeft()
	{
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Left];

		faces_[Top] = faces_[Right];
		faces_[Left] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Right] = temp1;
	}

	void Cube::TiltRight()
	{
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Right];

		faces_[Top] = faces_[Left];
		faces_[Right] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Left] = temp1;
	}



	CRubiksCube::CRubiksCube(int size)
		: cubes_(vector< vector< vector<Cube> > > (size, vector< vector<Cube> >(size, vector<Cube>(size)) ) ),
		size_(size),
		g_bRotating(false),
		g_bFlipRotation(false),
		g_vRotationAxis(0, 0, 0),
		g_nRotatingSection(-1),
		g_nRotationAngle(0)
	{
		ResetCube();

		//cubes_ = new Cube**[size_];

		//for (int i = 0; i < size_; i++)
		//{
		//	//cubes_[i] = new Cube*[size_];

		//	for (int j = 0; j < size_; j++)
		//	{
		//		//cubes_[i][j] = new Cube[size_];

		//		for (int k = 0; k < size_; k++)
		//		{
		//			cubes_[i][j][k] = CreateCube(i, j, k);
		//		}
		//	}
		//}
	}

	CRubiksCube::~CRubiksCube()
	{
		for (int i = 0; i < size_; i++)
		{
			for (int j = 0; j < size_; j++)
			{
				for (int k = 0; k < size_; k++)
				{
					//delete cubes_[i][j][k];
					//cubes_[i][j][k] = NULL;
				}

				//delete[] cubes_[i][j];
				//cubes_[i][j] = NULL;
			}

			//delete[] cubes_[i];
			//cubes_[i] = NULL;
		}

		//delete[] cubes_;
		//cubes_ = NULL;
	}

	CRubiksCube::CRubiksCube(const CRubiksCube& copy)
		: cubes_(copy.cubes_),
		size_(copy.size_),
		g_bRotating(copy.g_bRotating),
		g_bFlipRotation(copy.g_bFlipRotation),
		g_vRotationAxis(copy.g_vRotationAxis),
		g_nRotatingSection(copy.g_nRotatingSection),
		g_nRotationAngle(copy.g_nRotationAngle)
	{
		//cubes_ = new Cube**[size_];

		for (int i = 0; i < size_; i++)
		{
			//cubes_[i] = new Cube*[size_];

			for (int j = 0; j < size_; j++)
			{
				//cubes_[i][j] = new Cube[size_];

				for (int k = 0; k < size_; k++)
				{
					//cubes_[i][j][k] = CreateCube(i, j, k);
				}
			}
		}
	}

	void CRubiksCube::ResetCube()
	{
		g_bRotating = false;
		g_bFlipRotation = false;
		g_vRotationAxis = CVector3(0, 0, 0);
		g_nRotatingSection = -1;
		g_nRotationAngle = 0;

		for (int i = 0; i < size_; i++)
		{
			//cubes_[i] = new Cube*[size_];

			for (int j = 0; j < size_; j++)
			{
				//cubes_[i][j] = new Cube[size_];

				for (int k = 0; k < size_; k++)
				{
					cubes_[i][j][k] = CreateCube(i, j, k);
				}
			}
		}
	}

	Cube CRubiksCube::CreateCube(int x, int y, int z)
	{
		Color left, right, top, bottom, front, back;

		if (x == 0)
		{
			//left = Color(255, 165, 0);	// orange
			//right = Color(0, 0, 0);	// black
			left = Orange;
			right = Black;
		}
		else if (x == 1)
		{
			//left = Color(0, 0, 0);		// black
			//right = Color(0, 0, 0);	// black
			left = Black;
			right = Black;
		}
		else if (x == 2)
		{
			//left = Color(0, 0, 0);		// black
			//right = Color(255, 0, 0);	// red
			left = Black;
			right = Red;
		}
		if (y == 0)
		{
			//bottom = Color(255, 255, 255);	// blue //white Color(255, 255, 255)
			//top = Color(0, 0, 0);		// black
			bottom = White;
			top = Black;
		}
		else if (y == 1)
		{
			//bottom = Color(0, 0, 0);	// black
			//top = Color(0, 0, 0);		// black
			bottom = Black;
			top = Black;
		}
		else if (y == 2)
		{
			//bottom = Color(0, 0, 0);	  // black 
			//top = Color(255, 255, 0);	  // white //yellow Color(255, 255, 0)
			bottom = Black;
			top = Yellow;
		}
		if (z == 0)
		{
			//back = Color(0, 255, 0);	// yellow //green Color(0, 255, 0)
			//front = Color(0, 0, 0);	// black
			back = Green;
			front = Black;
		}
		else if (z == 1)
		{
			//back = Color(0, 0, 0);		// black
			//front = Color(0, 0, 0);	// black
			back = Black;
			front = Black;
		}
		else if (z == 2)
		{
			//back = Color(0, 0, 0);		// black
			//front = Color(0, 0, 255);	// green // blue Color(0, 0, 255)
			back = Black;
			front = Blue;
		}

		return Cube(top, bottom, left, right, front, back);
	}

	const Cube& CRubiksCube::GetCube(int x, int y, int z)
	{
		//if (!IsValidCube(x, y, z))
		//	assert
		
		return cubes_[x][y][z];
	}

	bool CRubiksCube::IsValidCube(int x, int y, int z)
	{
		return (x >= 0 && x < size_) &&
			(y >= 0 && y < size_) &&
			(z >= 0 && z < size_);
	}

	void CRubiksCube::Rotate(int section, int turns)
	{
		if (section >= 0 && section < 3)
		{
			for (int i = 0; i < size_; i++)
			{
				for (int k = 0; k < size_; k++)
				{
					for (int l = 0; l < turns; l++)
					{
						cubes_[i][section][k].TurnRight();
					}
				}
			}

			for (int i = 0; i < turns; i++)
			{
				Cube temp1 = cubes_[0][section][0];
				Cube temp2 = cubes_[0][section][2];

				cubes_[0][section][2] = temp1;

				temp1 = cubes_[2][section][2];
				cubes_[2][section][2] = temp2;

				temp2 = cubes_[2][section][0];
				cubes_[2][section][0] = temp1;

				cubes_[0][section][0] = temp2;

				temp1 = cubes_[1][section][0];
				temp2 = cubes_[0][section][1];

				cubes_[0][section][1] = temp1;
				temp1 = cubes_[1][section][2];

				cubes_[1][section][2] = temp2;
				temp2 = cubes_[2][section][1];

				cubes_[2][section][1] = temp1;
				cubes_[1][section][0] = temp2;
			}
		}
	}

	void CRubiksCube::Tilt(int section, int turns)
	{
		if (section >= 0 && section < 3)
		{
			for (int j = 0; j < size_; j++)
			{
				for (int k = 0; k < size_; k++)
				{
					for (int l = 0; l < turns; l++)
					{
						cubes_[section][j][k].TiltDown();
					}
				}
			}

			for (int i = 0; i < turns; i++)
			{
				Cube temp1 = cubes_[section][0][0];
				Cube temp2 = cubes_[section][2][0];

				cubes_[section][2][0] = temp1;

				temp1 = cubes_[section][2][2];
				cubes_[section][2][2] = temp2;

				temp2 = cubes_[section][0][2];
				cubes_[section][0][2] = temp1;

				cubes_[section][0][0] = temp2;

				temp1 = cubes_[section][1][0];
				temp2 = cubes_[section][2][1];

				cubes_[section][2][1] = temp1;
				temp1 = cubes_[section][1][2];

				cubes_[section][1][2] = temp2;
				temp2 = cubes_[section][0][1];

				cubes_[section][0][1] = temp1;
				cubes_[section][1][0] = temp2;
			}
		}
	}

	void CRubiksCube::Turn(int section, int turns)
	{
		if (section >= 0 && section < 3)
		{
			// rotate each cube
			for (int i = 0; i < size_; i++)
			{
				for (int j = 0; j < size_; j++)
				{
					for (int l = 0; l < turns; l++)
					{
						cubes_[i][j][section].TiltLeft();
					}
				}
			}

			for (int i = 0; i < turns; i++)
			{
				Cube temp1 = cubes_[0][0][section];
				Cube temp2 = cubes_[2][0][section];

				cubes_[2][0][section] = temp1;

				temp1 = cubes_[2][2][section];
				cubes_[2][2][section] = temp2;

				temp2 = cubes_[0][2][section];
				cubes_[0][2][section] = temp1;

				cubes_[0][0][section] = temp2;

				temp1 = cubes_[0][1][section];
				temp2 = cubes_[1][0][section];

				cubes_[1][0][section] = temp1;
				temp1 = cubes_[2][1][section];

				cubes_[2][1][section] = temp2;
				temp2 = cubes_[1][2][section];

				cubes_[1][2][section] = temp1;
				cubes_[0][1][section] = temp2;
			}
		}
	}

	void CRubiksCube::RotateWholeRubiksCube(int axis, int turns)
	{
		/*
		axis = 0 for X-axis
		axis = 1 for Y-axis
		axis = 2 for Z-axis
		*/

		for (int section = 0; section < 3; ++section)
		{
			if (axis == 0)
				Tilt(section, turns);
			else if (axis == 0)
				Rotate(section, turns);
			else if (axis == 0)
				Turn(section, turns);
		}
	}

	void CRubiksCube::Randomize()
	{
		int count = 0;
		bool done = false;
		srand((unsigned)time(NULL));

		while (!done)
		{
			int turns, section, axis;
			turns = (int)((double)rand() / (RAND_MAX + 1) * (4));
			section = (int)((double)rand() / (RAND_MAX + 1) * (size_));
			axis = (int)((double)rand() / (RAND_MAX + 1) * (3));

			switch (axis)
			{
			case 0:
				this->Rotate(section, turns);
				break;
			case 1:
				this->Tilt(section, turns);
			case 2:
				this->Turn(section, turns);
			}

			count++;

			if (count >= 20)
			{
				int diff = count - 20;
				int probability = (int)((double)rand() / (RAND_MAX + 1) * (100 - diff) + diff);

				if (probability >= 75)
					done = true;
			}
		}
	}

	bool CRubiksCube::IsSolved()
	{
		return IsFaceSolved(Top) &&
			IsFaceSolved(Bottom) &&
			IsFaceSolved(Left) &&
			IsFaceSolved(Right) &&
			IsFaceSolved(Front) &&
			IsFaceSolved(Back);
	}

	bool CRubiksCube::IsFaceSolved(Face face)
	{
		if (face == Top || face == Bottom)
		{
			int j = (face == Top) ? 2 : 0;

			Color color = cubes_[0][j][0].GetFaceColor(face);

			for (int i = 0; i < size_; i++)
			{
				for (int k = 0; k < size_; k++)
				{
					if (cubes_[i][j][k].GetFaceColor(face) != color)
						return FALSE;
				}
			}
		}

		else if (face == Left || face == Right)
		{
			int i = (face == Left) ? 0 : 2;

			Color color = cubes_[i][0][0].GetFaceColor(face);

			for (int j = 0; j < size_; j++)
			{
				for (int k = 0; k < size_; k++)
				{
					if (cubes_[i][j][k].GetFaceColor(face) != color)
						return FALSE;
				}
			}
		}

		else if (face == Front || face == Back)
		{
			int k = (face == Front) ? 2 : 0;

			Color color = cubes_[0][0][k].GetFaceColor(face);

			for (int i = 0; i < size_; i++)
			{
				for (int j = 0; j < size_; j++)
				{
					if (cubes_[i][j][k].GetFaceColor(face) != color)
						return FALSE;
				}
			}
		}

		return TRUE;
	}



	void CRubiksCube::applyAlgorithm(const string& algorithm, bool animate /*= false*/, int steps /*= 0*/, RubiksCubeSolverUI* ui /*= nullptr*/)
	{
		g_bFlipRotation = FALSE;

		for (int i = 0; i < algorithm.length(); ++i)
		{
			char face = algorithm[i];
			if (face == ' ')
				continue;

			if (face >= 'a')
				face = face - 32; // Convert into Upper case char

								  // Check if prime operation
			bool isPrime = false;
			int nextCharIndex = i + 1;
			if (nextCharIndex < algorithm.length() && algorithm[nextCharIndex] == '\'')
			{
				isPrime = true;
				++i;
			}
			// Check if multiple rotations
			nextCharIndex = i + 1;
			int numRotations = 1;
			if (nextCharIndex < algorithm.length() && '0' <= algorithm[nextCharIndex] && algorithm[nextCharIndex] <= '9')
			{
				numRotations = algorithm[i + 1] - '0';
				++i;
			}

			applyStep(face, isPrime, numRotations, animate, steps, ui);
		}
		//Scene::getInstance().g_bRotating = FALSE; // TODO: Find why this resets the cube
	}

	void CRubiksCube::applyStep(const char& face, bool isPrime, int numRotations, bool animate /*= false*/, int steps /*= 0*/, RubiksCubeSolverUI* ui /*= nullptr*/)
	{
		//cout << "\nApplying move: " << face;
		//if(isPrime)
		//	cout << "\'";
		//if(numRotations > 1)
		//	cout << numRotations;

		// Z-axis : Back -> Front // green  -> blue
		// X-axis : Left -> Right // orange -> red
		// Y-axis : Down -> Up    // white  -> yellow
		/*
		yellow
		Y
		|
		. --> X red
		/
		Z
		blue
		*/

		//int angle = 90;

		switch (face)
		{
		case 'F':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = 2;
			g_nRotationAngle = -90;
			break;

		case 'Z':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = 1;
			g_nRotationAngle = 90;
			break;

		case 'B':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = 0;
			g_nRotationAngle = 90;
			break;

		case 'L':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = 0;
			g_nRotationAngle = 90;
			break;

		case 'X':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = 1;
			g_nRotationAngle = 90;
			break;

		case 'R':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = 2;
			g_nRotationAngle = -90;
			break;

		case 'U':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = 2;
			g_nRotationAngle = -90;
			break;

		case 'Y':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = 1;
			g_nRotationAngle = 90;
			break;

		case 'D':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = 0;
			g_nRotationAngle = 90;
			break;

		default:
			//assert
			break;
		}

		g_nRotationAngle = g_nRotationAngle * numRotations;
		if (isPrime)
			g_nRotationAngle = -g_nRotationAngle;

		if (animate)
		{
			g_bRotating = TRUE;
			int angle = g_nRotationAngle;
			g_nRotationAngle = 0;
			int step = (angle - g_nRotationAngle) / steps;
			for (int i = 0; i < steps; ++i)
			{
				g_nRotationAngle += step;
				//scene.renderScene();
				if(ui)
					ui->redrawWindow();
				Sleep(5);
			}

			//If after above loop, the target angle is not achieved
			if (g_nRotationAngle != angle)
			{
				g_nRotationAngle = angle;
				//scene.renderScene();
				if (ui)
					ui->redrawWindow();
			}
			g_bRotating = FALSE;
		}

		//Fix cube position and Reset all parameters
		fixRubiksCubeFaces();
		g_nRotationAngle = 0;
		g_nRotatingSection = -1;
	}


	void CRubiksCube::fixRubiksCubeFaces()
	{
		int turns = 0;
		if (g_nRotationAngle == 0)
			turns = 0;
		else if (g_nRotationAngle == 90)
			turns = 1;
		else if (g_nRotationAngle == 180 || g_nRotationAngle == -180)
			turns = 2;
		else if (g_nRotationAngle == -90)
			turns = 3;

		if (g_vRotationAxis.x)
			Tilt(g_nRotatingSection, turns);
		else if (g_vRotationAxis.y)
			Rotate(g_nRotatingSection, turns);
		else if (g_vRotationAxis.z)
			Turn(g_nRotatingSection, turns);
	}

}

