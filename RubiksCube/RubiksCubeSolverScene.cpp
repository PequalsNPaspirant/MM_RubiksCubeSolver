// RubiksCube.cpp : Defines the entry point for the application.
//
#include "stdafx.h"

#include "Resource.h"
#include <windows.h>
#include <windowsx.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "RubiksCubeSolverScene.h"
#include "RubiksCubeSimulator.h"
#include "RubiksCubeSolver_v1.h"
#include "RubiksCubeSolverTest.h"

namespace mm {

	GLuint RubiksCubeSolverScene::g_pSelectBuffer[RubiksCubeSolverScene::SELECT_BUFFER_SIZE];
	const int RUBIKS_CUBE_SIZE = 3;

	RubiksCubeSolverScene::RubiksCubeSolverScene()
		: g_cCube(RUBIKS_CUBE_SIZE)
	{
	}

	void RubiksCubeSolverScene::initOpenGl(int nWidth, int nHeight)
	{
		sizeOpenGlScreen(nWidth, nHeight);

		glPolygonMode(GL_FRONT, GL_FILL);
		glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

		//glEnable(GL_COLOR_MATERIAL);
		//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		glClearDepth(1.0f);											// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do
		glLineWidth(LINE_WIDTH);									// Set outline width

		loadTexture(IDB_WHITE, &g_pTextures[0]);
		loadTexture(IDB_BLUE, &g_pTextures[1]);
		loadTexture(IDB_ORANGE, &g_pTextures[2]);
		loadTexture(IDB_RED, &g_pTextures[3]);
		loadTexture(IDB_GREEN, &g_pTextures[4]);
		loadTexture(IDB_YELLOW, &g_pTextures[5]);
		loadTexture(IDB_BLACK, &g_pTextures[6]);
	}

	void RubiksCubeSolverScene::loadTexture(int nId, GLuint* texture)
	{
		// bitmap handle
		HBITMAP hBMP;

		// bitmap struct
		BITMAP   bmp;

		glGenTextures(1, texture);    // Create The Texture 
		hBMP = (HBITMAP)LoadImage(
			GetModuleHandle(NULL),
			MAKEINTRESOURCE(nId),
			IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		GetObject(hBMP, sizeof(bmp), &bmp);

		// Pixel Storage Mode (Word Alignment / 4 Bytes) 
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		// bind to the texture ID
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			3,
			bmp.bmWidth, bmp.bmHeight,
			0,
			GL_BGR_EXT,
			GL_UNSIGNED_BYTE,
			bmp.bmBits
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		DeleteObject(hBMP);
	}

	void RubiksCubeSolverScene::sizeOpenGlScreen(int nWidth, int nHeight)
	{
		if (nHeight == 0)
		{
			nHeight = 1;
		}

		glViewport(0, 0, nWidth, nHeight);
		glMatrixMode(GL_PROJECTION);
		//glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		setFrustum(nWidth, nHeight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void RubiksCubeSolverScene::setFrustum(int nWidth, int nHeight)
	{
		GLdouble left, right;
		GLdouble top, bottom;

		if (nWidth < nHeight)
		{
			left = -1.0;
			right = 1.0;
			top = (double)nHeight / (double)nWidth;
			bottom = -top;

		}
		else
		{
			top = 1.0;
			bottom = -1.0;
			right = (double)nWidth / (double)nHeight;
			left = -right;
		}

		glFrustum(left, right, bottom, top, 5.0, 100.0);
	}

	void RubiksCubeSolverScene::initScene()
	{
		g_cCamera.SetDistance(45.0);
		g_cCamera.SetPhi((float)(PI / 4));
		g_cCamera.SetTheta((float)(PI / 4));

		//g_cCube.Randomize();
	}

	void RubiksCubeSolverScene::renderScene()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
		glLoadIdentity();									// Reset The View

		CVector3 pos = g_cCamera.GetPosition();
		CVector3 lookAt = g_cCamera.GetLookAt();
		CVector3 up = g_cCamera.GetUp();

		gluLookAt(
			pos.x, pos.y, pos.z,
			lookAt.x, lookAt.y, lookAt.z,
			up.x, up.y, up.z
		);

#ifdef _DEBUG
		// Draw Axis
		glBegin(GL_LINES);
		// x
		glColor3f(1.0f, 0.6f, 0.0f); // orange
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(CUBE_SIZE * 3, 0.0, 0.0);
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glVertex3d(CUBE_SIZE * 3, 0.0, 0.0);
		glVertex3d(CUBE_SIZE * 4.5f, 0.0, 0.0);

		// y
		//glColor3f(0.0f, 1.0f, 0.0f);  // green
		glColor3f(1.0f, 1.0f, 1.0f);  // white
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, CUBE_SIZE * 3, 0.0);
		glColor3f(1.0f, 1.0f, 0.0f);  // yellow
		glVertex3d(0.0, CUBE_SIZE * 3, 0.0);
		glVertex3d(0.0, CUBE_SIZE * 4.5f, 0.0);

		// z
		glColor3f(0.0f, 1.0f, 0.0f);  // green
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, CUBE_SIZE * 3);
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glVertex3d(0.0, 0.0, CUBE_SIZE * 3);
		glVertex3d(0.0, 0.0, CUBE_SIZE * 4.5f);
		glEnd();
#endif

		glEnable(GL_LIGHTING);
		float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float* position = new float[4];

		g_cCamera.GetPosition().ToFloatArray(position);

		glLightfv(GL_LIGHT1, GL_AMBIENT, color);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
		glLightfv(GL_LIGHT1, GL_POSITION, position);
		glLightfv(GL_LIGHT1, GL_SPECULAR, color);
		glLightfv(GL_LIGHT1, GL_SHININESS, color);

		delete[] position;
		position = NULL;

		glEnable(GL_LIGHT1);

		float shininess = 5.0f;
		glMaterialfv(GL_FRONT, GL_SPECULAR, color);
		//glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

		renderRubiksCube();

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHTING);

		//SwapBuffers(g_hDC);
	}

	void RubiksCubeSolverScene::renderRubiksCube()
	{
		glInitNames();

		for (int i = 0; i < g_cCube.getSize(); i++)
		{
			for (int j = 0; j < g_cCube.getSize(); j++)
			{
				for (int k = 0; k < g_cCube.getSize(); k++)
				{
					glPushMatrix();

					if (g_cCube.g_bRotating)
					{
						if (g_cCube.g_vRotationAxis.x && i == g_cCube.g_nRotatingSection ||
							g_cCube.g_vRotationAxis.y && j == g_cCube.g_nRotatingSection ||
							g_cCube.g_vRotationAxis.z && k == g_cCube.g_nRotatingSection)
						{
							int angle = g_cCube.g_bFlipRotation ? -g_cCube.g_nRotationAngle : g_cCube.g_nRotationAngle;
							glRotated(angle, g_cCube.g_vRotationAxis.x, g_cCube.g_vRotationAxis.y, g_cCube.g_vRotationAxis.z);
						}
					}

					renderIndividualCube(g_cCube.GetCube(i, j, k), i, j, k);

					glPopMatrix();
				}
			}
		}
	}

	void RubiksCubeSolverScene::renderIndividualCube(const Cube& pCube, int x, int y, int z)
	{
		glPushName(x);
		glPushName(y);
		glPushName(z);

		// scale to -1 to +1
		x--;
		y--;
		z--;

		glPushMatrix();

		glTranslated(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);

		ColorRGB top = pCube.GetFaceColorRGB(Top);
		ColorRGB bottom = pCube.GetFaceColorRGB(Bottom);
		ColorRGB left = pCube.GetFaceColorRGB(Left);
		ColorRGB right = pCube.GetFaceColorRGB(Right);
		ColorRGB back = pCube.GetFaceColorRGB(Back);
		ColorRGB front = pCube.GetFaceColorRGB(Front);

		glEnable(GL_TEXTURE_2D);

		// Front Face
		glPushName((GLuint)Front);
		glBindTexture(GL_TEXTURE_2D, getTextureID(front));
		glBegin(GL_QUADS);
		glColor3ub(front.r, front.g, front.b);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
		glEnd();
		glPopName();

		// Back Face
		glPushName((GLuint)Back);
		glBindTexture(GL_TEXTURE_2D, getTextureID(back));
		glBegin(GL_QUADS);
		glColor3ub(back.r, back.g, back.b);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
		glPopName();

		// Top Face
		glPushName((GLuint)Top);
		glBindTexture(GL_TEXTURE_2D, getTextureID(top));
		glBegin(GL_QUADS);
		glColor3ub(top.r, top.g, top.b);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glEnd();
		glPopName();

		// Bottom Face
		glPushName((GLuint)Bottom);
		glBindTexture(GL_TEXTURE_2D, getTextureID(bottom));
		glBegin(GL_QUADS);
		glColor3ub(bottom.r, bottom.g, bottom.b);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glEnd();
		glPopName();

		// Right face
		glPushName((GLuint)Right);
		glBindTexture(GL_TEXTURE_2D, getTextureID(right));
		glBegin(GL_QUADS);
		glColor3ub(right.r, right.g, right.b);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2d(1.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2d(0.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
		glEnd();
		glPopName();

		// Left Face
		glPushName((GLuint)Left);
		glBindTexture(GL_TEXTURE_2D, getTextureID(left));
		glBegin(GL_QUADS);
		glColor3ub(left.r, left.g, left.b);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2d(0.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2d(1.0, 0.0); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2d(1.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2d(0.0, 1.0); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glEnd();
		glPopName();

		glPopName();
		glPopName();
		glPopName();

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	GLuint RubiksCubeSolverScene::getTextureID(ColorRGB color)
	{
		if (color == ColorRGB::BLACK) 
			return g_pTextures[6];
		else if (color == ColorRGB(255, 255, 255)) // TODO: replace colours by static objects like above
			return g_pTextures[0];
		else if (color == ColorRGB(0, 0, 255))
			return g_pTextures[1];
		else if (color == ColorRGB(255, 165, 0))
			return g_pTextures[2];
		else if (color == ColorRGB(255, 0, 0))
			return g_pTextures[3];
		else if (color == ColorRGB(0, 255, 0))
			return g_pTextures[4];
		else if (color == ColorRGB(255, 255, 0))
			return g_pTextures[5];
		else
			return g_pTextures[6];
	}

	INT RubiksCubeSolverScene::getSelectedObjects(int x, int y, int nWidth, int nHeight)
	{
		GLint viewport[4];

		glSelectBuffer(SELECT_BUFFER_SIZE, g_pSelectBuffer);
		glRenderMode(GL_SELECT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glGetIntegerv(GL_VIEWPORT, viewport);
		gluPickMatrix(x, viewport[3] - y, 1, 1, viewport);

		setFrustum(nWidth, nHeight);

		glMatrixMode(GL_MODELVIEW);

		renderScene();

		// restoring the original projection matrix
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glFlush();

		return glRenderMode(GL_RENDER);
	}

	CVector3 RubiksCubeSolverScene::mapCoordinates(int x, int y)
	{
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		return CVector3(posX, posY, posZ);
	}

	void RubiksCubeSolverScene::getCubeSelection(int *x, int *y, int *z, Face *face, int g_nHitCount)
	{
		GLuint names, *ptr, minZ, *ptrNames, numberOfNames;
		ptr = (GLuint *)g_pSelectBuffer;
		minZ = 0xffffffff;

		for (int i = 0; i < g_nHitCount && i < SELECT_BUFFER_SIZE; i++)
		{
			names = *ptr;
			ptr++;
			if (*ptr < minZ)
			{
				numberOfNames = names;
				minZ = *ptr;
				ptrNames = ptr + 2;
			}

			ptr += names + 2;
		}

		*x = ptrNames[0];
		*y = ptrNames[1];
		*z = ptrNames[2];
		*face = (Face)ptrNames[3];
	}

	/*
	void RubiksCubeSolverScene::deInit()
	{
		if (g_hRC)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(g_hRC);
		}

		if (g_hDC)
			ReleaseDC(g_hWnd, g_hDC);

		if (g_bFullScreen)
			ChangeDisplaySettings(NULL, 0);

		UnregisterClass(g_szWindowClass, g_hInstance);

	}
	*/
}

