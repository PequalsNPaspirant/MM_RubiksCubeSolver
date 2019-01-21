//=======================================================================================================//
//   Copyright (c) 2018 Maruti Mhetre                                                                    //
//   All rights reserved.                                                                                //
//=======================================================================================================//
//   Redistribution and use of this software in source and binary forms, with or without modification,   //
//   are permitted for personal, educational or non-commercial purposes provided that the following      //
//   conditions are met:                                                                                 //
//   1. Redistributions of source code must retain the above copyright notice, this list of conditions   //
//      and the following disclaimer.                                                                    //
//   2. Redistributions in binary form must reproduce the above copyright notice, this list of           //
//      conditions and the following disclaimer in the documentation and/or other materials provided     //
//      with the distribution.                                                                           //
//   3. Neither the name of the copyright holder nor the names of its contributors may be used to        //
//      endorse or promote products derived from this software without specific prior written            //
//      permission.                                                                                      //
//=======================================================================================================//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR      //
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND    //
//   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          //
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   //
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   //
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  //
//   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT   //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     //
//=======================================================================================================//

#include "StdAfx.h"

#include <time.h>
#include <cassert>
#include <memory>
#include <chrono>
using namespace std;

#include "Resource.h"
#include "RubiksCubeModel_v5.h"
#include "RubiksCubeSolverUI.h"
#include "RubiksCubeSolverUtils.h"

namespace mm {

	//Factory function definition
	unique_ptr<RubiksCubeModel> createRubiksCubeModel_v5(int size)
	{
		return make_unique<RubiksCubeModel_v5>(size);
	}

	//==================== RubiksCubeModel_v5::Cube =========================

	const int RubiksCubeModel_v5::Cube::FACE_COUNT /* = 6*/;
	const double RubiksCubeModel_v5::scale_ = 1.0;

	const RubiksCubeModel_v5::ColorRGB RubiksCubeModel_v5::ColorRGB::RGBColors[7] = {
		ColorRGB{ 255, 255, 0 },
		ColorRGB{ 255, 0, 0 },
		ColorRGB{ 0, 0, 255 },
		ColorRGB{ 0, 255, 0 },
		ColorRGB{ 255, 165, 0 },
		ColorRGB{ 255, 255, 255 },
		ColorRGB{ 0, 0, 0 }
	};

	RubiksCubeModel_v5::Cube::Cube(Color cTop, Color cBottom, Color cLeft, Color cRight, Color cFront, Color cBack, const Location& location, int cubeSize)
		: faces_(FACE_COUNT)
	{
		faces_[Up] = cTop;
		faces_[Down] = cBottom;
		faces_[Left] = cLeft;
		faces_[Right] = cRight;
		faces_[Front] = cFront;
		faces_[Back] = cBack;

		location_ = location;
		cubeSize_ = cubeSize;
		//group_ = group;
	}

	RubiksCubeModel_v5::Cube::~Cube(void)
	{
	}

	Color RubiksCubeModel_v5::Cube::GetFaceColor(Face eFace) const
	{
		return faces_[eFace];
	}

	/*
	Make sure Cube is positioned in right way

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

	void RubiksCubeModel_v5::Cube::rotate(CVector3 rotationAxis, double rotationAngle)
	{
		if (fabs(rotationAngle) < 0.00001)
			return;

		location_.rotate(rotationAxis, rotationAngle);

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
	void RubiksCubeModel_v5::Cube::TiltUp()
	{
		Color temp1 = faces_[Up];
		faces_[Up] = faces_[Front];
		faces_[Front] = faces_[Down];
		faces_[Down] = faces_[Back];
		faces_[Back] = temp1;
	}

	// Aound X axis
	void RubiksCubeModel_v5::Cube::TiltDown()
	{
		Color temp1 = faces_[Up];
		faces_[Up] = faces_[Back];
		faces_[Back] = faces_[Down];
		faces_[Down] = faces_[Front];
		faces_[Front] = temp1;
	}

	//Around Y axis
	void RubiksCubeModel_v5::Cube::TurnLeft()
	{
		Color temp1 = faces_[Front];
		faces_[Front] = faces_[Right];
		faces_[Right] = faces_[Back];
		faces_[Back] = faces_[Left];
		faces_[Left] = temp1;
	}

	//Around Y axis
	void RubiksCubeModel_v5::Cube::TurnRight()
	{
		Color temp1 = faces_[Front];
		faces_[Front] = faces_[Left];
		faces_[Left] = faces_[Back];
		faces_[Back] = faces_[Right];
		faces_[Right] = temp1;
	}

	//Around Z axis
	void RubiksCubeModel_v5::Cube::TiltLeft()
	{
		Color temp1 = faces_[Up];
		faces_[Up] = faces_[Right];
		faces_[Right] = faces_[Down];
		faces_[Down] = faces_[Left];
		faces_[Left] = temp1;
	}

	//Around Z axis
	void RubiksCubeModel_v5::Cube::TiltRight()
	{
		Color temp1 = faces_[Up];
		faces_[Up] = faces_[Left];
		faces_[Left] = faces_[Down];
		faces_[Down] = faces_[Right];
		faces_[Right] = temp1;
	}

	bool RubiksCubeModel_v5::Cube::belongsTo(Face rotatingSection, int layerIndex, int extend) const
	{
		if (rotatingSection == All)
			return true;

		RubiksCubeSolverUtils::RunTimeAssert(layerIndex > 0);

		//double extend_ = (size - 1) / 2.0;
		/*
		Up = 0,
		Down = 1,
		Left = 2,
		Right = 3,
		Front = 4,
		Back = 5,
		*/
		static int diffData[6] = {-1, 1, 1, -1, -1, 1};
		double diff = cubeSize_ * diffData[rotatingSection] * (layerIndex - 1);

		double x, y, z;
		x = y = z = -1;
		bool retVal = false;
		switch (rotatingSection)
		{
		case Left:
			x = -extend + diff;
			retVal = (fabs(x - location_.x_) < 0.0001);
			break;
		case Right:
			x = extend + diff;
			retVal = (fabs(x - location_.x_) < 0.0001);
			break;
		case Down:
			y = -extend + diff;
			retVal = (fabs(y - location_.y_) < 0.0001);
			break;
		case Up:
			y = extend + diff;
			retVal = (fabs(y - location_.y_) < 0.0001);
			break;
		case Back:
			z = -extend + diff;
			retVal = (fabs(z - location_.z_) < 0.0001);
			break;
		case Front:
			z = extend + diff;
			retVal = (fabs(z - location_.z_) < 0.0001);
			break;
		}
		
		return retVal;
	}

	//==================== RubiksCubeModel_v5 =========================

	RubiksCubeModel_v5::RubiksCubeModel_v5(int size)
		: //cubes_(vector< vector< vector<Cube> > > (size, vector< vector<Cube> >(size, vector<Cube>(size)) ) ),
		//layerF_(vector< vector<Cube*> >(size, vector<Cube*>(size, nullptr))),
		//layerB_(vector< vector<Cube*> >(size, vector<Cube*>(size, nullptr))),
		//layerL_(vector< vector<Cube*> >(size, vector<Cube*>(size, nullptr))),
		//layerR_(vector< vector<Cube*> >(size, vector<Cube*>(size, nullptr))),
		//layerU_(vector< vector<Cube*> >(size, vector<Cube*>(size, nullptr))),
		//layerD_(vector< vector<Cube*> >(size, vector<Cube*>(size, nullptr))),
		//cubes_(27),
		size_(size),
		cubeSize_(2),
		extend_(cubeSize_ * (size - 1) / 2.0)
		//g_bRotating(false),
		//g_bFlipRotation(false),
		//g_vRotationAxis(0, 0, 0),
		//g_nRotatingSection(None),
		//g_nRotationAngle(0)
	{
		ResetCube();

		//for(int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerF_[i][j] = cubes_[Location(i, j, size - 1)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerB_[i][j] = cubes_[Location(i, j, 0)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerL_[i][j] = cubes_[Location(0, i, j)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerR_[i][j] = cubes_[Location(size - 1, i, j)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerU_[i][j] = cubes_[Location(i, size - 1, j)].get();

		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		layerD_[i][j] = cubes_[Location(i, 0, j)].get();

	}

	RubiksCubeModel_v5::RubiksCubeModel_v5(const RubiksCubeModel_v5& copy)
		: //cubes_(copy.cubes_),
		size_(copy.size_),
		cubeSize_(copy.cubeSize_),
		extend_(copy.extend_),
		g_bRotating(copy.g_bRotating),
		g_bFlipRotation(copy.g_bFlipRotation),
		g_vRotationAxis(copy.g_vRotationAxis),
		g_nRotatingSection(copy.g_nRotatingSection),
		g_nLayerIndex(copy.g_nLayerIndex),
		g_nRotationAngle(copy.g_nRotationAngle)
	{
		for (auto& obj : copy.cubes_)
		{
			cubes_[obj.first] = make_unique<Cube>(*obj.second.get());
		}
	}

	RubiksCubeModel_v5::~RubiksCubeModel_v5()
	{
	}

	void RubiksCubeModel_v5::ResetCube()
	{
		g_bRotating = false;
		g_bFlipRotation = false;
		g_vRotationAxis = CVector3(0, 0, 0);
		g_nRotatingSection = None;
		g_nLayerIndex = 1;
		g_nRotationAngle = 0;

		double x = -extend_;
		for (int i = 0; i < size_; i++, x += cubeSize_)
		{
			double y = -extend_;
			for (int j = 0; j < size_; j++, y += cubeSize_)
			{
				double z = -extend_;
				for (int k = 0; k < size_; k++, z += cubeSize_)
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

					Location loc(x, y, z);

					if (i == 0 || i == size_ - 1
						|| j == 0 || j == size_ - 1
						|| k == 0 || k == size_ - 1)
						cubes_[loc] = CreateCube(i, j, k, loc);
				}
			}
		}
	}

	string RubiksCubeModel_v5::solve(unsigned int& solutionSteps, unsigned long long& duration, bool animate, RubiksCubeSolverUI& ui)
	{
		//string solution;
		//if (size_ == 2)
		//{
		//	RubiksCubeSolver_2x2x2 solver(*this, animate, ui);
		//	using HRClock = std::chrono::high_resolution_clock;
		//	HRClock::time_point start_time = HRClock::now();
		//	solution = solver.solve(solutionSteps);
		//	HRClock::time_point end_time = HRClock::now();
		//	std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		//	duration = time_span.count();
		//}
		//else if (size_ == 3)
		//{
		//	RubiksCubeSolver_3x3x3 solver(*this, animate, ui);
		//	using HRClock = std::chrono::high_resolution_clock;
		//	HRClock::time_point start_time = HRClock::now();
		//	solution = solver.solve(solutionSteps);
		//	HRClock::time_point end_time = HRClock::now();
		//	std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		//	duration = time_span.count();
		//}

		RubiksCubeSolver_NxNxN solver(*this, animate, ui);
		using HRClock = std::chrono::high_resolution_clock;
		HRClock::time_point start_time = HRClock::now();
		string solution = solver.solve(solutionSteps);
		HRClock::time_point end_time = HRClock::now();
		std::chrono::nanoseconds time_span = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		duration = time_span.count();

		return solution;
	}

	void RubiksCubeModel_v5::render()
	{
#ifdef _DEBUG
		// Draw Axis
		glBegin(GL_LINES);
		// x
		glColor3f(1.0f, 0.6f, 0.0f); // orange
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(scale_ * cubeSize_ * 3, 0.0, 0.0);
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glVertex3d(scale_ * cubeSize_ * 3, 0.0, 0.0);
		glVertex3d(scale_ * cubeSize_ * 4.5f, 0.0, 0.0);

		// y
		//glColor3f(0.0f, 1.0f, 0.0f);  // green
		glColor3f(1.0f, 1.0f, 1.0f);  // white
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, scale_ * cubeSize_ * 3, 0.0);
		glColor3f(1.0f, 1.0f, 0.0f);  // yellow
		glVertex3d(0.0, scale_ * cubeSize_ * 3, 0.0);
		glVertex3d(0.0, scale_ * cubeSize_ * 4.5f, 0.0);

		// z
		glColor3f(0.0f, 1.0f, 0.0f);  // green
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, scale_ * cubeSize_ * 3);
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glVertex3d(0.0, 0.0, scale_ * cubeSize_ * 3);
		glVertex3d(0.0, 0.0, scale_ * cubeSize_ * 4.5f);
		glEnd();
#endif

		glInitNames();

		for (auto& obj : cubes_)
		{
			const Location& loc = obj.first;
			const Cube& cube = *obj.second.get();

			glPushMatrix();

			if (g_bRotating)
			{
				if (cube.belongsTo(g_nRotatingSection, g_nLayerIndex, extend_))
				{
					int angle = g_bFlipRotation ? -g_nRotationAngle : g_nRotationAngle;
					glRotated(angle, g_vRotationAxis.x, g_vRotationAxis.y, g_vRotationAxis.z);
				}
			}

			renderIndividualCube(cube, cube.getLocation());

			glPopMatrix();
		}
	}

	void RubiksCubeModel_v5::renderIndividualCube(const RubiksCubeModel_v5::Cube& pCube, const RubiksCubeModel_v5::Location& location)
	{
		//TODO:
		// draw back faces only for the rotating section and the neighbouring sections

		double x = location.x_;
		double y = location.y_;
		double z = location.z_;
		//bool mirrorVisibleFaces = true;
		int offsetDist = (1 + size_) * cubeSize_; //distance of mirror image plane from the cube face
		const float textureExtend = cubeSize_ / 2.0;

		glPushName(x);
		glPushName(y);
		glPushName(z);

		// scale to -1 to +1
		//x--;
		//y--;
		//z--;

		glPushMatrix();

		glTranslated(x * scale_, y * scale_, z * scale_);

		Color top = pCube.GetFaceColor(Up);
		Color bottom = pCube.GetFaceColor(Down);
		Color left = pCube.GetFaceColor(Left);
		Color right = pCube.GetFaceColor(Right);
		Color back = pCube.GetFaceColor(Back);
		Color front = pCube.GetFaceColor(Front);

		glEnable(GL_TEXTURE_2D);

		//if (front != Color::Black)
		{
			// Front Face
			glPushName((GLuint)Front);
			glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(front));
			glBegin(GL_QUADS);
			ColorRGB colRgb = ColorRGB::RGBColors[front];
			glColor3ub(colRgb.r, colRgb.g, colRgb.b);
			glNormal3f(0.0f, 0.0f, 1.0f);
			//glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2d(0.0, 0.0); glVertex3f(-textureExtend, -textureExtend, textureExtend);	// Bottom Left Of The Texture and Quad
			glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
			glEnd();
			glPopName();

			//if (mirrorVisibleFaces && g_bRotating && fabs(z - extend_) < 0.0001)
			if (fabs(z - extend_) < 0.0001)
			{
				glPushMatrix();
				glTranslated(0, 0, offsetDist * scale_);

				// Mirror Front Face
				glPushName((GLuint)Front);
				glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(front));
				glBegin(GL_QUADS);
				ColorRGB colRgb = ColorRGB::RGBColors[front];
				glColor3ub(colRgb.r, colRgb.g, colRgb.b);
				glNormal3f(0.0f, 0.0f, -1.0f);
				glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
				glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
				glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
				glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad			

				glEnd();
				glPopName();

				glTranslated(0, 0, -offsetDist * scale_);
				glPushMatrix();
			}
		}

		//if (Back != Color::Black)
		{
			// Back Face
			glPushName((GLuint)Back);
			glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(back));
			glBegin(GL_QUADS);
			ColorRGB colRgb = ColorRGB::RGBColors[back];
			glColor3ub(colRgb.r, colRgb.g, colRgb.b);
			glNormal3f(0.0f, 0.0f, -1.0f);
			glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glEnd();
			glPopName();

			if (fabs(z - -extend_) < 0.0001)
			{
				glPushMatrix();
				glTranslated(0, 0, -offsetDist * scale_);

				// Mirror Back Face
				glPushName((GLuint)Back);
				glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(back));
				glBegin(GL_QUADS);
				colRgb = ColorRGB::RGBColors[back];
				glColor3ub(colRgb.r, colRgb.g, colRgb.b);
				glNormal3f(0.0f, 0.0f, +1.0f);
				glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
				glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
				glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
				glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad			

				glEnd();
				glPopName();

				glTranslated(0, 0, offsetDist * scale_);
				glPushMatrix();
			}
		}

		//if (Up != Color::Black)
		{
			// Up Face
			glPushName((GLuint)Up);
			glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(top));
			glBegin(GL_QUADS);
			ColorRGB colRgb = ColorRGB::RGBColors[top];
			glColor3ub(colRgb.r, colRgb.g, colRgb.b);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glEnd();
			glPopName();

			//if (mirrorVisibleFaces && g_bRotating && fabs(y - extend_) < 0.0001)
			if (fabs(y - extend_) < 0.0001)
			{
				glPushMatrix();
				glTranslated(0, (offsetDist + 0) * scale_, 0);

				// Mirror Up Face
				glPushName((GLuint)Up);
				glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(top));
				glBegin(GL_QUADS);
				colRgb = ColorRGB::RGBColors[top];
				glColor3ub(colRgb.r, colRgb.g, colRgb.b);
				glNormal3f(0.0f, -1.0f, 0.0f);
				glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
				glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
				glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
				glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Texture and Quad

				glEnd();
				glPopName();

				glTranslated(0, -(offsetDist + 0) * scale_, 0);
				glPushMatrix();
			}
		}

		//if (Down != Color::Black)
		{
			// Down Face
			glPushName((GLuint)Down);
			glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(bottom));
			glBegin(GL_QUADS);
			ColorRGB colRgb = ColorRGB::RGBColors[bottom];
			glColor3ub(colRgb.r, colRgb.g, colRgb.b);
			glNormal3f(0.0f, -1.0f, 0.0f);
			glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
			glEnd();
			glPopName();

			if (fabs(y - -extend_) < 0.0001)
			{
				glPushMatrix();
				glTranslated(0, -(offsetDist + 1) * scale_, 0);

				// Down Face
				glPushName((GLuint)Down);
				glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(bottom));
				glBegin(GL_QUADS);
				colRgb = ColorRGB::RGBColors[bottom];
				glColor3ub(colRgb.r, colRgb.g, colRgb.b);
				glNormal3f(0.0f, +1.0f, 0.0f);
				glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
				glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
				glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
				glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad

				glEnd();
				glPopName();

				glTranslated(0, (offsetDist + 1) * scale_, 0);
				glPushMatrix();
			}
		}

		//if (Right != Color::Black)
		{
			// Right face
			glPushName((GLuint)Right);
			glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(right));
			glBegin(GL_QUADS);
			ColorRGB colRgb = ColorRGB::RGBColors[right];
			glColor3ub(colRgb.r, colRgb.g, colRgb.b);
			glNormal3f(1.0f, 0.0f, 0.0f);
			glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
			glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
			glEnd();
			glPopName();

			//if (mirrorVisibleFaces && g_bRotating && fabs(x - extend_) < 0.0001)
			if (fabs(x - extend_) < 0.0001)
			{
				glPushMatrix();
				glTranslated(offsetDist * scale_, 0, 0);

				// Mirror Right face
				glPushName((GLuint)Right);
				glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(right));
				glBegin(GL_QUADS);
				colRgb = ColorRGB::RGBColors[right];
				glColor3ub(colRgb.r, colRgb.g, colRgb.b);
				glNormal3f(-1.0f, 0.0f, 0.0f);
				glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
				glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
				glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
				glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad

				glEnd();
				glPopName();

				glTranslated(-offsetDist * scale_, 0, 0);
				glPushMatrix();
			}
		}

		//if (Left != Color::Black)
		{
			// Left Face
			glPushName((GLuint)Left);
			glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(left));
			glBegin(GL_QUADS);
			ColorRGB colRgb = ColorRGB::RGBColors[left];
			glColor3ub(colRgb.r, colRgb.g, colRgb.b);
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
			glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
			glEnd();
			glPopName();

			if (fabs(x - -extend_) < 0.0001)
			{
				glPushMatrix();
				glTranslated(-offsetDist * scale_, 0, 0);

				// Mirror Left Face
				glPushName((GLuint)Left);
				glBindTexture(GL_TEXTURE_2D, Textures::getTextureID(left));
				glBegin(GL_QUADS);
				colRgb = ColorRGB::RGBColors[left];
				glColor3ub(colRgb.r, colRgb.g, colRgb.b);
				glNormal3f(+1.0f, 0.0f, 0.0f);
				glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
				glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
				glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
				glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad

				glEnd();
				glPopName();

				glTranslated(offsetDist * scale_, 0, 0);
				glPushMatrix();
			}
		}

		glPopName();
		glPopName();
		glPopName();

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	unique_ptr<RubiksCubeModel_v5::Cube> RubiksCubeModel_v5::CreateCube(double x, double y, double z, const Location& location)
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

		return make_unique<Cube>(top, bottom, left, right, front, back, location, cubeSize_);
	}

	//const RubiksCubeModel_v5::Cube& RubiksCubeModel_v5::GetCube(double x, double y, double z)
	//{
	//	//if (!IsValidCube(x, y, z))
	//	//	RubiksCubeSolverUtils::RunTimeAssert
	//	
	//	return *cubes_[Location(cubeSize_ * (x - 1), cubeSize_ * (y - 1), cubeSize_ * (z - 1))];
	//}

	
	RubiksCubeModel_v5::Cube& RubiksCubeModel_v5::GetCube(Face layer1, int layerIndex1, Face layer2, int layerIndex2, Face layer3, int layerIndex3)
	{
		RubiksCubeSolverUtils::RunTimeAssert(layer1 != layer2 && layer2 != layer3);

		double x, y, z;
		//double extend_ = (size_ - 1) / 2.0;
		for (int i = 0; i < 3; ++i)
		{
			Face layer;
			int index = 1;
			if (i == 0)
			{
				layer = layer1;
				index = layerIndex1 - 1;
			}
			else if (i == 1)
			{
				layer = layer2;
				index = layerIndex2 - 1;
			}
			else if (i == 2)
			{
				layer = layer3;
				index = layerIndex3 - 1;
			}

			switch (layer)
			{
			case Left:
				x = -extend_ + cubeSize_ * index;
				break;
			case Right:
				x = extend_ - cubeSize_ * index;
				break;
			case Down:
				y = -extend_ + cubeSize_ * index;
				break;
			case Up:
				y = extend_ - cubeSize_ * index;
				break;
			case Back:
				z = -extend_ + cubeSize_ * index;
				break;
			case Front:
				z = extend_ - cubeSize_ * index;
				break;
			}
		}

		return *cubes_[Location(x, y, z)];		
	}

	bool RubiksCubeModel_v5::IsValidCube(int x, int y, int z)
	{
		return (x >= 0 && x < size_) &&
			(y >= 0 && y < size_) &&
			(z >= 0 && z < size_);
	}

	bool RubiksCubeModel_v5::isSolved()
	{
		return IsFaceSolved(Up) &&
			IsFaceSolved(Down) &&
			IsFaceSolved(Left) &&
			IsFaceSolved(Right) &&
			IsFaceSolved(Front) &&
			IsFaceSolved(Back);
	}

	bool RubiksCubeModel_v5::IsFaceSolved(RubiksCubeModel_v5::Face face)
	{
		//double extend_ = (size_ - 1) / 2.0;

		if (face == Left || face == Right)
		{
			double iStart = (face == Left) ? -extend_ : extend_;

			Color color = cubes_[Location(iStart, -extend_, -extend_)]->GetFaceColor(face);

			double jStart = -extend_;
			for (int j = 0; j < size_; j++, jStart += cubeSize_)
			{
				double kStart = -extend_;
				for (int k = 0; k < size_; k++, kStart += cubeSize_)
				{
					if (cubes_[Location(iStart, jStart, kStart)]->GetFaceColor(face) != color)
						return false;
				}
			}
		}
		else if (face == Up || face == Down)
		{
			double jStart = (face == Down) ? -extend_ : extend_;

			Color color = cubes_[Location(-extend_, jStart, -extend_)]->GetFaceColor(face);

			double iStart = -extend_;
			for (int i = 0; i < size_; i++, iStart += cubeSize_)
			{
				double kStart = -extend_;
				for (int k = 0; k < size_; k++, kStart += cubeSize_)
				{
					if (cubes_[Location(iStart, jStart, kStart)]->GetFaceColor(face) != color)
						return false;
				}
			}
		}
		else if (face == Front || face == Back)
		{
			double kStart = (face == Back) ? -extend_ : extend_;

			Color color = cubes_[Location(-extend_, -extend_, kStart)]->GetFaceColor(face);

			double iStart = -extend_;
			for (int i = 0; i < size_; i++, iStart += cubeSize_)
			{
				double jStart = -extend_;
				for (int j = 0; j < size_; j++, jStart += cubeSize_)
				{
					if (cubes_[Location(iStart, jStart, kStart)]->GetFaceColor(face) != color)
						return false;
				}
			}
		}

		return true;
	}

	unique_ptr<RubiksCubeModel> RubiksCubeModel_v5::copy()
	{
		return make_unique<RubiksCubeModel_v5>(*this);
	}

	string RubiksCubeModel_v5::getModelName()
	{
		return "RubiksCubeModel_v1";
	}

	int RubiksCubeModel_v5::getDimension()
	{
		return size_;
	}

	int RubiksCubeModel_v5::applyAlgorithm(const string& algorithm, bool animate, RubiksCubeSolverUI& ui)
	{
		int solutionSteps = 0;
		g_bFlipRotation = false;

		for (int i = 0; i < algorithm.length();)
		{
			char face = algorithm[i];
			if (face == ' ')
				continue;

			int layerIndex = 1;
			++i;
			if (i < algorithm.length() && algorithm[i] == '(')
			{
				int layer = 0;
				for(++i; i < algorithm.length() && algorithm[i] != ')'; ++i)
					layer = layer * 10 + (algorithm[i] - '0');

				layerIndex = layer;
				++i;
			}

			// Check if prime operation
			bool isPrime = false;
			if (i < algorithm.length() && algorithm[i] == '\'')
			{
				isPrime = true;
				++i;
			}
			// Check if multiple rotations
			int numRotations = 1;
			int rotations = 0;
			for(; i < algorithm.length() && '0' <= algorithm[i] && algorithm[i] <= '9'; ++i)
			{
				rotations = rotations * 10 + (numRotations = algorithm[i] - '0');
			}
			if (rotations > 0)
				numRotations = rotations;

			applyStep(face, layerIndex, isPrime, numRotations, animate, ui);
			++solutionSteps;
		}

		return solutionSteps;
	}

	//const CVector3& RubiksCubeModel_v5::getRotationAxis(Groups rotationSection)
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
	//		RubiksCubeSolverUtils::RunTimeAssert(false);
	//		break;
	//	}
	//}

	void RubiksCubeModel_v5::applyStep(const char& face, int layerIndex, bool isPrime, int numRotations, bool animate, RubiksCubeSolverUI& ui)
	//void RubiksCubeModel_v5::applyStep(const char& face, bool isPrime, int layerIndex, bool animate /*= false*/, int steps /*= 0*/, RubiksCubeSolverUI* ui /*= nullptr*/)
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
			g_nRotatingSection = Front;
			g_nRotationAngle = -90;
			break;

		case 'Z':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = All;
			g_nRotationAngle = 90;
			break;

		case 'B':
			g_vRotationAxis = CVector3(0, 0, 1);
			g_nRotatingSection = Back;
			g_nRotationAngle = 90;
			break;

		case 'L':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = Left;
			g_nRotationAngle = 90;
			break;

		case 'X':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = All;
			g_nRotationAngle = 90;
			break;

		case 'R':
			g_vRotationAxis = CVector3(1, 0, 0);
			g_nRotatingSection = Right;
			g_nRotationAngle = -90;
			break;

		case 'U':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = Up;
			g_nRotationAngle = -90;
			break;

		case 'Y':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = All;
			g_nRotationAngle = 90;
			break;

		case 'D':
			g_vRotationAxis = CVector3(0, 1, 0);
			g_nRotatingSection = Down;
			g_nRotationAngle = 90;
			break;

		default:
			//RubiksCubeSolverUtils::RunTimeAssert
			break;
		}

		g_nRotationAngle = g_nRotationAngle * numRotations;
		g_nLayerIndex = layerIndex;
		if (isPrime)
			g_nRotationAngle = -g_nRotationAngle;

		if (animate)
		{
			g_bRotating = true;
			int angle = g_nRotationAngle;
			g_nRotationAngle = 0;
			int step = (angle - g_nRotationAngle) / ui.getFramesPerRotation();
			for (int i = 0; i < ui.getFramesPerRotation(); ++i)
			{
				g_nRotationAngle += step;
				ui.redrawWindow();
				Sleep(ui.getSleepTimeMilliSec());
			}

			//If after above loop, the target angle is not achieved
			if (g_nRotationAngle != angle)
			{
				g_nRotationAngle = angle;
				ui.redrawWindow();
			}
			g_bRotating = false;
		}

		//Fix cube position and Reset all parameters
		fixRubiksCubeFaces();
		if (animate)
			ui.redrawWindow();
		g_vRotationAxis = CVector3{0.0, 0.0, 0.0};
		g_nRotationAngle = 0;
		g_nRotatingSection = None;
	}


	void RubiksCubeModel_v5::fixRubiksCubeFaces()
	{
		Rotate(g_vRotationAxis, g_nRotatingSection, g_nLayerIndex, g_nRotationAngle);
	}

	void RubiksCubeModel_v5::Rotate(CVector3 rotationAxis, RubiksCubeModel_v5::Face rotatingSection, int layerIndex, double rotationAngle)
	{
		if (rotatingSection == RubiksCubeModel_v5::Face::All)
		{
			for (auto& obj : cubes_)
			{
				//const Location& loc = obj.first;
				Cube& cube = *obj.second.get();
				cube.rotate(rotationAxis, rotationAngle);
			}

			for (auto& obj : cubes_)
			{
				const Location& loc = obj.first;
				unique_ptr<Cube>& current = obj.second;

				while (loc != current->getLocation())
				{
					unique_ptr<Cube> temp = std::move(cubes_[current->getLocation()]);
					cubes_[current->getLocation()] = std::move(current);
					current = std::move(temp);
				}
			}

			return;
		}

		//double extend_ = (size_ - 1) / 2.0;
		static int diffData[6] = { -1, 1, 1, -1, -1, 1 };
		double diff = cubeSize_ * diffData[rotatingSection] * (layerIndex - 1);
		double x, y, z;
		double *pi, *pj;
		switch (rotatingSection)
		{
		case RubiksCubeModel_v5::Face::Left:
			x = -extend_ + diff;
			pi = &y;
			pj = &z;
			break;
		case RubiksCubeModel_v5::Face::Right:
			x = +extend_ + diff;
			pi = &y;
			pj = &z;
			break;
		case RubiksCubeModel_v5::Face::Down:
			pi = &x;
			y = -extend_ + diff;
			pj = &z;
			break;
		case RubiksCubeModel_v5::Face::Up:
			pi = &x;
			y = +extend_ + diff;
			pj = &z;
			break;
		case RubiksCubeModel_v5::Face::Back:
			pi = &x;
			pj = &y;
			z = -extend_ + diff;
			break;
		case RubiksCubeModel_v5::Face::Front:
			pi = &x;
			pj = &y;
			z = +extend_ + diff;
			break;
		case RubiksCubeModel_v5::Face::All:
		default:
			RubiksCubeSolverUtils::RunTimeAssert(false, "Unrecognized face");
		}

		*pi = -extend_;
		for (int i = 0; i < size_; ++i, *pi += cubeSize_)
		{
			*pj = -extend_;
			for (int j = 0; j < size_; ++j, *pj += cubeSize_)
			{
				if (fabs(fabs(x) - extend_) > 0.0001 && fabs(fabs(y) - extend_) > 0.0001 && fabs(fabs(z) - extend_) > 0.0001)
					continue;

				auto it = cubes_.find(Location(x, y, z));
				RubiksCubeSolverUtils::RunTimeAssert(it != cubes_.end());
				//const Location& loc = it->first;
				unique_ptr<Cube>& current = it->second;
				current->rotate(rotationAxis, rotationAngle);
			}
		}

		*pi = -extend_;
		for (int i = 0; i < size_; ++i, *pi += cubeSize_)
		{
			*pj = -extend_;
			for (int j = 0; j < size_; ++j, *pj += cubeSize_)
			{
				if (fabs(fabs(x) - extend_) > 0.0001 && fabs(fabs(y) - extend_) > 0.0001 && fabs(fabs(z) - extend_) > 0.0001)
					continue;

				auto it = cubes_.find(Location(x, y, z));
				RubiksCubeSolverUtils::RunTimeAssert(it != cubes_.end());
				const Location& loc = it->first;
				unique_ptr<Cube>& current = it->second;

				while (loc != current->getLocation())
				{
					unique_ptr<Cube> temp = std::move(cubes_[current->getLocation()]);
					cubes_[current->getLocation()] = std::move(current);
					current = std::move(temp);
				}
			}
		}
	}

	string RubiksCubeModel_v5::getScramblingAlgo(int length, bool includeNonStandardRotations)
	{
		vector<char> charSet{
			//Standard rotations
			'F', //Front
			'B', //Back
			'L', //Left
			'R', //Right
			'U', //Up
			'D',  //Down

			//Non-Standard rotations
			'X', //whole cube in X Axis direction X = L + R + center
			'Y', //whole cube in Y Axis direction Y = U + D + center
			'Z'  //whole cube in Z Axis direction Z = F + B + center
		};

		//int numNotations = sizeof(charSet) / sizeof(char);
		int numNotations = charSet.size();
		//int wholeCubeRotateNotations = 3; // 'X', 'Y' and 'Z'
		//int numSingleLayerRotateNotations = numNotations - wholeCubeRotateNotations;
		const int standardRotations = 6;
		if (!includeNonStandardRotations)
			numNotations = standardRotations;
		string retVal;
		for (int i = 0; i < length; ++i)
		{
			int index = rand() % numNotations;
			retVal += charSet[index];

			int layerIndex = rand() % (size_ - 1);
			if(layerIndex > 1)
				retVal += ("(" + to_string(layerIndex) + ")");

			//Generate 50% prime rotations
			bool isPrime = (rand() % 2) == 1;
			if (isPrime)
				retVal += '\'';
			
			//Generate double rotations 1/3 times
			if (index < standardRotations) //Avoid X, Y and Z since layerIndex is not applicable for it
			{
				int rotations = rand() % 3; 
				if(rotations > 1)
					retVal += to_string(rotations);
			}
		}

		return retVal;
	}


	//=======================================================================================================
	// Solver NxNxN
	//=======================================================================================================

	RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::RubiksCubeSolver_NxNxN(RubiksCubeModel_v5& rubiksCube, bool animate, RubiksCubeSolverUI& ui)
		: rubiksCube_(rubiksCube),
		solutionSteps_(0),
		animate_(animate),
		ui_(ui),
		solvedEdges_(0)
	{
	}

	string RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::solve(unsigned int& solutionSteps)
	{
		solutionSteps_ = 0;
		solution_ = "";

		if (rubiksCube_.getSize() == 1)
			return solution_;

		if (rubiksCube_.getSize() % 2 == 1) // has center pieces
		{
			positionTheCube();
			reduceTo3x3x3();
		}
		else
		{
			reduceTo3x3x3();
			positionTheCube();
		}

		buildCross();
		buildF2L();
		buildOLL();
		buildPLL();
		positionTheCube();

		//verify
		RubiksCubeSolverUtils::RunTimeAssert(rubiksCube_.isSolved());

		solutionSteps = solutionSteps_;
		return solution_;
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::reduceTo3x3x3()
	{
		int size = rubiksCube_.getSize();
		if (size <= 3)
			return;

		//=====FIX CENTER CUBES====

		//TODO: Check if we already have solved face centers 

		//Fix center cubes on Up Face
		fixCenterCubes_singleFace(Color::Yellow);

		//Fix center cubes on Front Face
		applyAlgorithm("X'");
		fixCenterCubes_singleFace(Color::Blue);

		//Fix center cubes on Right Face
		applyAlgorithm("Z");
		fixCenterCubes_singleFace(Color::Red);

		//Fix center cubes on Back Face
		applyAlgorithm("Z");
		fixCenterCubes_singleFace(Color::Green);

		//Fix center cubes on Left & Down Face together
		//applyAlgorithm("Z");
		applyAlgorithm("");
		fixCenterCubes_twoFaces(Color::White, Color::Orange);

		//Reset the cube
		//applyAlgorithm("XY'");
		applyAlgorithm("XY'2");
		

		//=====FIX EDGES====

		//TODO: Check if we already have any solved edges

		//Take while face up
		//applyAlgorithm("X2"); //This may help create cross
		applyAlgorithm("Z");
		fixEdgeCubes(Color::Red, Color::Blue);
		applyAlgorithm("X");
		fixEdgeCubes(Color::Green, Color::Red);
		applyAlgorithm("X");
		fixEdgeCubes(Color::Orange, Color::Green);
		applyAlgorithm("X");
		fixEdgeCubes(Color::Blue, Color::Orange);
		applyAlgorithm("XZ");

		fixEdgeCubes(Color::White, Color::Blue);
		applyAlgorithm("Y'");
		fixEdgeCubes(Color::White, Color::Orange);
		applyAlgorithm("Y'");
		fixEdgeCubes(Color::White, Color::Green);
		applyAlgorithm("Y'");
		fixEdgeCubes(Color::White, Color::Red);
		
		applyAlgorithm("X2");
		fixEdgeCubes(Color::Yellow, Color::Orange);
		applyAlgorithm("Y'");
		fixEdgeCubes(Color::Yellow, Color::Blue);

		//Bring 2 remaining broken edges on Up Face (Front and Back edges)
		//fixEdgeCubes(Color::Yellow, Color::Red);
		//fixEdgeCubes(Color::Yellow, Color::Green);
		applyAlgorithm("Y'");
		fixEdgeCubes_lastTwoEdges(Color::Yellow, Color::Red, Color::Yellow, Color::Green);
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixCenterCubes_singleFace(Color targetColor)
	{
		//Always fix center cubes on top face
		//First search Top and Front face for target cubes, then Right, Down, Left, Back (we are fixing exactly in reverse order)

		int size = rubiksCube_.getSize();

		bool noMismatch = true;
		for (int i = 2; i < size && noMismatch; ++i)
		{
			for (int j = 2; j < size && noMismatch; ++j)
			{
				Cube cube = rubiksCube_.GetCube(Face::Up, 1, Face::Left, i, Face::Back, j);
				Color c1 = cube.GetFaceColor(Face::Up);
				if(c1 != targetColor)
					noMismatch = false;
			}
		}

		if (noMismatch == true)
			return;

		//Fix center line first if it has center piece
		bool hasCenterPiece = (size % 2 == 1);
		int totalVeticalLines = size - 2;
		int totalVerticalLinesToBeFormed = 0;
		int centerLineIndex = -1;
		if (hasCenterPiece)
		{
			totalVerticalLinesToBeFormed = (totalVeticalLines + 1) / 2; //including center line
			--totalVerticalLinesToBeFormed; //Exclude center line for now
			centerLineIndex = 1 + totalVerticalLinesToBeFormed;
		}
		else
			totalVerticalLinesToBeFormed = totalVeticalLines / 2;

		int lastTargetVerticleLinesIndex = 1 + totalVerticalLinesToBeFormed; //including center line

		//Go on forming vertical lines from Left --> CenterLayer on Front Face
		for (int targetLineIndexFromLeft = lastTargetVerticleLinesIndex; targetLineIndexFromLeft > 1; --targetLineIndexFromLeft)
		{
			//Do it two times
			for (int move = 0; move < 2; ++move)
			{
				//Avoid second round for center line
				if (targetLineIndexFromLeft == centerLineIndex)
				{
					++move;
					applyAlgorithm("X"); //Form center line on Top Face directly
				}

				//Collect all cubes one by one from Up --> Down on Front Face
				for (int targetIndexFromUp = 2; targetIndexFromUp < size; ++targetIndexFromUp)
				{
					//If the target cube is already at right position, continue...
					if (rubiksCube_.GetCube(Face::Front, 1, Face::Left, targetLineIndexFromLeft, Face::Up, targetIndexFromUp).GetFaceColor(Face::Front) == targetColor)
						continue;

					//Search all faces. Every target cube can be at 4 places on each Face
					bool found = false;
					
					//if target cube is on Up or Front Face, take it to Right Face as we can not directly position at right place
					//Up --> Right
					//applyAlgorithm("X");
					//applyAlgorithm("F");
					int rowFromTopToAvoid = -1;
					if (move == 1)
						rowFromTopToAvoid = targetLineIndexFromLeft;
					found = fixCenterCubes_moveTargetCubeToRightFace(Face::Up, "U", targetLineIndexFromLeft, targetIndexFromUp, targetColor
						, rowFromTopToAvoid, centerLineIndex);
					//applyAlgorithm("F'");
					//applyAlgorithm("X'");

					if (!found)
					{
						//Front --> Right
						//applyAlgorithm("F");
						found = fixCenterCubes_moveTargetCubeToRightFace(Face::Front, "F", targetLineIndexFromLeft, targetIndexFromUp, targetColor
							, targetLineIndexFromLeft, -1);
						//applyAlgorithm("F'");
					}

					//Reset the flag as we are going to place target cube at right place on Front Face
					found = false;

					//Right --> Front - if we have targetColor cube on Right Face, take it to Front Face
					if (!found)
					{
						found = fixCenterCubes_moveTargetCubeToFrontFace(Face::Right, "", targetLineIndexFromLeft, targetIndexFromUp, targetColor);
					}

					//Down --> Front
					if (!found)
					{
						//applyAlgorithm("Z");
						//applyAlgorithm("F");
						found = fixCenterCubes_moveTargetCubeToFrontFace(Face::Down, "F", targetLineIndexFromLeft, targetIndexFromUp, targetColor);
						//applyAlgorithm("F'");
						//applyAlgorithm("Z'");
					}

					//Left --> Front
					if (!found)
					{
						//applyAlgorithm("Z2");
						//applyAlgorithm("F2");
						found = fixCenterCubes_moveTargetCubeToFrontFace(Face::Left, "F2", targetLineIndexFromLeft, targetIndexFromUp, targetColor);
						//applyAlgorithm("F2");
						//applyAlgorithm("Z'2");
					}

					//Back --> Right --> Front
					if (!found)
					{
						//applyAlgorithm("Y'");
						found = fixCenterCubes_moveTargetCubeToFrontFace(Face::Back, "", targetLineIndexFromLeft, targetIndexFromUp, targetColor);
						//applyAlgorithm("Y");
						//if (found)
						//{
						//	found = fixCenterCubes_moveTargetCubeToFrontFace(Face::Right, "", targetLineIndexFromLeft, targetIndexFromUp, targetColor);
						//	RubiksCubeSolverUtils::RunTimeAssert(found);
						//}
					}

					RubiksCubeSolverUtils::RunTimeAssert(found);
					RubiksCubeSolverUtils::RunTimeAssert(rubiksCube_.GetCube(Face::Front, 1, Face::Left, targetLineIndexFromLeft, Face::Up, targetIndexFromUp).GetFaceColor(Face::Front) == targetColor);
				}

				//Once we have all cubes in target line on Front Face at right position (i.e. Left-i, Up-j ), move it to Up Face
				if (targetLineIndexFromLeft == centerLineIndex)
				{
					applyAlgorithm("X'"); //Center line is formed on Top Face directly
				}
				else
				{
					applyAlgorithm("L" + string{ "(" + to_string(targetLineIndexFromLeft) + ")'" });
					applyAlgorithm("U2");
					applyAlgorithm("L" + string{ "(" + to_string(targetLineIndexFromLeft) + ")" });
					if (move == 0)
						applyAlgorithm("U2");
				}
			}
		}

	}

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixCenterCubes_moveTargetCubeToRightFace(Face fromFace, const string& preMove,
		int targetLineIndexFromLeft, int targetIndexFromUp, Color targetColor,
		int columnFromLeftToAvoid, int centerColumnToAvoid)
	{
		vector<Face> faces(0);
		string rotation;
		string prime1, prime2;
		switch (fromFace)
		{
		case Face::Up:
			faces = { Face::Left , Face::Back, Face::Right, Face::Front };
			rotation = "B";
			prime1 = "'";
			prime2 = "";
			break;
		case Face::Front:
			faces = { Face::Left , Face::Up, Face::Right, Face::Down };
			rotation = "U";
			prime1 = "'";
			prime2 = "";
			break;
		default:
			RubiksCubeSolverUtils::RunTimeAssert(false);
		}

		string postMove;
		if (preMove.empty())
			postMove = "";
		else if (preMove[preMove.length() - 1] == '2')
			postMove = preMove;
		else if (preMove[preMove.length() - 1] == '\'')
			postMove = string(preMove.begin(), preMove.end() - 1);
		else
			postMove = preMove + "'";

		int target = -1;
		int numCase = 0;

		int size = rubiksCube_.getSize();
		if ((target = targetLineIndexFromLeft) != -1
			&& centerColumnToAvoid != target
			&& columnFromLeftToAvoid != target
			&& rubiksCube_.GetCube(fromFace, 1, faces[0], targetLineIndexFromLeft, faces[1], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 1;
		}
		else if ((target = size - targetIndexFromUp + 1) != -1
			&& centerColumnToAvoid != target
			&& columnFromLeftToAvoid != target
			&& rubiksCube_.GetCube(fromFace, 1, faces[1], targetLineIndexFromLeft, faces[2], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 2;
		}
		else if ((target = size - targetLineIndexFromLeft + 1) != -1
			&& centerColumnToAvoid != target
			&& columnFromLeftToAvoid != target
			&& rubiksCube_.GetCube(fromFace, 1, faces[2], targetLineIndexFromLeft, faces[3], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 3;
		}
		else if ((target = targetIndexFromUp) != -1
			&& centerColumnToAvoid != target
			&& columnFromLeftToAvoid != target
			&& rubiksCube_.GetCube(fromFace, 1, faces[3], targetLineIndexFromLeft, faces[0], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 4;
		}

		if (numCase != 0)
		{
			applyAlgorithm(preMove);
			applyAlgorithm(rotation + string{ "(" + to_string(target) + ")" + prime1 });
			applyAlgorithm("R2");
			applyAlgorithm(rotation + string{ "(" + to_string(target) + ")" + prime2 });
			//applyAlgorithm("R2");
			applyAlgorithm(postMove);
			return true;
		}

		return false;
	}

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixCenterCubes_moveTargetCubeToFrontFace(Face fromFace, const string& frontFacePreMove,
		int targetLineIndexFromLeft, int targetIndexFromUp, Color targetColor)
	{
		vector<Face> faces(0);
		string rotation;
		string numRotations = "1";
		string prime1, prime2;
		string faceMove;
		switch (fromFace)
		{
		case Face::Right:
			faces = { Face::Front , Face::Up, Face::Back, Face::Down };
			rotation = "U";
			prime1 = "";
			prime2 = "'";
			faceMove = "R";
			break;
		case Face::Back:
			faces = { Face::Right , Face::Up, Face::Left, Face::Down };
			rotation = "U";
			prime1 = "";
			prime2 = "'";
			faceMove = "B";
			numRotations = "2";
			break;
		case Face::Down:
			faces = { Face::Front , Face::Right, Face::Back, Face::Left };
			rotation = "R";
			prime1 = "";
			prime2 = "'";
			faceMove = "D";
			break;
		case Face::Left:
			faces = { Face::Front , Face::Down, Face::Back, Face::Up };
			rotation = "D";
			prime1 = "";
			prime2 = "'";
			faceMove = "L";
			break;
		default:
			RubiksCubeSolverUtils::RunTimeAssert(false);
		}

		string frontFacePostMove;
		if (frontFacePreMove.empty())
			frontFacePostMove = "";
		else if (frontFacePreMove[frontFacePreMove.length() - 1] == '2')
			frontFacePostMove = frontFacePreMove;
		else if (frontFacePreMove[frontFacePreMove.length() - 1] == '\'')
			frontFacePostMove = string(frontFacePreMove.begin(), frontFacePreMove.end() - 1);
		else
			frontFacePostMove = frontFacePreMove + "'";

		int target = -1;
		int numCase = 0;

		if (rubiksCube_.GetCube(fromFace, 1, faces[0], targetLineIndexFromLeft, faces[1], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 1;
			faceMove = "";
		}
		else if (rubiksCube_.GetCube(fromFace, 1, faces[1], targetLineIndexFromLeft, faces[2], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 2;
			faceMove += "'";
		}
		else if (rubiksCube_.GetCube(fromFace, 1, faces[2], targetLineIndexFromLeft, faces[3], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 3;
			faceMove += "2";
		}
		else if (rubiksCube_.GetCube(fromFace, 1, faces[3], targetLineIndexFromLeft, faces[0], targetIndexFromUp).GetFaceColor(fromFace) == targetColor)
		{
			numCase = 4;
			//faceMove remains unchanged
		}

		if(numCase != 0)
		{
			applyAlgorithm(frontFacePreMove);

			applyAlgorithm(faceMove);

			//applyAlgorithm("U" + string{ "(" + to_string(targetIndexFromUp) + ")" });
			applyAlgorithm(rotation + string{ "(" + to_string(targetIndexFromUp) + ")" + prime1 + numRotations });

			if (targetLineIndexFromLeft == targetIndexFromUp)
				applyAlgorithm("F'");
			else
				applyAlgorithm("F");

			//applyAlgorithm("U" + string{ "(" + to_string(targetIndexFromUp) + ")'" });
			applyAlgorithm(rotation + string{ "(" + to_string(targetIndexFromUp) + ")" + prime2 + numRotations });

			if (targetLineIndexFromLeft == targetIndexFromUp)
				applyAlgorithm("F");
			else
				applyAlgorithm("F'");

			applyAlgorithm(frontFacePostMove);

			return true;
		}

		return false;
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixCenterCubes_twoFaces(Color targetColor1, Color targetColor2)
	{
		Color targetColorFrontFace = targetColor1;
		Color targetColorRightFace = targetColor2;
		int size = rubiksCube_.getSize();
		bool hasCenterPiece = (size % 2 == 1);
		int totalVeticalLines = size - 2;
		int totalVerticalLinesToBeFormed = 0;
		int centerLineIndex = -1;
		if (hasCenterPiece)
		{
			totalVerticalLinesToBeFormed = (totalVeticalLines + 1) / 2; //including center line
			--totalVerticalLinesToBeFormed; //Exclude center line for now
			centerLineIndex = 1 + totalVerticalLinesToBeFormed;
		}
		else
			totalVerticalLinesToBeFormed = totalVeticalLines / 2;

		int lastTargetVerticleLinesIndex = 1 + totalVerticalLinesToBeFormed; //including center line

		//Form vertical center line on Front Face
		//if (centerLineIndex != -1)
		//{
		//	for (int targetIndexFromUp = 2; targetIndexFromUp < size; ++targetIndexFromUp)
		//	{
		//		//If the target cube is already at right position, continue...
		//		if (rubiksCube_.GetCube(Face::Front, 1, Face::Left, centerLineIndex, Face::Up, targetIndexFromUp).GetFaceColor(Face::Front) == targetColorFrontFace)
		//			continue;

		//		fixCenterCubes_moveTargetCubeFromRightToFrontFace(centerLineIndex, targetIndexFromUp, targetColorFrontFace);
		//		RubiksCubeSolverUtils::RunTimeAssert(rubiksCube_.GetCube(Face::Front, 1, Face::Left, centerLineIndex, Face::Up, targetIndexFromUp).GetFaceColor(Face::Front) == targetColorFrontFace);
		//	}
		//}

		//For each vertical line on Front Face
		for (int targetLineIndexFromLeft = lastTargetVerticleLinesIndex; targetLineIndexFromLeft > 1; --targetLineIndexFromLeft)
		{
			//On Front Face, lines will be formed vertical
			//On Right Face, lines will be vertical first and then moved to Right Face
			for (int move = 0; move < 2; ++move)
			{
				if (move == 0 && (
					//Check Left vertical line on Right Face
					fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Right, Face::Front, targetLineIndexFromLeft, Face::Up, targetColorRightFace, "R")
					//Check Up horizontal line on Right Face
					|| (fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Right, Face::Up, targetLineIndexFromLeft, Face::Back, targetColorRightFace, ""))
					//Check Right vertical line on Right Face
					|| fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Right, Face::Back, targetLineIndexFromLeft, Face::Down, targetColorRightFace, "R'")
					//Check Down horizontal line on Right Face
					|| fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Right, Face::Down, targetLineIndexFromLeft, Face::Front, targetColorRightFace, "R2")
					)
					)
					continue;

				if (
					//Check Left vertical line on Front Face
					fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Front, Face::Left, targetLineIndexFromLeft, Face::Up, targetColorRightFace, "F")
					//Check Up horizontal line on Front Face
					|| fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Front, Face::Up, targetLineIndexFromLeft, Face::Right, targetColorRightFace, "")
					//Check Right vertical line on Front Face
					|| fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Front, Face::Right, targetLineIndexFromLeft, Face::Down, targetColorRightFace, "F'")
					//Check Down horizontal line on Front Face
					|| fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face::Front, Face::Down, targetLineIndexFromLeft, Face::Left, targetColorRightFace, "F2")
					)
				{
					// Once we have all cubes in target line on Front Face at right position(i.e.Left - i, Up - j), move it to Right Face					
					applyAlgorithm("U" + string{ "(" + to_string(targetLineIndexFromLeft) + ")'" });
					applyAlgorithm("R2");
					applyAlgorithm("U" + string{ "(" + to_string(targetLineIndexFromLeft) + ")" });
					if (move == 0)
						applyAlgorithm("R2");
					continue;
				}

				//Check all orientations
				bool found = false;
				for (int orientation = 0; orientation < 4; ++orientation)
				{
					//Check the vertical line from Left
					for (int targetIndexFromUp = 2; targetIndexFromUp < size; ++targetIndexFromUp)
					{
						//If the target cube is already at right position, continue...
						if (rubiksCube_.GetCube(Face::Front, 1, Face::Left, targetLineIndexFromLeft, Face::Up, targetIndexFromUp).GetFaceColor(Face::Front) == targetColorRightFace)
							continue;

						int rowFromTopToAvoid = -1;
						if (move == 1)
							rowFromTopToAvoid = targetLineIndexFromLeft;
						found = fixCenterCubes_bothFrontAndUpFaces(targetLineIndexFromLeft, targetIndexFromUp, targetColorRightFace, rowFromTopToAvoid, centerLineIndex);
						if (!found)
							break;
					}
					if (found)
					{
						//Make line horizontal
						applyAlgorithm("F");
						break;
					}

					//Check the vertical line from Right
					for (int targetIndexFromUp = 2; targetIndexFromUp < size; ++targetIndexFromUp)
					{
						//If the target cube is already at right position, continue...
						if (rubiksCube_.GetCube(Face::Front, 1, Face::Right, targetLineIndexFromLeft, Face::Up, targetIndexFromUp).GetFaceColor(Face::Front) == targetColorRightFace)
							continue;

						int rowFromTopToAvoid = -1;
						if (move == 1)
							rowFromTopToAvoid = targetLineIndexFromLeft;
						found = fixCenterCubes_bothFrontAndUpFaces((size - targetLineIndexFromLeft + 1), targetIndexFromUp, targetColorRightFace, rowFromTopToAvoid, centerLineIndex);
						if (!found)
							break;
					}
					if (found)
					{
						//Make line horizontal
						applyAlgorithm("F'");
						break;
					}

					//Try next orientation
					applyAlgorithm("F");
				}

				RubiksCubeSolverUtils::RunTimeAssert(found);

				// Once we have all cubes in target line on Front Face at right position(i.e.Left - i, Up - j), move it to Right Face					
				applyAlgorithm("U" + string{ "(" + to_string(targetLineIndexFromLeft) + ")'" });
				applyAlgorithm("R2");
				applyAlgorithm("U" + string{ "(" + to_string(targetLineIndexFromLeft) + ")" });
				if (move == 0)
					applyAlgorithm("R2");
				//applyAlgorithm("F'");
			}
		}
	}

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixCenterCubes_bothFrontAndUpFaces_CheckIfLineExist(Face faceFront, Face faceLeft, int targetLineIndexFromLeft, Face faceUp, Color targetColorRightFace, const string& algo)
	{
		int size = rubiksCube_.getSize();
		bool lineAlreadyPresent = true;
		for (int targetIndexFromUp = 2; targetIndexFromUp < size; ++targetIndexFromUp)
		{
			if (rubiksCube_.GetCube(faceFront, 1, faceLeft, targetLineIndexFromLeft, faceUp, targetIndexFromUp).GetFaceColor(faceFront) != targetColorRightFace)
			{
				lineAlreadyPresent = false;
				break;
			}
		}
		if (lineAlreadyPresent)
			applyAlgorithm(algo);

		return lineAlreadyPresent;
	}

	//This version does not change any structure on Right Face, but rearranges Front Face
	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixCenterCubes_bothFrontAndUpFaces(int targetLineIndexFromLeft, int targetIndexFromUp, Color targetColor,
		int rowFromTopToAvoid, int centerRowToAvoid)
	{
		int size = rubiksCube_.getSize();
		bool retVal = false;

		if (targetIndexFromUp != centerRowToAvoid
			&& targetIndexFromUp != rowFromTopToAvoid
			&& rubiksCube_.GetCube(Face::Right, 1, Face::Front, targetLineIndexFromLeft, Face::Up, targetIndexFromUp).GetFaceColor(Face::Right) == targetColor)
		{
			applyAlgorithm("U" + string{ "(" + to_string(targetIndexFromUp) + ")" });

			if (targetLineIndexFromLeft == targetIndexFromUp)
				applyAlgorithm("F'");
			else
				applyAlgorithm("F");

			applyAlgorithm("U" + string{ "(" + to_string(targetIndexFromUp) + ")'" });

			if (targetLineIndexFromLeft == targetIndexFromUp)
				applyAlgorithm("F");
			else
				applyAlgorithm("F'");

			retVal = true;
		}
		//else if (targetLineIndexFromLeft != centerRowToAvoid
		//	&& targetLineIndexFromLeft != rowFromTopToAvoid
		//	&& rubiksCube_.GetCube(Face::Right, 1, Face::Up, targetLineIndexFromLeft, Face::Back, targetIndexFromUp).GetFaceColor(Face::Right) == targetColor)
		//{
		//	applyAlgorithm("F");

		//	applyAlgorithm("U" + string{ "(" + to_string(targetLineIndexFromLeft) + ")" });

		//	if (targetLineIndexFromLeft == targetIndexFromUp)
		//		applyAlgorithm("F'");
		//	else
		//		applyAlgorithm("F");

		//	applyAlgorithm("U" + string{ "(" + to_string(targetLineIndexFromLeft) + ")'" });

		//	if (targetLineIndexFromLeft == targetIndexFromUp)
		//		applyAlgorithm("F");
		//	else
		//		applyAlgorithm("F'");

		//	applyAlgorithm("F'");
		//	retVal = true;

		//}
		//else if (targetIndexFromUp != centerRowToAvoid
		//	&& (size - targetIndexFromUp + 1) != rowFromTopToAvoid
		//	&& rubiksCube_.GetCube(Face::Right, 1, Face::Back, targetLineIndexFromLeft, Face::Down, targetIndexFromUp).GetFaceColor(Face::Right) == targetColor)
		//{
		//	applyAlgorithm("F2");

		//	applyAlgorithm("U" + string{ "(" + to_string((size - targetIndexFromUp + 1)) + ")" });

		//	if (targetLineIndexFromLeft == targetIndexFromUp)
		//		applyAlgorithm("F'");
		//	else
		//		applyAlgorithm("F");

		//	applyAlgorithm("U" + string{ "(" + to_string((size - targetIndexFromUp + 1)) + ")'" });

		//	if (targetLineIndexFromLeft == targetIndexFromUp)
		//		applyAlgorithm("F");
		//	else
		//		applyAlgorithm("F'");

		//	applyAlgorithm("F2");
		//	retVal = true;
		//}
		//else if (targetLineIndexFromLeft != centerRowToAvoid
		//	&& (size - targetLineIndexFromLeft + 1) != rowFromTopToAvoid
		//	&& rubiksCube_.GetCube(Face::Right, 1, Face::Down, targetLineIndexFromLeft, Face::Front, targetIndexFromUp).GetFaceColor(Face::Right) == targetColor)
		//{
		//	applyAlgorithm("F'");

		//	applyAlgorithm("U" + string{ "(" + to_string((size - targetLineIndexFromLeft + 1)) + ")" });

		//	if (targetLineIndexFromLeft == targetIndexFromUp)
		//		applyAlgorithm("F'");
		//	else
		//		applyAlgorithm("F");

		//	applyAlgorithm("U" + string{ "(" + to_string((size - targetLineIndexFromLeft + 1)) + ")'" });

		//	if (targetLineIndexFromLeft == targetIndexFromUp)
		//		applyAlgorithm("F");
		//	else
		//		applyAlgorithm("F'");

		//	applyAlgorithm("F");
		//	retVal = true;
		//}

		return retVal;
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixEdgeCubes(Color targetColorUp, Color targetColorFront)
	{
		//Always fix front and back edges on top face
		int size = rubiksCube_.getSize();
		int numEdgeCubes = size - 2;
		//Collect all edge cubes one by one on Front-Up edge from Left to Right
		//Color targetColorFront = color1;
		//Color targetColorUp = color2;
		
		//Ensure that the Up-Front edge is broken/unsolved edge
		int i = 0;
		vector<string> algoToCheckAllUpFrontEdges{
			"", "U", "U", "U",		// Up face
			"F", "F", "F",			// Front face
			"BF2", "BF2", "BF2",	// Bottom face
			"R'U",					// Right face
			"LU'"					// Left face
		};
		for (; i < algoToCheckAllUpFrontEdges.size(); ++i)
		{
			applyAlgorithm(algoToCheckAllUpFrontEdges[i]);
			Cube& cube = rubiksCube_.GetCube(Face::Up, 1, Face::Front, 1, Face::Left, 2);
			Color up = cube.GetFaceColor(Face::Up);
			Color front = cube.GetFaceColor(Face::Front);
			unsigned int targetColorPair = ((1u << up) | (1u << front));
			if (solvedEdges_.find(targetColorPair) == solvedEdges_.end())
				break;
		}
		RubiksCubeSolverUtils::RunTimeAssert(i != algoToCheckAllUpFrontEdges.size());

		Cube cube;
		//int index1 = fixEdgeCubes_bringToUpBackEdge(-1, targetColorFront, targetColorUp);
		//Find similar edge and take it to Up-Back edge
		for (int edgeCubeIndexFromLeft = 2; edgeCubeIndexFromLeft < size; ++edgeCubeIndexFromLeft)
		{
			//check if its found at right position
			if (fixEdgeCubes_bringToUpBackEdge_searchEdge(edgeCubeIndexFromLeft, targetColorUp, targetColorFront, Face::Up, Face::Front, Face::Left, "", true))
				continue;

			//bring the cube at Up-Back edge in opposite orientation
			Color expectedUp = targetColorFront;
			Color expectedBack = targetColorUp;

			int targetIndexRight = size - edgeCubeIndexFromLeft + 1;
			if (edgeCubeIndexFromLeft < targetIndexRight)
			{
				//TODO: remove it from Up-Front edge
				bool found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Up, Face::Front, Face::Left, "", true);
				//if (found) casesCovered[16] = true;
				if (found)
				{
					//Ensure Up-Right edge is broken
					fixEdgeCubes_ensureUpRightEdgeUnsolved();

					//Ensure that Up-Back edge is broken/unsolved
					Cube& cube = rubiksCube_.GetCube(Face::Up, 1, Face::Back, 1, Face::Left, 2);
					Color up = cube.GetFaceColor(Face::Up);
					Color back = cube.GetFaceColor(Face::Back);
					unsigned int targetColorPair = ((1u << up) | (1u << back));
					if (solvedEdges_.find(targetColorPair) != solvedEdges_.end())
					{
						//Take Up-Right edge to Up-Back
						applyAlgorithm("RB");
						//Ensure Up-Right edge is broken
						fixEdgeCubes_ensureUpRightEdgeUnsolved();
					}

					//Remove this cube from Up-Front edge
					applyAlgorithm("L(" + to_string(targetIndexRight) + ")");
					applyAlgorithm("F");
					applyAlgorithm("R'");
					applyAlgorithm("F'");
					applyAlgorithm("L(" + to_string(targetIndexRight) + ")'");
					applyAlgorithm("R");
					applyAlgorithm("F'");
				}
			}
			
			bool found = fixEdgeCubes_bringToUpBackEdge(edgeCubeIndexFromLeft, expectedUp, expectedBack);
			//RubiksCubeSolverUtils::RunTimeAssert(found);
			//Cubes can never be in same orientation when we come here. Above function ensures its in correct orientation.
			//if (index1 == index2)
			//{
				//Move cubes into other index, preserve orientation.
				//This case may not be called
			//}

			//Ensure that the Up-Right edge is broken/unsolved edge. Do not touch Up-Front and Up-Back edges.
			fixEdgeCubes_ensureUpRightEdgeUnsolved();
			
			//Apply algorithm to join edge cubes
			//Ensure that the new edge is at front again
			applyAlgorithm("L(" + to_string(edgeCubeIndexFromLeft) + ")");
			applyAlgorithm("F");
			applyAlgorithm("R'");
			applyAlgorithm("F'");
			applyAlgorithm("L(" + to_string(edgeCubeIndexFromLeft) + ")'");
			applyAlgorithm("R");
			applyAlgorithm("F'");
		}

		solvedEdges_.insert((1u << targetColorUp) | (1u << targetColorFront));

		//Once done, move this edge to left
		//applyAlgorithm("F'");
	}

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixEdgeCubes_ensureUpRightEdgeUnsolved()
	{
		int j = 0;
		vector<string> algoToCheckAllUpRightEdges{
			"", "R", "R", "R",			//Right face
			"DR2", "DR2", "DR2",		//Down face
			"LD2R2", "LD2R2", "LD2R2"	//Left face
		};
		for (; j < algoToCheckAllUpRightEdges.size(); ++j)
		{
			applyAlgorithm(algoToCheckAllUpRightEdges[j]);
			Cube& cube = rubiksCube_.GetCube(Face::Up, 1, Face::Right, 1, Face::Front, 2);
			Color up = cube.GetFaceColor(Face::Up);
			Color right = cube.GetFaceColor(Face::Right);
			unsigned int targetColorPair = ((1u << up) | (1u << right));
			if (solvedEdges_.find(targetColorPair) == solvedEdges_.end())
				break;
		}
		RubiksCubeSolverUtils::RunTimeAssert(j != algoToCheckAllUpRightEdges.size());
		return true;
	}

	//This function searches in all edges (searches Front-Up edge only when index = -1)
	//This function ensures that the cube is in expected orientation while taking to Up-Back edge
	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixEdgeCubes_bringToUpBackEdge(int targetIndexLeft, Color expectedUp, Color expectedBack)
	{
		int size = rubiksCube_.getSize();
		int targetBits = (1 << expectedBack) | (1 << expectedUp);
		int targetIndexRight = size - targetIndexLeft + 1;

		string algo1{ "L'B'" };
		string algo2{ "FUF'" };
		string algo3{ "F'U'F" };
		string algo4{ "FU'F'" };
		string algo{ "" };

		bool found = false;
		static vector<bool> casesCovered(24, false);
		
		//Move from Down edges to vertical edge
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Down, Face::Front, Face::Left, "D2B", true);
			if(found) casesCovered[0] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Down, Face::Front, Face::Left, "D2B", false);
			if (found) casesCovered[1] = true;
		}

		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Down, Face::Back, Face::Right, "B", true);
			if (found) casesCovered[2] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Down, Face::Back, Face::Right, "B" /*+ algo*/, false);
			if (found) casesCovered[3] = true;
		}

		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Down, Face::Left, Face::Front, "L" /*+ algo1*/, false);
			if (found) casesCovered[4] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Down, Face::Left, Face::Front, "L" /*+ algo2*/, true);
			if (found) casesCovered[5] = true;
		}

		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Down, Face::Right, Face::Back, "R'" /*+ algo3*/, false);
			if (found) casesCovered[6] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Down, Face::Right, Face::Back, "R'" /*+ algo4*/, true);
			if (found) casesCovered[7] = true;
		}

		found = false;
			
		//Move from vertical edges to Up edge
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Front, Face::Left, Face::Down, "L'" /*+ algo1*/, true);
			if (found) casesCovered[8] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Front, Face::Left, Face::Down, "L'" /*+ algo2*/, false);
			if (found) casesCovered[9] = true;
		}

		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Front, Face::Right, Face::Up, "R" /*+ algo3*/, true);
			if (found) casesCovered[10] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Front, Face::Right, Face::Up, "R" /*+ algo4*/, false);
			if (found) casesCovered[11] = true;
		}

		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Back, Face::Left, Face::Down, "B'", false);
			if (found) casesCovered[12] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Back, Face::Left, Face::Down, "B'" /*+ algo*/, true);
			if (found) casesCovered[13] = true;
		}

		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Back, Face::Right, Face::Up, "B", false);
			if (found) casesCovered[14] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Back, Face::Right, Face::Up, "B" /*+ algo*/, true);
			if (found) casesCovered[15] = true;
		}

		found = false;

		//Move from Up edges to Up-Back edge
		//Up-Front edge
		//if (!found && targetIndexLeft < targetIndexRight)
		//{
		//	//TODO: remove it from Up-Front edge
		//	found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, targetColorUp, targetColorFront, Face::Up, Face::Front, Face::Left, string("") + "B'UR'U'", true);
		//	if (found) casesCovered[16] = true;
		//}
		//Up-Back edge
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Up, Face::Back, Face::Left, "", true);
			if (found) casesCovered[17] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Up, Face::Back, Face::Left, "B'UR'U'", false);
			if (found) casesCovered[18] = true;
		}
		//Up-Left edge
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Up, Face::Left, Face::Front, "F'UF", true);
			if (found) casesCovered[19] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Up, Face::Left, Face::Front, string("F'UF") + "B'UR'U'", false);
			if (found) casesCovered[20] = true;
		}
		//Up-Right edge
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexLeft, expectedUp, expectedBack, Face::Up, Face::Right, Face::Back, "F'U'F", true);
			if (found) casesCovered[21] = true;
		}
		if (!found)
		{
			found = fixEdgeCubes_bringToUpBackEdge_searchEdge(targetIndexRight, expectedUp, expectedBack, Face::Up, Face::Right, Face::Back, "RB", false);
			if (found) casesCovered[22] = true;
		}

		RubiksCubeSolverUtils::RunTimeAssert(found); //We must find edge before reaching here
		return found;
	}

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixEdgeCubes_bringToUpBackEdge_searchEdge(int targetIndex, Color targetColorUp, Color targetColorFront,
		Face face1, Face face2, Face face3, const string& algo, bool sameOrientation)
	{
		int size = rubiksCube_.getSize();
		int targetBits = (1 << targetColorFront) | (1 << targetColorUp);
		//for (int i = 2; i < size; ++i)
		//{
			Cube& cube = rubiksCube_.GetCube(face1, 1, face2, 1, face3, targetIndex);
			Color up = cube.GetFaceColor(face1);
			Color front = cube.GetFaceColor(face2);
			if ((sameOrientation && targetColorUp == up && targetColorFront == front) ||
				(!sameOrientation && targetColorUp == front && targetColorFront == up))
			{
				applyAlgorithm(algo);
				return true;
			}

			//int bits = ((1 << front) | (1 << up));
			//if (bits == targetBits)
			//{
			//	if (targetColorUp == up)
			//	{
			//		applyAlgorithm(correctionAlgo);
			//		//retIndex = size - retIndex + 1;
			//	}
			//	else
			//	{
			//		applyAlgorithm(algo);
			//		//retIndex = i; // return the index from left
			//	}

			//	return true;
			//}
		//

		return false;
	}

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::fixEdgeCubes_lastTwoEdges(Color targetColorUp1, Color targetColorFront1, Color targetColorUp2, Color targetColorBack2)
	{
		//Ensure Up-Front edge is unsolved
		fixEdgeCubes_ensureUpRightEdgeUnsolved();
		applyAlgorithm("U");

		//Ensure Up-Back edge is unsolved
		fixEdgeCubes_ensureUpRightEdgeUnsolved();
		applyAlgorithm("RB");

		return true;
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::applyAlgorithm(const string& step)
	{
		solution_ += step;
		solutionSteps_ += rubiksCube_.applyAlgorithm(step, animate_, ui_);
	}

	//bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::isEdgeCube(const Cube& currentCube, const Color& first, const Color& second)
	//{
	//	int firstCount = 0;
	//	int secondCount = 0;
	//	for (int i = 0; i < 6; ++i)
	//	{
	//		if (currentCube.GetFaceColor(Face(i)) == first)
	//			++firstCount;
	//		else if (currentCube.GetFaceColor(Face(i)) == second)
	//			++secondCount;
	//		else if (currentCube.GetFaceColor(Face(i)) != Color::Black)
	//			return false;
	//	}

	//	return firstCount == 1 && secondCount == 1;
	//}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildCross_PlaceEdgePiece(const Color& targetColorFront, const Color& targetColorBottom)
	{
		//Cube* currentCube = nullptr;

		// Bring it from bottom layer (y = 0) to top layer
		//Cube currentCube = rubiksCube_.GetCube(1, 0, 2);
		Cube currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 1, Face::Front, 1);
		Color c1 = currentCube.GetFaceColor(Face::Front);
		Color c2 = currentCube.GetFaceColor(Face::Down);

		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			//Do nothing
		}
		if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("F2");
		}
		//currentCube = rubiksCube_.GetCube(2, 0, 1);
		currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 1, Face::Front, 2);
		c1 = currentCube.GetFaceColor(Face::Right);
		c2 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("R2");
		}
		//currentCube = rubiksCube_.GetCube(1, 0, 0);
		currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 1, Face::Back, 1);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("B2");
		}
		//currentCube = rubiksCube_.GetCube(0, 0, 1);
		currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 1, Face::Front, 2);
		c1 = currentCube.GetFaceColor(Face::Left);
		c2 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("L2");
			//RubiksCubeAlgoExecuter::executeAlgorithm("L'F'");
		}

		// Bring it from middle later (y = 1) to top layer
		//currentCube = rubiksCube_.GetCube(0, 1, 0);
		currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 2, Face::Back, 1);
		c1 = currentCube.GetFaceColor(Face::Left);
		c2 = currentCube.GetFaceColor(Face::Back);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("LU'L'");
		}
		//currentCube = rubiksCube_.GetCube(0, 1, 2);
		currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 2, Face::Front, 1);
		c1 = currentCube.GetFaceColor(Face::Left);
		c2 = currentCube.GetFaceColor(Face::Front);
		if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("F'");
		}
		else if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("F");
		}
		//currentCube = rubiksCube_.GetCube(2, 1, 2);
		currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Front, 1);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Right);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("F");
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("F'");
		}
		//currentCube = rubiksCube_.GetCube(2, 1, 0);
		currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Back, 1);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Right);
		if ((c1 == targetColorFront && c2 == targetColorBottom) || (c1 == targetColorBottom && c2 == targetColorFront))
		{
			applyAlgorithm("R'UR");
		}

		// Bring it from top later (y = 2) to bottom layer at appropriate position
		//currentCube = rubiksCube_.GetCube(1, 2, 0);
		currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 1);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Back);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("B'R'URBF2");
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("U2F2");
		}

		//currentCube = rubiksCube_.GetCube(0, 2, 1);
		currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("LF'L'");
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("U'F2");
		}

		//currentCube = rubiksCube_.GetCube(1, 2, 2);
		currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Front, 1);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Front);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("FRUR'F2");
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("F2");
		}
		//currentCube = rubiksCube_.GetCube(2, 2, 1);
		currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Right);
		if (c1 == targetColorFront && c2 == targetColorBottom)
		{
			applyAlgorithm("R'FR");
		}
		else if (c1 == targetColorBottom && c2 == targetColorFront)
		{
			applyAlgorithm("UF2");
		}
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::positionTheCube()
	{
		if (rubiksCube_.getSize() <= 2)
			return;

		/*
		Make sure Cube is positioned in right way

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

		// At the most 3 out of below 3 if clauses are executed
		Cube currentCube;
		Color c;

		// Check front face has blue center cube
		//currentCube = rubiksCube_.GetCube(1, 1, 2);
		currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 2, Face::Front, 1);
		c = currentCube.GetFaceColor(Face::Front);
		if (c != Color::Blue)
		{
			if (c == Color::Green)
				applyAlgorithm("Y2");
			else if (c == Color::Orange)
				applyAlgorithm("Y'");
			else if (c == Color::Red)
				applyAlgorithm("Y");
			else if (c == Color::White)
				applyAlgorithm("X");
			else if (c == Color::Yellow)
				applyAlgorithm("X'");
		}

		//Check right face
		// Do not disturb front face, so rotate around only z axis
		//currentCube = rubiksCube_.GetCube(2, 1, 1);
		currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Back, 2);
		c = currentCube.GetFaceColor(Face::Right);
		if (c != Color::Red)
		{
			if (c == Color::Orange)
				applyAlgorithm("Z2");
			else if (c == Color::Green)
				applyAlgorithm("Y");
			else if (c == Color::Blue)
				applyAlgorithm("Y'");
			else if (c == Color::White)
				applyAlgorithm("Z'");
			else if (c == Color::Yellow)
				applyAlgorithm("Z");
		}

		//Check top face
		//currentCube = rubiksCube_.GetCube(1, 2, 1);
		currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 2);
		c = currentCube.GetFaceColor(Face::Up);
		if (c != Color::Yellow)
		{
			if (c == Color::White)
				applyAlgorithm("X2");
			else if (c == Color::Green)
				applyAlgorithm("X'");
			else if (c == Color::Blue)
				applyAlgorithm("X");
			else if (c == Color::Orange)
				applyAlgorithm("Z");
			else if (c == Color::Red)
				applyAlgorithm("Z'");
		}
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildCross()
	{
		if (rubiksCube_.getSize() <= 2)
			return;

		// Place blue-white at right position
		buildCross_PlaceEdgePiece(Color::Blue, Color::White);

		// Place red at right position
		applyAlgorithm("Y'");
		buildCross_PlaceEdgePiece(Color::Red, Color::White);

		// Place green at right position
		applyAlgorithm("Y'");
		buildCross_PlaceEdgePiece(Color::Green, Color::White);

		// Place orange at right position
		applyAlgorithm("Y'");
		buildCross_PlaceEdgePiece(Color::Orange, Color::White);

		applyAlgorithm("Y'");
	}

	/*
	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildF2L_PositionCornerPieces(const Color& targetColorFront, const Color& targetColorRight, const Color& targetColorBottom)
	{
		Cube currentCube;
		Color c1, c2, c3;

		// Check bottom layer and bring target to top layer at (2, 2, 2)
		currentCube = rubiksCube_.GetCube(0, 0, 0);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("LU2L'");


		currentCube = rubiksCube_.GetCube(0, 0, 2);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("FU'F'U'");

		currentCube = rubiksCube_.GetCube(2, 0, 2);
		c1 = currentCube.GetFaceColor(Face::Front);
		c2 = currentCube.GetFaceColor(Face::Right);
		c3 = currentCube.GetFaceColor(Face::Down);
		if (c1 == targetColorFront || c2 == targetColorRight || c3 == targetColorBottom)
		{
			//do nothing
		}
		else if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("F'UF");


		currentCube = rubiksCube_.GetCube(2, 0, 0);
		c1 = currentCube.GetFaceColor(Face::Back);
		c2 = currentCube.GetFaceColor(Face::Right);
		c3 = currentCube.GetFaceColor(Face::Down);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("R'URU");

		// Check top layer and bring target to (2, 2, 2)
		currentCube = rubiksCube_.GetCube(0, 2, 0);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Back);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("U2");

		currentCube = rubiksCube_.GetCube(0, 2, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Left);
		c3 = currentCube.GetFaceColor(Face::Front);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("U'");

		currentCube = rubiksCube_.GetCube(2, 2, 0);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Back);
		c3 = currentCube.GetFaceColor(Face::Right);
		if ((c1 == targetColorFront || c2 == targetColorFront || c3 == targetColorFront)
			&& (c1 == targetColorRight || c2 == targetColorRight || c3 == targetColorRight)
			&& (c1 == targetColorBottom || c2 == targetColorBottom || c3 == targetColorBottom)
			)
			applyAlgorithm("U");

		// Target is now in top layer at (2, 2, 2)
		currentCube = rubiksCube_.GetCube(2, 2, 2);
		c1 = currentCube.GetFaceColor(Face::Up);
		c2 = currentCube.GetFaceColor(Face::Front);
		c3 = currentCube.GetFaceColor(Face::Right);

		if (c1 == targetColorFront && c2 == targetColorBottom && c3 == targetColorRight)
			applyAlgorithm("F'U'F");
		else if (c1 == targetColorRight && c2 == targetColorFront && c3 == targetColorBottom)
			applyAlgorithm("RUR'");
		else if (c1 == targetColorBottom && c2 == targetColorRight && c3 == targetColorFront)
			applyAlgorithm("RUUR'U'RUR'");
		else
		{
			//RubiksCubeSolverUtils::RunTimeAssert
			int i = 0;
			++i;
		}
	}
	*/

	bool RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildF2L_PositionEdgeColumns(const Color& targetColorFront, const Color& targetColorRight)
	{
		//Cube currentCube;
		Color c1, c2, c3, c4, c5, c6, c7;
		bool retVal = true;
		string algo1("URU'R'U'F'UF");
		string algo2("U'F'UFURU'R'");

		int targetCornerPieceVal = (1 << targetColorFront) | (1 << targetColorRight) | (1 << Color::White);
		int targetEdgePieceVal = (1 << targetColorFront) | (1 << targetColorRight);

		//Check if already at position
		const Cube& cornerCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 1, Face::Front, 1);
		c1 = cornerCube.GetFaceColor(Face::Front);
		c2 = cornerCube.GetFaceColor(Face::Right);
		c3 = cornerCube.GetFaceColor(Face::Down);
		c4 = targetColorFront;
		c5 = targetColorRight;

		if (rubiksCube_.getSize() >= 3)
		{
			const Cube& edgeCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Front, 1);
			c4 = edgeCube.GetFaceColor(Face::Front);
			c5 = edgeCube.GetFaceColor(Face::Right);
		}
		
		if (c1 == targetColorFront && c2 == targetColorRight && c3 == Color::White
			&& c4 == targetColorFront && c5 == targetColorRight)
			return true;

		int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
		int edgePieceVal = (1 << c4) | (1 << c5);

		//Move the corner piece and edge piece to top layer
		//if(cornerPieceVal == targetCornerPieceVal || edgePieceVal == targetEdgePieceVal)
		//	applyAlgorithm("RU'R'");

		//Move the corner piece at Front-Right-Top corner
		bool done = false;
		if (done == false)
		{
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("RU'R'");
			}
		}
		if(done == false)
		{
			const Cube& cornerCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 1);
			c1 = cornerCube.GetFaceColor(Face::Up);
			c2 = cornerCube.GetFaceColor(Face::Left);
			c3 = cornerCube.GetFaceColor(Face::Back);
			int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("U'2");
			}
		}
		if (done == false)
		{
			const Cube& cornerCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Front, 1);
			c1 = cornerCube.GetFaceColor(Face::Up);
			c2 = cornerCube.GetFaceColor(Face::Left);
			c3 = cornerCube.GetFaceColor(Face::Front);
			int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("U'");
			}
		}
		if (done == false)
		{
			const Cube& cornerCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Back, 1);
			c1 = cornerCube.GetFaceColor(Face::Up);
			c2 = cornerCube.GetFaceColor(Face::Right);
			c3 = cornerCube.GetFaceColor(Face::Back);
			int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("U");
			}
		}

		if (done == false) //corner piece is stuck at Down layer
		{
			const Cube& cornerCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 1, Face::Back, 1);
			c1 = cornerCube.GetFaceColor(Face::Down);
			c2 = cornerCube.GetFaceColor(Face::Right);
			c3 = cornerCube.GetFaceColor(Face::Back);
			int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("R'U'RU'2");
			}
		}
		if (done == false) //corner piece is stuck at Down layer
		{
			const Cube& cornerCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 1, Face::Back, 1);
			c1 = cornerCube.GetFaceColor(Face::Down);
			c2 = cornerCube.GetFaceColor(Face::Left);
			c3 = cornerCube.GetFaceColor(Face::Back);
			int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("B'U'2B");
			}
		}
		if (done == false) //corner piece is stuck at Down layer
		{
			const Cube& cornerCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 1, Face::Front, 1);
			c1 = cornerCube.GetFaceColor(Face::Down);
			c2 = cornerCube.GetFaceColor(Face::Left);
			c3 = cornerCube.GetFaceColor(Face::Front);
			int cornerPieceVal = (1 << c1) | (1 << c2) | (1 << c3);
			if (cornerPieceVal == targetCornerPieceVal)
			{
				done = true;
				applyAlgorithm("FUF'U'2");
			}
		}

		//Move edge piece to back layer
		if (rubiksCube_.getSize() >= 3)
		{
			//const Cube& cornerCube = rubiksCube_.GetCube(Face::Front, Face::Right, 1, Face::Up, 1);
			//c1 = cornerCube.GetFaceColor(Face::Front);
			//c2 = cornerCube.GetFaceColor(Face::Right);

			done = false;
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 2);
				c4 = edgeCube.GetFaceColor(Face::Up);
				c5 = edgeCube.GetFaceColor(Face::Right);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("U'RU'R'U");
				}
			}
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Front, 2);
				c4 = edgeCube.GetFaceColor(Face::Up);
				c5 = edgeCube.GetFaceColor(Face::Left);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("U'RUR'U");
				}
			}
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Front, 1, Face::Up, 1, Face::Right, 2);
				c4 = edgeCube.GetFaceColor(Face::Up);
				c5 = edgeCube.GetFaceColor(Face::Front);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("UF'U2FU'");
				}
			}

			//Edge piece is stuck in second layer
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 2, Face::Front, 1);
				c4 = edgeCube.GetFaceColor(Face::Right);
				c5 = edgeCube.GetFaceColor(Face::Front);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("URUR'U'2");
				}
			}
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 2, Face::Back, 1);
				c4 = edgeCube.GetFaceColor(Face::Right);
				c5 = edgeCube.GetFaceColor(Face::Back);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("BUB'U'");
				}
			}
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 2, Face::Back, 1);
				c4 = edgeCube.GetFaceColor(Face::Left);
				c5 = edgeCube.GetFaceColor(Face::Back);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("B'UBU'");
				}
			}
			if (done == false)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 2, Face::Front, 1);
				c4 = edgeCube.GetFaceColor(Face::Left);
				c5 = edgeCube.GetFaceColor(Face::Front);
				int edgePieceVal = (1 << c4) | (1 << c5);
				if (edgePieceVal == targetEdgePieceVal)
				{
					done = true;
					applyAlgorithm("U'L'UL");
				}
			}
		}

		//Place corner piece in aprropriate orientation, then Put everyting else in place
		{
			Color edgeCubeUp = Color::Black;
			Color edgeCubeBack = Color::Black;

			if (rubiksCube_.getSize() >= 3)
			{
				const Cube& edgeCube = rubiksCube_.GetCube(Face::Right, 2, Face::Up, 1, Face::Back, 1);
				edgeCubeUp = edgeCube.GetFaceColor(Face::Up);
				edgeCubeBack = edgeCube.GetFaceColor(Face::Back);
			}

			const Cube& cornerCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 1);
			Color cornerCubeFront = cornerCube.GetFaceColor(Face::Front);
			Color cornerCubeRight = cornerCube.GetFaceColor(Face::Right);
			Color cornerCubeUp = cornerCube.GetFaceColor(Face::Up);
			if (cornerCubeUp == Color::White)
			{
				if (rubiksCube_.getSize() < 3)
				{
					if(cornerCubeFront == targetColorRight && cornerCubeRight == targetColorFront)
						applyAlgorithm("F'UFRU'2R'");
					else
						RubiksCubeSolverUtils::RunTimeAssert(false, "Imposible cube");

				}
				else if (cornerCubeRight == edgeCubeUp)
				{
					//Take edge piece to left face first
					applyAlgorithm("U'RU'R'U");
					//Orient corner cube
					applyAlgorithm("RU'R'U'2");
					//Put everything in place
					applyAlgorithm("RUR'");
				}
				else if (cornerCubeFront == edgeCubeUp)
				{
					applyAlgorithm("F'UFU'2");
					//Put everything in place
					applyAlgorithm("F'U'F");
				}
			}
			else if (cornerCubeRight == Color::White)
			{
				if (rubiksCube_.getSize() < 3)
				{
					if(cornerCubeFront == targetColorFront && cornerCubeUp == targetColorRight)
						applyAlgorithm("RUR'");
					else
						RubiksCubeSolverUtils::RunTimeAssert(false, "Imposible cube");
				}
				else if (cornerCubeFront == edgeCubeUp)
				{
					//Put everything in place
					applyAlgorithm("RUR'");
				}
				else if (cornerCubeUp == edgeCubeUp) //top colors are same
				{
					//Take edge piece to left face first
					//applyAlgorithm("U'RU'R'U");
					//make top colors different
					applyAlgorithm("RU'R'U2");
					//Corner and edge pieces are together, separate them
					applyAlgorithm("U'RU'2R'U");
					//Put everything in place
					applyAlgorithm("F'U'F");
				}
			}
			else if (cornerCubeFront == Color::White)
			{
				if (rubiksCube_.getSize() < 3)
				{
					if (cornerCubeRight == targetColorRight && cornerCubeUp == targetColorFront)
						applyAlgorithm("URU'R'");
					else
						RubiksCubeSolverUtils::RunTimeAssert(false, "Imposible cube");
				}
				else if (cornerCubeRight == edgeCubeUp)
				{
					//Take edge piece to left face first
					applyAlgorithm("U'RU'R'U");
					//Put everything in place
					applyAlgorithm("F'U'F");
				}
				else if (cornerCubeUp == edgeCubeUp) //top colors are same
				{
					//make top colors different
					applyAlgorithm("F'UFU'2");
					//edge cube goes to left, take it at back
					applyAlgorithm("U'RUR'U");
					//Put everything in place
					applyAlgorithm("RUR'");
				}
			}
			else
				RubiksCubeSolverUtils::RunTimeAssert(false, "Unhandled situation");
		}

		return true;
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildF2L()
	{
		//position corner pieces
		buildF2L_PositionEdgeColumns(Color::Blue, Color::Red);

		applyAlgorithm("Y'");
		buildF2L_PositionEdgeColumns(Color::Red, Color::Green);

		applyAlgorithm("Y'");
		buildF2L_PositionEdgeColumns(Color::Green, Color::Orange);

		applyAlgorithm("Y'");
		buildF2L_PositionEdgeColumns(Color::Orange, Color::Blue);

		applyAlgorithm("Y'");

		RubiksCubeSolverUtils::RunTimeAssert(rubiksCube_.IsFaceSolved(Down), "F2L failed");
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildOLL()
	{
		// Step 1: build yellow cross on top face

		if (rubiksCube_.getSize() >= 3)
		{
			while (true)
			{
				Cube currentCube;
				Color c, c1, c2, c3, c4;
				string algo("FRUR'U'F'");

				/*
				Top Face
				*   c1  *
				c4  c   c2
				*   c3  *

				*/

				//Check if aleady at position
				//currentCube = rubiksCube_.GetCube(1, 2, 1);
				currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 2);
				c = currentCube.GetFaceColor(Face::Up);
				//currentCube = rubiksCube_.GetCube(1, 2, 0);
				currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 1);
				c1 = currentCube.GetFaceColor(Face::Up);
				//currentCube = rubiksCube_.GetCube(2, 2, 1);
				currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 2);
				c2 = currentCube.GetFaceColor(Face::Up);
				//currentCube = rubiksCube_.GetCube(1, 2, 2);
				currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Front, 1);
				c3 = currentCube.GetFaceColor(Face::Up);
				//currentCube = rubiksCube_.GetCube(0, 2, 1);
				currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 2);
				c4 = currentCube.GetFaceColor(Face::Up);
				if (c1 == Color::Yellow
					&& c2 == Color::Yellow
					&& c3 == Color::Yellow
					&& c4 == Color::Yellow) // We are sure that c is Color::Yellow
					break;

				if (c1 == Color::Yellow && c3 == Color::Yellow)
					applyAlgorithm("U");

				if (c2 == Color::Yellow && c4 == Color::Yellow)
				{
					applyAlgorithm(algo);
					continue;
				}

				if (c1 == Color::Yellow && c2 == Color::Yellow)
					applyAlgorithm("U'");
				else if (c2 == Color::Yellow && c3 == Color::Yellow)
					applyAlgorithm("U2");
				else if (c3 == Color::Yellow && c4 == Color::Yellow)
					applyAlgorithm("U");

				if (c1 == Color::Yellow && c4 == Color::Yellow)
				{
					applyAlgorithm(algo + algo);
					continue;
				}

				// Do the sequence once if none of above was executed
				applyAlgorithm(algo);
			}
		}

		// Step 2: get all yellow on top face
		while (true)
		{
			Cube currentCube;
			Color c1, c2, c3, c4, c5, c6, c7, c8, c9;
			Color s1, s2, s3, s4, s5, s6, s7, s8;
			string algo("RUR'URUUR'");

			/*
			Top Face
			s2      s3
			s1 c1  c2  c3 s4
			   c4  c5  c6
			s8 c7  c8  c9 s5
			s7      s6
			*/

			//Check if aleady at position
			//currentCube = rubiksCube_.GetCube(0, 2, 0);
			currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 1);
			c1 = currentCube.GetFaceColor(Face::Up);
			s1 = currentCube.GetFaceColor(Face::Left);
			s2 = currentCube.GetFaceColor(Face::Back);
			//currentCube = rubiksCube_.GetCube(1, 2, 0);
			//currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 1);
			//c2 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(2, 2, 0);
			currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Back, 1);
			c3 = currentCube.GetFaceColor(Face::Up);
			s3 = currentCube.GetFaceColor(Face::Back);
			s4 = currentCube.GetFaceColor(Face::Right);
			//currentCube = rubiksCube_.GetCube(0, 2, 1);
			//currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 2);
			//c4 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(1, 2, 1);
			//currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 2);
			//c5 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(2, 2, 1);
			//currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 2);
			//c6 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(0, 2, 2);
			currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Front, 1);
			c7 = currentCube.GetFaceColor(Face::Up);
			s7 = currentCube.GetFaceColor(Face::Front);
			s8 = currentCube.GetFaceColor(Face::Left);
			//currentCube = rubiksCube_.GetCube(1, 2, 2);
			//currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Front, 1);
			//c8 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(2, 2, 2);
			currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 1);
			c9 = currentCube.GetFaceColor(Face::Up);
			s5 = currentCube.GetFaceColor(Face::Right);
			s6 = currentCube.GetFaceColor(Face::Front);
			if (c1 == Color::Yellow
				//&& c2 == Color::Yellow
				&& c3 == Color::Yellow
				//&& c4 == Color::Yellow
				//&& c5 == Color::Yellow
				//&& c6 == Color::Yellow
				&& c7 == Color::Yellow
				//&& c8 == Color::Yellow
				&& c9 == Color::Yellow)
				break;

			int numYellowCorners = 0;
			if (c1 == Color::Yellow)
				++numYellowCorners;
			if (c3 == Color::Yellow)
				++numYellowCorners;
			if (c7 == Color::Yellow)
				++numYellowCorners;
			if (c9 == Color::Yellow)
				++numYellowCorners;

			int* n1 = nullptr;
			switch (numYellowCorners)
			{
			case 0:
				if (s6 == Color::Yellow)
					applyAlgorithm("U");
				else if (s4 == Color::Yellow)
					applyAlgorithm("U2");
				else if (s2 == Color::Yellow)
					applyAlgorithm("U'");
				else if (s8 == Color::Yellow)
					applyAlgorithm(""); // do nothing
				else
				{
					int* n = nullptr;
					*n = 10; //we should not come here
				}
				break;
			case 1: // After this you will go to case 1 again or you would be done
				if (c9 == Color::Yellow)
					applyAlgorithm("U");
				else if (c3 == Color::Yellow)
					applyAlgorithm("U2");
				else if (c1 == Color::Yellow)
					applyAlgorithm("U'");
				else if (c7 == Color::Yellow)
					applyAlgorithm(""); // do nothing
				else
				{
					int* n = nullptr;
					*n = 10; //we should not come here
				}
				break;
			case 2: // After this you will go to case 1 
				if (s5 == Color::Yellow)
					applyAlgorithm("U");
				else if (s3 == Color::Yellow)
					applyAlgorithm("U2");
				else if (s1 == Color::Yellow)
					applyAlgorithm("U'");
				else if (s7 == Color::Yellow)
					applyAlgorithm(""); // do nothing
				else
				{
					int* n = nullptr;
					*n = 10; //we should not come here
				}
				break;
			case 3:
				*n1 = 10; //we should not come here
				break;
			default:
				*n1 = 10; //we should not come here
				break;
			}

			// Do the sequence once and continue
			applyAlgorithm(algo);
		}
	}

	void RubiksCubeModel_v5::RubiksCubeSolver_NxNxN::buildPLL()
	{
		//Step 1
		while (true)
		{
			Cube currentCube;
			//Color c1, c2, c3, c4, c5, c6, c7, c8, c9;
			Color s1, s2, s3, s4, s5, s6, s7, s8;
			string algo("RB'RFFR'BRFFRR");

			/*
            Top Face
               s2  o1  s3
			s1 c1  c2  c3 s4
			o4 c4  c5  c6 o2
			s8 c7  c8  c9 s5
			   s7  o3  s6
			*/

			//Check if aleady at position
			//currentCube = rubiksCube_.GetCube(0, 2, 0);
			currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 1);
			//c1 = currentCube.GetFaceColor(Face::Up);
			s1 = currentCube.GetFaceColor(Face::Left);
			s2 = currentCube.GetFaceColor(Face::Back);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 0);
			//c2 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(2, 2, 0);
			currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Back, 1);
			//c3 = currentCube.GetFaceColor(Face::Up);
			s3 = currentCube.GetFaceColor(Face::Back);
			s4 = currentCube.GetFaceColor(Face::Right);
			//currentCube = Scene::getInstance().g_cCube.GetCube(0, 2, 1);
			//c4 = currentCube.GetFaceColor(Face::Up);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 1);
			//c5 = currentCube.GetFaceColor(Face::Up);
			//currentCube = Scene::getInstance().g_cCube.GetCube(2, 2, 1);
			//c6 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(0, 2, 2);
			currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Front, 1);
			//c7 = currentCube.GetFaceColor(Face::Up);
			s7 = currentCube.GetFaceColor(Face::Front);
			s8 = currentCube.GetFaceColor(Face::Left);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 2);
			//c8 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(2, 2, 2);
			currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 1);
			//c9 = currentCube.GetFaceColor(Face::Up);
			s5 = currentCube.GetFaceColor(Face::Right);
			s6 = currentCube.GetFaceColor(Face::Front);
			if (s2 == s3
				&& s4 == s5
				&& s6 == s7
				&& s8 == s1)
			{
				//Match centers and corners before proceeding
				//Get centers
				Color o1, o2, o3, o4;
				//Color o1 = Color(Face::Back);
				//Color o2 = Color(Face::Right);
				//Color o3 = Color(Face::Front);
				//Color o4 = Color(Face::Left);
				if (rubiksCube_.getSize() >= 3)
				{
					//currentCube = rubiksCube_.GetCube(1, 1, 0);
					currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 2, Face::Back, 1);
					o1 = currentCube.GetFaceColor(Face::Back);
					//currentCube = rubiksCube_.GetCube(2, 1, 1);
					currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Back, 2);
					o2 = currentCube.GetFaceColor(Face::Right);
					//currentCube = rubiksCube_.GetCube(1, 1, 2);
					currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 2, Face::Front, 1);
					o3 = currentCube.GetFaceColor(Face::Front);
					//currentCube = rubiksCube_.GetCube(0, 1, 1);
					currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 2, Face::Back, 2);
					o4 = currentCube.GetFaceColor(Face::Left);
				}
				else
				{
					currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 1, Face::Back, 1);
					o1 = currentCube.GetFaceColor(Face::Back);
					//currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Back, 2);
					o2 = currentCube.GetFaceColor(Face::Right);
					currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 1, Face::Front, 1);
					o3 = currentCube.GetFaceColor(Face::Front);
					//currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 2, Face::Back, 2);
					o4 = currentCube.GetFaceColor(Face::Left);
				}

				if (o1 == s4)
					applyAlgorithm("U'");
				else if (o1 == s6)
					applyAlgorithm("U2");
				else if (o1 == s8)
					applyAlgorithm("U");

				break;
			}

			//Rotate the complete cube to set the face "having two corner piece color same" as front face
			if (s4 == s5)
				applyAlgorithm("Y'");
			else if (s2 == s3)
				applyAlgorithm("Y2");
			else if (s1 == s8)
				applyAlgorithm("Y");

			applyAlgorithm(algo);
		}

		//Step 2
		while (true)
		{
			Cube currentCube;
			//Color c1, c2, c3, c4, c5, c6, c7, c8, c9;
			//Color s1, s2, s3, s4, s5, s6, s7, s8;
			Color e1, e2, e3, e4;
			Color s4, s6, s8;
			string algo("RU'RURURU'R'U'RR");

			//Get centers
			/*
			            o1
			            e1
			        c1  c2  c3
			o4  e4  c4  c5  c6 e2 o2
			        c7  c8  c9
			            e3
			            o3
			*/
			Color o1, o2, o3, o4;
			if (rubiksCube_.getSize() >= 3)
			{
				//currentCube = rubiksCube_.GetCube(1, 1, 0);
				currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 2, Face::Back, 1);
				o1 = currentCube.GetFaceColor(Face::Back);
				//currentCube = rubiksCube_.GetCube(2, 1, 1);
				currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Back, 2);
				o2 = currentCube.GetFaceColor(Face::Right);
				//currentCube = rubiksCube_.GetCube(1, 1, 2);
				currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Down, 2, Face::Front, 1);
				o3 = currentCube.GetFaceColor(Face::Front);
				//currentCube = rubiksCube_.GetCube(0, 1, 1);
				currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 2, Face::Back, 2);
				o4 = currentCube.GetFaceColor(Face::Left);
			}
			else
			{
				currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 1, Face::Back, 1);
				o1 = currentCube.GetFaceColor(Face::Back);
				//currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Down, 2, Face::Back, 2);
				o2 = currentCube.GetFaceColor(Face::Right);
				currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 1, Face::Front, 1);
				o3 = currentCube.GetFaceColor(Face::Front);
				//currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Down, 2, Face::Back, 2);
				o4 = currentCube.GetFaceColor(Face::Left);
			}

			/*
			Top Face
			            o1
			            e1
			        c1  c2  c3
			o4  e4  c4  c5  c6 e2 o2
			        c7  c8  c9
			            e3
			            o3
			*/

			//Check if aleady at position
			//currentCube = Scene::getInstance().g_cCube.GetCube(0, 2, 0);
			//c1 = currentCube.GetFaceColor(Face::Up);
			//s1 = currentCube.GetFaceColor(Face::Left);
			//s2 = currentCube.GetFaceColor(Face::Back);
			//currentCube = rubiksCube_.GetCube(1, 2, 0);
			currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Back, 1);
			//c2 = currentCube.GetFaceColor(Face::Up);
			e1 = currentCube.GetFaceColor(Face::Back);
			//currentCube = Scene::getInstance().g_cCube.GetCube(2, 2, 0);
			//c3 = currentCube.GetFaceColor(Face::Up);
			//s3 = currentCube.GetFaceColor(Face::Back);
			//s4 = currentCube.GetFaceColor(Face::Right);
			//currentCube = rubiksCube_.GetCube(0, 2, 1);
			currentCube = rubiksCube_.GetCube(Face::Left, 1, Face::Up, 1, Face::Back, 2);
			//c4 = currentCube.GetFaceColor(Face::Up);
			e4 = currentCube.GetFaceColor(Face::Left);
			//currentCube = Scene::getInstance().g_cCube.GetCube(1, 2, 1);
			//c5 = currentCube.GetFaceColor(Face::Up);
			//currentCube = rubiksCube_.GetCube(2, 2, 1);
			currentCube = rubiksCube_.GetCube(Face::Right, 1, Face::Up, 1, Face::Front, 2);
			//c6 = currentCube.GetFaceColor(Face::Up);
			e2 = currentCube.GetFaceColor(Face::Right);
			//currentCube = Scene::getInstance().g_cCube.GetCube(0, 2, 2);
			//c7 = currentCube.GetFaceColor(Face::Up);
			//s7 = currentCube.GetFaceColor(Face::Front);
			//s8 = currentCube.GetFaceColor(Face::Left);
			//currentCube = rubiksCube_.GetCube(1, 2, 2);
			currentCube = rubiksCube_.GetCube(Face::Left, 2, Face::Up, 1, Face::Front, 1);
			//c8 = currentCube.GetFaceColor(Face::Up);
			e3 = currentCube.GetFaceColor(Face::Front);
			//currentCube = Scene::getInstance().g_cCube.GetCube(2, 2, 2);
			//c9 = currentCube.GetFaceColor(Face::Up);
			//s5 = currentCube.GetFaceColor(Face::Right);
			//s6 = currentCube.GetFaceColor(Face::Front);

			//Match centers with corners, they may be misaligned after few iterations of last algo
			//if(o1 == s4)
			//	RubiksCubeAlgoExecuter::executeAlgorithm("U'");
			//else if(o1 == s6)
			//	RubiksCubeAlgoExecuter::executeAlgorithm("U2");
			//else if(o1 == s8)
			//	RubiksCubeAlgoExecuter::executeAlgorithm("U");

			if (e1 == o1
				&& e2 == o2
				&& e3 == o3
				&& e4 == o4)
			{
				break;
			}

			//Move the completed face at back
			if (e2 == o2)
				applyAlgorithm("Y");
			else if (e3 == o3)
				applyAlgorithm("Y2");
			else if (e4 == o4)
				applyAlgorithm("Y'");

			applyAlgorithm(algo);
		}
	}
}

