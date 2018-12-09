#pragma once

const int RUBIKS_CUBE_SIZE = 3;
typedef unsigned char byte;
typedef byte BYTE;

enum Face
{
	Top = 0,
	Bottom,
	Left,
	Right,
	Front,
	Back,
};

struct CColor
{
	CColor(byte red, byte green, byte blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	CColor()
	{
		r = g = b = 0;
	}

	byte r;
	byte g;
	byte b;

	BOOL operator==(const CColor& c)
	{
		return (r == c.r) && (g == c.g) && (b == c.b);
	}

	BOOL operator!=(const CColor& c)
	{
		return !((r == c.r) && (g == c.g) && (b == c.b));
	}
};

class CCubeState
{
public:
	CCubeState(CColor cTop, CColor cBottom, CColor cLeft,
		CColor cRight, CColor cFront, CColor cBack);
	~CCubeState(void);
	CColor GetFaceColor(Face eFace);
	void TiltUp();
	void TiltDown();
	void TurnLeft();
	void TurnRight();
	void TiltLeft();
	void TiltRight();


protected:
	CColor * m_pFaces;
};


class CRubiksCube
{
public:
	CRubiksCube(void);
	~CRubiksCube(void);
	CCubeState* GetCube(int x, int y, int z);
	void Rotate(int section, int turns);	// around y axis
	void Tilt(int section, int turns);	// around x axis
	void Turn(int section, int turns);	// around z axis
	void Randomize();
	BOOL IsSolved();

	VOID RotateWholeRubiksCube(int axis, int turns);

protected:
	CCubeState*** *m_pCubes;
	BOOL IsValidCube(int x, int y, int z);

private:
	CCubeState* CreateCube(int x, int y, int z);
	BOOL IsFaceSolved(Face face);
};	
