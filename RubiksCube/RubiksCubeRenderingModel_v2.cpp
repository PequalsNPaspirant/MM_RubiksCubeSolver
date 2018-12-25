#include "StdAfx.h"

#include <time.h>
#include <cassert>
using namespace std;

#include "RubiksCubeRenderingModel_v2.h"
#include "RubiksCubeSolverUI.h"

namespace mm {
	/*
	const int FACE_COUNT = 6;

	//const ColorRGB ColorRGB::RED{ 255, 0, 0 };
	//const ColorRGB ColorRGB::ORANGE{ 255, 165, 0 };

	//const ColorRGB ColorRGB::BLUE{ 0, 0, 255 };
	//const ColorRGB ColorRGB::GREEN{ 0, 255, 0 };

	//const ColorRGB ColorRGB::YELLOW{ 255, 255, 0 };
	//const ColorRGB ColorRGB::WHITE{ 255, 255, 255 };

	//const ColorRGB ColorRGB::BLACK{ 0, 0, 0 };

	const ColorRGB ColorRGB::RGBColors[7] = {
		ColorRGB{ 255, 255, 0 },
		ColorRGB{ 255, 0, 0 },
		ColorRGB{ 0, 0, 255 },
		ColorRGB{ 0, 255, 0 },
		ColorRGB{ 255, 165, 0 },
		ColorRGB{ 255, 255, 255 },
		ColorRGB{ 0, 0, 0 }
	};
	*/

	Cube_v2::Cube_v2(Color cTop, Color cBottom, Color cLeft, Color cRight, Color cFront, Color cBack, const Location_v2& location, int group)
		: faces_(FACE_COUNT)
	{
		//faces_ = new Color[FACE_COUNT];

		faces_[Top] = cTop;
		faces_[Bottom] = cBottom;
		faces_[Left] = cLeft;
		faces_[Right] = cRight;
		faces_[Front] = cFront;
		faces_[Back] = cBack;

		location_ = location;
		group_ = group;
	}

	Cube_v2::~Cube_v2(void)
	{
		//delete[] faces_;
		//faces_ = NULL;
	}

	Color Cube_v2::GetFaceColor(Face eFace) const
	{
		return faces_[eFace];
	}

	/*
	ColorRGB Cube_v2::GetFaceColorRGB(Face eFace) const
	{ 
		//switch(GetFaceColor(eFace))
		//{
		//case Yellow:
		//	return ColorRGB::YELLOW;
		//case Red:
		//	return ColorRGB::RED;
		//case Blue:
		//	return ColorRGB::BLUE;
		//case Green:
		//	return ColorRGB::GREEN;
		//case Orange:
		//	return ColorRGB::ORANGE;
		//case White:
		//	return ColorRGB::WHITE;
		//case Black :
		//	return ColorRGB::BLACK;
		//default:
		//	//Assert
		//	return ColorRGB::BLACK;
		//}

		Color faceCol = GetFaceColor(eFace);
		return ColorRGB::RGBColors[faceCol];
	}
	*/

	/*
	Make sure Cube_v2 is positioned in right way

	// Z-axis : Back -> Front // green  -> blue
	// X-axis : Left -> Right // orange -> red
	// Y-axis : Down -> Up    // white  -> yellow

		  yellow
			Y
			|
			. --> X red
		  /
		Z
	 blue

	*/

	void Cube_v2::rotate(CVector3 rotationAxis, double rotationAngle)
	{
		if (fabs(rotationAngle) < 0.00001)
			return;

		int numRotations = fabs(rotationAngle) / 90;
		if(rotationAxis == CVector3::XAxis)
		{
			while (--numRotations > -1)
			{
				if (rotationAngle > 0)
					TiltDown();
				else
					TiltUp();					
			}
		}
		else if (rotationAxis == CVector3::YAxis)
		{
			while (--numRotations > -1)
			{
				if (rotationAngle > 0)
					TurnRight();
				else
					TurnLeft();
			}
		}
		else if (rotationAxis == CVector3::ZAxis)
		{
			while (--numRotations > -1)
			{
				if (rotationAngle > 0)
					TiltLeft();
				else
					TiltRight();
			}
		}
	}

	// Aound X axis
	void Cube_v2::TiltUp()
	{
		/*
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Back];

		faces_[Top] = faces_[Front];
		faces_[Back] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Front] = temp1;
		*/

		Color temp1 = faces_[Top];
		faces_[Top] = faces_[Front];
		faces_[Front] = faces_[Bottom];
		faces_[Bottom] = faces_[Back];
		faces_[Back] = temp1;
	}

	// Aound X axis
	void Cube_v2::TiltDown()
	{
		/*
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Front];

		faces_[Top] = faces_[Back];
		faces_[Front] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Back] = temp1;
		*/

		Color temp1 = faces_[Top];
		faces_[Top] = faces_[Back];
		faces_[Back] = faces_[Bottom];
		faces_[Bottom] = faces_[Front];
		faces_[Front] = temp1;
	}

	//Around Y axis
	void Cube_v2::TurnLeft()
	{
		/*
		Color temp1 = faces_[Front];
		Color temp2 = faces_[Left];

		faces_[Front] = faces_[Right];
		faces_[Left] = temp1;

		temp1 = faces_[Back];
		faces_[Back] = temp2;

		faces_[Right] = temp1;
		*/

		Color temp1 = faces_[Front];
		faces_[Front] = faces_[Right];
		faces_[Right] = faces_[Back];
		faces_[Back] = faces_[Left];
		faces_[Left] = temp1;
	}

	//Around Y axis
	void Cube_v2::TurnRight()
	{
		/*
		Color temp1 = faces_[Front];
		Color temp2 = faces_[Right];

		faces_[Front] = faces_[Left];
		faces_[Right] = temp1;

		temp1 = faces_[Back];
		faces_[Back] = temp2;

		faces_[Left] = temp1;
		*/

		Color temp1 = faces_[Front];
		faces_[Front] = faces_[Left];
		faces_[Left] = faces_[Back];
		faces_[Back] = faces_[Right];
		faces_[Right] = temp1;
	}

	//Around Z axis
	void Cube_v2::TiltLeft()
	{
		/*
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Left];

		faces_[Top] = faces_[Right];
		faces_[Left] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Right] = temp1;
		*/

		Color temp1 = faces_[Top];
		faces_[Top] = faces_[Right];
		faces_[Right] = faces_[Bottom];
		faces_[Bottom] = faces_[Left];
		faces_[Left] = temp1;
	}

	//Around Z axis
	void Cube_v2::TiltRight()
	{
		/*
		Color temp1 = faces_[Top];
		Color temp2 = faces_[Right];

		faces_[Top] = faces_[Left];
		faces_[Right] = temp1;

		temp1 = faces_[Bottom];
		faces_[Bottom] = temp2;

		faces_[Left] = temp1;
		*/

		Color temp1 = faces_[Top];
		faces_[Top] = faces_[Left];
		faces_[Left] = faces_[Bottom];
		faces_[Bottom] = faces_[Right];
		faces_[Right] = temp1;
	}



	CRubiksCube_v2::CRubiksCube_v2(int size)
		: //cubes_(vector< vector< vector<Cube_v2> > > (size, vector< vector<Cube_v2> >(size, vector<Cube_v2>(size)) ) ),
		//layerF_(vector< vector<Cube_v2*> >(size, vector<Cube_v2*>(size, nullptr))),
		//layerB_(vector< vector<Cube_v2*> >(size, vector<Cube_v2*>(size, nullptr))),
		//layerL_(vector< vector<Cube_v2*> >(size, vector<Cube_v2*>(size, nullptr))),
		//layerR_(vector< vector<Cube_v2*> >(size, vector<Cube_v2*>(size, nullptr))),
		//layerU_(vector< vector<Cube_v2*> >(size, vector<Cube_v2*>(size, nullptr))),
		//layerD_(vector< vector<Cube_v2*> >(size, vector<Cube_v2*>(size, nullptr))),
		size_(size),
		g_bRotating(false),
		g_bFlipRotation(false),
		g_vRotationAxis(0, 0, 0),
		g_nRotatingSection(None),
		g_nRotationAngle(0)
	{
		ResetCube();

		//for(int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerF_[i][j] = cubes_[Location_v2(i, j, size - 1)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerB_[i][j] = cubes_[Location_v2(i, j, 0)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerL_[i][j] = cubes_[Location_v2(0, i, j)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerR_[i][j] = cubes_[Location_v2(size - 1, i, j)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerU_[i][j] = cubes_[Location_v2(i, size - 1, j)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerD_[i][j] = cubes_[Location_v2(i, 0, j)].get();

		//cubes_ = new Cube_v2**[size_];

		//for (int i = 0; i < size_; i++)
		//{
		//	//cubes_[i] = new Cube_v2*[size_];

		//	for (int j = 0; j < size_; j++)
		//	{
		//		//cubes_[i][j] = new Cube_v2[size_];

		//		for (int k = 0; k < size_; k++)
		//		{
		//			cubes_[i][j][k] = CreateCube(i, j, k);
		//		}
		//	}
		//}
	}

	CRubiksCube_v2::~CRubiksCube_v2()
	{
		/*
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
		*/
	}

	CRubiksCube_v2::CRubiksCube_v2(const CRubiksCube_v2& copy)
		: //cubes_(copy.cubes_),
		size_(copy.size_),
		g_bRotating(copy.g_bRotating),
		g_bFlipRotation(copy.g_bFlipRotation),
		g_vRotationAxis(copy.g_vRotationAxis),
		g_nRotatingSection(copy.g_nRotatingSection),
		g_nRotationAngle(copy.g_nRotationAngle)
	{
		for (auto& obj : copy.cubes_)
		{
			cubes_[obj.first] = make_unique<Cube_v2>(*obj.second.get());
		}
		//cubes_ = new Cube_v2**[size_];
		/*
		for (int i = 0; i < size_; i++)
		{
			//cubes_[i] = new Cube_v2*[size_];

			for (int j = 0; j < size_; j++)
			{
				//cubes_[i][j] = new Cube_v2[size_];

				for (int k = 0; k < size_; k++)
				{
					//cubes_[i][j][k] = CreateCube(i, j, k);
				}
			}
		}
		*/
	}

	void CRubiksCube_v2::ResetCube()
	{
		g_bRotating = false;
		g_bFlipRotation = false;
		g_vRotationAxis = CVector3(0, 0, 0);
		g_nRotatingSection = None;
		g_nRotationAngle = 0;

		double extend = (size_ - 1) / 2.0;
		double x = -extend;
		for (int i = 0; i < size_; i++, ++x)
		{
			double y = -extend;
			for (int j = 0; j < size_; j++, ++y)
			{
				double z = -extend;
				for (int k = 0; k < size_; k++, ++z)
				{
					//int group = 0;
					//if (i == 0)
					//	group |= Groups::L;
					//else if (i == size_ - 1)
					//	group |= Groups::R;
					//
					//if (j == 0)
					//	group |= Groups::D;
					//else if (j == size_ - 1)
					//	group |= Groups::U;

					//if (k == 0)
					//	group |= Groups::B;
					//else if (k == size_ - 1)
					//	group |= Groups::F;

					Location_v2 loc(x, y, z);

					if (i == 0 || i == size_ - 1
						|| j == 0 || j == size_ - 1
						|| k == 0 || k == size_ - 1)
						cubes_[loc] = CreateCube(i, j, k, loc, loc.recalcGroup(size_));
				}
			}
		}
	}

	unique_ptr<Cube_v2> CRubiksCube_v2::CreateCube(double x, double y, double z, const Location_v2& location, int group)
	{
		Color left, right, top, bottom, front, back;

		if (x == 0)
		{
			left = Orange;
			right = Black;
		}
		else if (x == size_ - 1)
		{
			left = Black;
			right = Red;
		}
		else
		{
			left = Black;
			right = Black;
		}

		if (y == 0)
		{
			bottom = White;
			top = Black;
		}
		else if (y == size_ - 1)
		{
			bottom = Black;
			top = Yellow;
		}
		else
		{
			bottom = Black;
			top = Black;
		}

		if (z == 0)
		{
			back = Green;
			front = Black;
		}
		else if (z == size_ - 1)
		{
			back = Black;
			front = Blue;
		}
		else
		{
			back = Black;
			front = Black;
		}

		return make_unique<Cube_v2>(top, bottom, left, right, front, back, location, group);
	}

	const Cube_v2& CRubiksCube_v2::GetCube(double x, double y, double z)
	{
		//if (!IsValidCube(x, y, z))
		//	assert
		
		return *cubes_[Location_v2(x, y, z)];
	}

	bool CRubiksCube_v2::IsValidCube(int x, int y, int z)
	{
		return (x >= 0 && x < size_) &&
			(y >= 0 && y < size_) &&
			(z >= 0 && z < size_);
	}

	/*
	//Around Y axis
	void CRubiksCube_v2::Rotate(int section, int turns)
	{
		if (section >= 0 && section < 3)
		{
			for (int i = 0; i < size_; i++)
			{
				for (int k = 0; k < size_; k++)
				{
					for (int l = 0; l < turns; l++)
					{
						if (i == 0 || i == size_ - 1
							|| section == 0 || section == size_ - 1
							|| k == 0 || k == size_ - 1)
							cubes_[Location_v2(i, section, k)]->TurnRight();
					}
				}
			}

			/*
			Up/Down Layer
			 ____ X
			|
			|
			Z
			*//*
			
			for (int i = 0; i < turns; i++)
			{
				//if (section == 0) // layerD_
				//{
				//	int numBlocksToMove = size_ - 1;
				//	for (int p = 0; p < numBlocksToMove; ++p)
				//	{
				//		int index = numBlocksToMove - p;
				//		Cube_v2 temp1 = cubes_[p][section][index];
				//		cubes_[p][section][index] = cubes_[p][section][p];
				//		cubes_[p][section][p] = cubes_[index][section][p];
				//		cubes_[index][section][p] = cubes_[index][section][index];
				//		cubes_[index][section][index] = temp1;
				//	}
				//}
				//else if (section == size_ - 1) //layerU_
				//{
				//}

				unique_ptr<Cube_v2> temp1 = cubes_[0][section][0];
				unique_ptr<Cube_v2> temp2 = cubes_[0][section][2];

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

				// front -> right -> back -> left

				//int numBlocksToMove = size_ - 1;
				//for (int p = 0; p < numBlocksToMove; ++p)
				//{
				//	int index = numBlocksToMove - p;
				//	Cube_v2 temp1 = cubes_[p][section][index];
				//	cubes_[p][section][index] = cubes_[p][section][p];
				//	cubes_[p][section][p] = cubes_[index][section][p];
				//	cubes_[index][section][p] = cubes_[index][section][index];
				//	cubes_[index][section][index] = temp1;
				//}
				//for (int p = 0; p < numBlocksToMove; ++p)
				//{
				//	int index = numBlocksToMove - p;
				//	Cube_v2 temp1 = cubes_[0][section][index];
				//	cubes_[0][section][index] = cubes_[index][section][index];
				//	cubes_[index][section][index] = cubes_[index][section][0];
				//	cubes_[index][section][0] = cubes_[0][section][0];
				//	cubes_[0][section][0] = temp1;
				//}
			}
		}
	}

	//Around X axis
	void CRubiksCube_v2::Tilt(int section, int turns)
	{
		if (section >= 0 && section < 3)
		{
			for (int j = 0; j < size_; j++)
			{
				for (int k = 0; k < size_; k++)
				{
					for (int l = 0; l < turns; l++)
					{
						if (section == 0 || section == size_ - 1
							|| j == 0 || j == size_ - 1
							|| k == 0 || k == size_ - 1)
							cubes_[Location_v2(section, j, k)]->TiltDown();
					}
				}
			}

			for (int i = 0; i < turns; i++)
			{
				Cube_v2 temp1 = cubes_[section][0][0];
				Cube_v2 temp2 = cubes_[section][2][0];

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

	//Around Z axis
	void CRubiksCube_v2::Turn(int section, int turns)
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
						if (i == 0 || i == size_ - 1
							|| j == 0 || j == size_ - 1
							|| section == 0 || section == size_ - 1)
							cubes_[Location_v2(i, j, section)]->TiltLeft();
					}
				}
			}

			for (int i = 0; i < turns; i++)
			{
				Cube_v2 temp1 = cubes_[0][0][section];
				Cube_v2 temp2 = cubes_[2][0][section];

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
	*/

	void CRubiksCube_v2::Rotate(CVector3 rotationAxis, Groups rotatingSection, double rotationAngle)
	{
		for(auto& obj : cubes_)
		{
			const Location_v2& loc = obj.first;
			Cube_v2& cube = *obj.second.get();
			
			//if (rotatingSection == Groups::All || cube.group_ & rotatingSection)
			if (cube.group_ & rotatingSection)
			{
				cube.rotate(rotationAxis, rotationAngle);
				cube.location_.rotate(rotationAxis, rotationAngle * PI / 180.0); //Angle should be in radians
				//bool noChangeInGroupAfterRotation = (rotationAxis == CVector3::XAxis && (cube.group_ == L || cube.group_ == R))
				//	|| (rotationAxis == CVector3::YAxis && (cube.group_ == L || cube.group_ == R))
				//	|| (rotationAxis == CVector3::ZAxis && (cube.group_ == L || cube.group_ == R));
				//if (rotationAxis != getRotationAxis(cube.group_))
				cube.group_ = cube.location_.recalcGroup(size_); //Need to recalculate group always in case of any type of rotation since the cube may belong to multiple groups
			}
		}

		for (auto& obj : cubes_)
		{
			const Location_v2& loc = obj.first;
			unique_ptr<Cube_v2>& current = obj.second;

			//unique_ptr<Cube_v2> current = std::move(cube);
			while(loc != current->location_)
			{
				unique_ptr<Cube_v2> temp = std::move(cubes_[current->location_]);
				cubes_[current->location_] = std::move(current);
				current = std::move(temp);
			}
			//obj.second = std::move(current);
		}
	}

	void CRubiksCube_v2::RotateWholeRubiksCube(int axis, int turns)
	{
		/*
		axis = 0 for X-axis
		axis = 1 for Y-axis
		axis = 2 for Z-axis
		*/

		for (int section = 0; section < size_; ++section)
		{
			//if (axis == 0)
			//	Tilt(section, turns);
			//else if (axis == 1)
			//	Rotate(section, turns);
			//else if (axis == 2)
			//	Turn(section, turns);
		}
	}

	/*
	void CRubiksCube_v2::Randomize()
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
	*/

	bool CRubiksCube_v2::IsSolved()
	{
		return IsFaceSolved(Top) &&
			IsFaceSolved(Bottom) &&
			IsFaceSolved(Left) &&
			IsFaceSolved(Right) &&
			IsFaceSolved(Front) &&
			IsFaceSolved(Back);
	}

	bool CRubiksCube_v2::IsFaceSolved(Face face)
	{
		if (face == Top || face == Bottom)
		{
			int j = (face == Top) ? 2 : 0;

			Color color = cubes_[Location_v2(0, j, 0)]->GetFaceColor(face);

			for (int i = 0; i < size_; i++)
			{
				for (int k = 0; k < size_; k++)
				{
					if (cubes_[Location_v2(i, j, k)]->GetFaceColor(face) != color)
						return false;
				}
			}
		}

		else if (face == Left || face == Right)
		{
			int i = (face == Left) ? 0 : 2;

			Color color = cubes_[Location_v2(i, 0, 0)]->GetFaceColor(face);

			for (int j = 0; j < size_; j++)
			{
				for (int k = 0; k < size_; k++)
				{
					if (cubes_[Location_v2(i, j, k)]->GetFaceColor(face) != color)
						return false;
				}
			}
		}

		else if (face == Front || face == Back)
		{
			int k = (face == Front) ? 2 : 0;

			Color color = cubes_[Location_v2(0, 0, k)]->GetFaceColor(face);

			for (int i = 0; i < size_; i++)
			{
				for (int j = 0; j < size_; j++)
				{
					if (cubes_[Location_v2(i, j, k)]->GetFaceColor(face) != color)
						return false;
				}
			}
		}

		return true;
	}



	int CRubiksCube_v2::applyAlgorithm(const string& algorithm, bool animate /*= false*/, int steps /*= 0*/, RubiksCubeSolverUI* ui /*= nullptr*/)
	{
		int solutionSteps = 0;
		g_bFlipRotation = false;

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
			++solutionSteps;
		}

		return solutionSteps;
	}

	//const CVector3& CRubiksCube_v2::getRotationAxis(Groups rotationSection)
	//{
	//	switch (rotationSection)
	//	{
	//	case F:
	//		g_vRotationAxis = CVector3(0, 0, 1);
	//		break;

	//	case Z:
	//		g_vRotationAxis = CVector3(0, 0, 1);
	//		break;

	//	case B:
	//		g_vRotationAxis = CVector3(0, 0, 1);
	//		break;

	//	case L:
	//		g_vRotationAxis = CVector3(1, 0, 0);
	//		break;

	//	case X:
	//		g_vRotationAxis = CVector3(1, 0, 0);
	//		break;

	//	case R:
	//		g_vRotationAxis = CVector3(1, 0, 0);
	//		break;

	//	case U:
	//		g_vRotationAxis = CVector3(0, 1, 0);
	//		break;

	//	case Y:
	//		g_vRotationAxis = CVector3(0, 1, 0);
	//		break;

	//	case D:
	//		g_vRotationAxis = CVector3(0, 1, 0);
	//		break;

	//	default:
	//		assert(false);
	//		break;
	//	}
	//}

	void CRubiksCube_v2::applyStep(const char& face, bool isPrime, int numRotations, bool animate /*= false*/, int steps /*= 0*/, RubiksCubeSolverUI* ui /*= nullptr*/)
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
			g_nRotatingSection = F;
			g_nRotationAngle = -90;
			break;

		case 'Z':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = All;
			g_nRotationAngle = 90;
			break;

		case 'B':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = B;
			g_nRotationAngle = 90;
			break;

		case 'L':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = L;
			g_nRotationAngle = 90;
			break;

		case 'X':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = All;
			g_nRotationAngle = 90;
			break;

		case 'R':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = R;
			g_nRotationAngle = -90;
			break;

		case 'U':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = U;
			g_nRotationAngle = -90;
			break;

		case 'Y':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = All;
			g_nRotationAngle = 90;
			break;

		case 'D':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = D;
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
			g_bRotating = true;
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
			g_bRotating = false;
		}

		//Fix cube position and Reset all parameters
		fixRubiksCubeFaces();
		g_vRotationAxis = CVector3{0.0, 0.0, 0.0};
		g_nRotationAngle = 0;
		g_nRotatingSection = None;
	}


	void CRubiksCube_v2::fixRubiksCubeFaces()
	{
		//int turns = 0;
		//if (g_nRotationAngle == 0)
		//	turns = 0;
		//else if (g_nRotationAngle == 90)
		//	turns = 1;
		//else if (g_nRotationAngle == 180 || g_nRotationAngle == -180)
		//	turns = 2;
		//else if (g_nRotationAngle == -90)
		//	turns = 3;

		//if (g_vRotationAxis.x)
		//	Tilt(g_nRotatingSection, turns);
		//else if (g_vRotationAxis.y)
		//	Rotate(g_nRotatingSection, turns);
		//else if (g_vRotationAxis.z)
		//	Turn(g_nRotatingSection, turns);

		Rotate(g_vRotationAxis, g_nRotatingSection, g_nRotationAngle);
	}

}

